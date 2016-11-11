'''
Given a JSON file with Camflow-style provenance data, constructs a provenance
graph and a dictionary of metadata.
Provides functions to output data in different formats for dot/gspan processing.
'''
import json

DICT_BEGIN = '{'
DICT_END = '}'
RELATIVE_NODE = '@'
VALUES_SEP = ','
IDENTIFIER_SEP = '#'
KEY_VAL_SEP = '$'
UNKNOWN = '?'

RECOGNIZED_TYPS = ["prefix", "activity", "entity", "relation", "unknown"]

class Edge:
    def __init__(self, dest, label):
        self.dest = dest
        # XXX do we want to use common strings for this?
        self.label = label

    def __repr__(self):
        return '(dest = "%s", label = "%s")' % (self.dest, self.label)

class Metadata:
    def __init__(self, typ, data):
        self.typ = typ
        self.data = data

    def __repr__(self):
        return '(typ = "%s", data = "%s")' % (self.typ, self.data)

# Returns an adjacency-list style graph and dictionary of metadata, both
# indexed by Camflow provenance identifier.
def json_to_graph_data(infile):
    metadata = {}
    graph = {}
    missing_nodes = set()
    with open(infile) as f:
        for line in f:
            i = line.find(DICT_BEGIN)
            if i == -1:
                continue
            line = line[i:]
            for typ, entries in json.loads(line).items():
                assert typ in RECOGNIZED_TYPS and typ != "unknown"
                if typ == "prefix":
                    continue
                for identifier, data in entries.items():
                    # confirm that each entry in the JSON has a unique ID.
                    assert identifier not in metadata
                    metadata[identifier] = Metadata(typ, data)
                    missing_nodes.discard(identifier)
                    if typ == "relation":
                        tail = data["cf:receiver"]
                        head = data["cf:sender"]
                        # there are relations defined where the sender ID 
                        # is never actually defined as an entity/activity
                        # add empty metadata for these
                        for v in [head, tail]:
                            if v not in metadata:
                                missing_nodes.add(v)
                        graph.setdefault(tail, []).append(
                                Edge(head, identifier))
                    else:
                        # This is just so that we have a node in the graph
                        # for every entity/activity.
                        graph.setdefault(identifier, [])
    for i, identifier in enumerate(missing_nodes):
        assert identifier not in metadata
        metadata[identifier] = Metadata("unknown", {'cf:id':-i, 'cf:type':None})
        graph.setdefault(identifier, [])
    return graph, metadata

# Returns a dictionary mapping all identifier strings for vertices
# and edges in the graph to unique integers + n/r for node/relation
def identifier_to_int(graph):
    iti = {}
    v_ctr = 0
    e_ctr = 0
    for v, edges in graph.items():
        iti[v] = str(v_ctr) + 'n'
        v_ctr += 1
        for edge in edges:
            iti[edge.label] = str(e_ctr) + 'r'
            e_ctr += 1
    return iti 

# Returns a string of the graph in DOT format. To view a file in DOT format,
# use `dot -Tps file.dot -o output.ps`.
def graph_to_dot(infile):
    graph, metadata = json_to_graph_data(infile)
    iti = identifier_to_int(graph)
    s = ["digraph prov {"]
    for v, edges in graph.items():
        s.extend(['\t"%s" -> "%s" [label="%s"];' % (
            str(metadata[v].data['cf:type']) + ", " 
                + str(metadata[v].data['cf:id']) + ', ' 
                + str(iti[v]), 
            str(metadata[edge.dest].data['cf:type']) + ", " 
                + str(metadata[edge.dest].data['cf:id']) + ', '
                + str(iti[edge.dest]),
            metadata[edge.label].data['cf:type'])
            for edge in edges])
    s.append("}")
    return "\n".join(s)

# Returns a string of the graph in gspan format. 
# Run gSpan -f provgspan -s 0.1 -o -i. Output will be located in provspan.fp
# All gSpan input needs to be ints: vertex ids, edge ids, and typ ids are 
# all mapped to ints and printed out.
def graph_to_gspan(infile):
    graph, metadata = json_to_graph_data(infile)
    iti = identifier_to_int(graph)
    s = ["t # 0"]
    vs = []
    es = []
    for v, edges in graph.items():
        vs.extend(['v %s %s' % (iti[v], RECOGNIZED_TYPS.index(metadata[v].typ))])
        es.extend(['e %s %s %s' % (iti[v], iti[edge.dest], 0) for edge in edges])
    return "\n".join(s + vs + es)

