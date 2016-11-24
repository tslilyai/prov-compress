#include <queue>
#include <set>

#include "graph.hh"

using namespace std;

vector<Node_Id> Graph::get_all_descendants(Node_Id node) {
    return bfs_helper(node,
            [this](Node_Id nid) { return get_outgoing_edges(nid); });
}

vector<Node_Id> Graph::bfs_helper(Node_Id node,
        function<vector<Node_Id>(Node_Id)> get_neighbors) {
    vector<Node_Id> neighbors = get_neighbors(node);
    queue<Node_Id> q;
    set<Node_Id> visited;
    for (Node_Id neighbor : neighbors) {
        q.push(neighbor);
        visited.insert(neighbor);
    }
    while (!q.empty()) {
        Node_Id nid = q.front();
        q.pop();
        for (Node_Id nid2 : get_neighbors(nid)) {
            if (!visited.count(nid2)) {
                visited.insert(nid2);
                q.push(nid2);
            }
        }
    }
    return vector<Node_Id>(visited.begin(), visited.end());
}
