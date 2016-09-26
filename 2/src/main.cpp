#include <cassert>
#include <iostream>
#include "huffman.h"

using namespace std;

void print_help() {
    cerr << "usage: huffman COMMAND -f SOURCE -o OUTPUT\n"
            "    COMMAND:\n"
            "        -c             compress\n"
            "        -u             uncompress\n"
            "    OPTIONS:\n"
            "        -f, --file     source file\n"
            "        -o, --output   output file\n";
}

int main(int argc, char **argv) {
    Arguments parser;
    try {
        parser.parse(argc, argv);
    } catch(const char *error_msg) {
        cerr << error_msg << '\n';
        print_help();
        return 1;
    }
    Huffman huffman;
    try {
        if (parser.compress)
            huffman.compress(parser.file_in, parser.file_out);
        else {
            huffman.uncompress(parser.file_in, parser.file_out);
        }
    } catch(const char *error_msg) {
        cerr << error_msg << '\n';
        return 2;
    }

    return 0;
}
