#ifndef META_H
#define META_H

#include "helpers.hh"

class MetadataInterface {
    virtual map<string, string> get_metadata(string& identifier) = 0;
};

class DummyMetadata : MetadataInterface {
private:
    map<string, string> id2jsonstr;

public:
    DummyMetadata(string& infile);
    void construct_metadata_dict(string& infile);
    map<string, string> get_metadata(string& identifier) override;
};


class CompressedMetadata : MetadataInterface {
    static const string PROV_DICTS_FILE;
    static const string IDENTIFIERS_FILE;
    static const string METADATA_FILE;
    static const string RELATIVE_NODE;
    static const string DEFAULT_NODE_KEY;
    static const string DEFAULT_RELATION_KEY;
    static const int MAX_STRING_SIZE_BITS = 10;

private:
    // Prov strings dictionaries
    map<unsigned char, string>node_types_dict;
    map<unsigned char, string>typ_dict;
    map<unsigned char, string>key_dict;
    map<unsigned char, string>prov_label_dict;
    map<unsigned char, string>val_dict;
    vector<string> identifiers;
    BitSet* metadata_bs;

    map<string, string>default_node_data;
    map<string, string>default_relation_data;
    map<int, size_t>intid2dataindex;

    size_t node_type_bits;
    size_t typ_bits;
    size_t key_bits;
    size_t label_bits;
    size_t val_bits;
    size_t id_bits;
    size_t num_nodes;

public:
    CompressedMetadata(string& infile);
    void construct_identifiers_dict();
    void construct_prov_dicts();
    void construct_metadata_dict(string& infile);
    map<string, string> get_metadata(string& identifier) override;
};

/*
class MetadataEntry {
public:
    MetadataEntry(unsigned char typ) : typ_(typ) {};

    void add_equal_key(unsigned char k) {
        equal_keys_.push_back(k);
    }
    void add_encoded_key_val(unsigned char k, unsigned char v) {
        encoded_key_vals_[k] = v;
    }
    void add_other_key_val(unsigned char k, string v) {
        other_key_vals_[k] = v;
    }

    vector<unsigned char> get_keys_equal() {
        return equal_keys_;
    }
    map<unsigned char, unsigned char> get_encoded() {
        return encoded_key_vals_;
    }
    map<unsigned char, string> get_other() {
        return other_key_vals_;
    }

private:
    unsigned char typ_;
    vector<unsigned char> equal_keys_;
    map<unsigned char, unsigned char> encoded_key_vals_;
    map<unsigned char, string> other_key_vals_;
};
*/

#endif
