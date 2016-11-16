#include "query.hh"

void construct_prov_dicts() {
    ifstream infile;
    string buffer;
    vector<string> data;

    infile.open(PROV_DICTS_FILE, ios::in);
    assert(infile.is_open());
    getline(infile, buffer);
    infile.close();

    data = split(buffer, DICT_END);
    assert(data.size() == 5);

    std::vector<map<unsigned char, string>*> dicts = {&key_dict, &val_dict, &prov_label_dict, &typ_dict, &node_types_dict};
    std::vector<int*> bits = {&key_bits, &val_bits, &label_bits, &typ_bits, &node_type_bits};
    for (size_t i = 0; i < data.size(); ++i) {
        set_dict_entries(*dicts[i], data[i]);
        *bits[i] = nbits_for_int(log(dicts[i]->size()));
    }
}

void construct_identifiers_dict() {
    ifstream infile;
    string buffer;

    infile.open(IDENTIFIERS_FILE, ios::in);
    assert(infile.is_open());
    getline(infile, buffer);
    infile.close();

    set_dict_entries(intid_to_id_dict, buffer);
    // create the dictionary mapping the other direction
    for (auto i = intid_to_id_dict.begin(); i != intid_to_id_dict.end(); ++i) {
        id_to_intid_dict[i->second] = i->first; 
    }
}

void construct_metadata_dict(string& buffer) {
    // TODO
    /*
    bitset<N * 8> b;
    for (int i = 0; i < buffer.length(); ++i) {
      char c = s[i];
      for (int j = 7; j >= 0 && c; --j) {
        if (c & 0x1) {
          b.set(8 * i + j);
        }
        c >>= 1;
      }
    }
    int total_size = 0;
    int cur_size = 0;

    while(1) {
        return;
        // read type bits
        // read id bits
    }
    */
}

string decode_from_default(string& identifier, vector<string>& data) {
    (void)data;
    return identifier;
}

string get_metadata(string identifier) {
    return identifier;
}

int main(int argc, char *argv[]) {
    ifstream infile;
    string buffer;
    vector<string> data;

    if (argc == 1) {
        infile.open("compressed_metadata.txt", ios::in);
    } else if (argc == 2) {
        infile.open(argv[1], ios::in);
    } else {
        cout << "Usage: ./query [infile]" << endl;
        return 1;
    }
    if (!infile.is_open()) {
        cout << "Failed to open input file" << endl;
        return 1;
    }
    getline(infile, buffer);
    infile.close();

    construct_prov_dicts();
    construct_identifiers_dict();
    construct_metadata_dict(buffer);

    return 0;
}
