#include "huffman.h"
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <queue>
#include <iostream>
#include <iterator>

using namespace std;

template<class Char>
bool bit(Char c, int i) { 
    return ((static_cast<unsigned char>(c) >> i) & 1) == 1; 
}

int read_int(istream &in, int num_bytes = sizeof(int)) {
    int res = 0;
    for (int i = 0; i < num_bytes; ++i) {
        res |= (static_cast<int>(in.get()) << (i * kBitsInByte));
        if (in.fail())
            throw "error: reading source file failed, maybe the file is wrong";
    }
    return res;
}

void write_int(ostream &out, int a, int num_bytes = sizeof(int)) {
    for (int i = 0; i < num_bytes; ++i)
        out.put(static_cast<char>((a >> (i * kBitsInByte)) & 
                                  ((1 << kBitsInByte) - 1)));
}

void open_files(ifstream &in, ofstream &out, 
        const string &file_in, const string &file_out) {
    in.open(file_in, ios::binary | ios::in);
    if (in.fail()) {
        cerr << "error: couldn't open source file\n";
        exit(2);
    }
    if (out.fail()) {
        cerr << "error: couldn't open output file\n";
        exit(2);
    }
    out.open(file_out, ios::binary | ios::out);
}

BinaryTrieNode::BinaryTrieNode(char character_) : character(character_) { }

void BinaryTrie::build(vector<int> &frequency) {
    typedef pair<pair<int, char> , shared_ptr<BinaryTrieNode> > HeapNode;
    priority_queue<HeapNode, vector<HeapNode>, greater<HeapNode>> heap; 
    for (int i = 0; i < kAlphabetSize; ++i)
        if (frequency[i])
            heap.push(make_pair(make_pair(frequency[i], static_cast<char>(i)), 
                        shared_ptr<BinaryTrieNode>(
                            new BinaryTrieNode(static_cast<char>(i)))));
    if (heap.size() == 0) { // empty file
        root = shared_ptr<BinaryTrieNode>();
        codes.clear();
        return;
    }
    if (heap.size() == 1) {
        root = shared_ptr<BinaryTrieNode>(new BinaryTrieNode);
        root->left = heap.top().second;
    } else {
        while (heap.size() > 1) {
            auto node1 = heap.top();
            heap.pop();
            auto node2 = heap.top();
            heap.pop();
            auto new_node = shared_ptr<BinaryTrieNode>(new BinaryTrieNode);
            new_node->left = node1.second;
            new_node->right = node2.second;
            heap.push(make_pair(make_pair(node1.first.first + node2.first.first,
                            min(node1.first.second, node2.first.second)), 
                        new_node));
        }
        root = heap.top().second;
    }

    codes.assign(kAlphabetSize, vector<bool>());
    vector<bool> trie_path;
    traverse(root, trie_path);
}

vector<bool> BinaryTrie::getCode(char character) {
    return codes[static_cast<unsigned char>(character)];
}

char BinaryTrie::nextCharacter(vector<bool> &rev_input) {
    shared_ptr<BinaryTrieNode> current = root;
    while (current->left || current->right) {
        if (rev_input.empty())
            throw "error: reading source file failed, maybe the file is wrong";
        if (rev_input.back())
            current = current->right;
        else
            current = current->left;
        rev_input.pop_back();
    }
    return current->character;
}

void
BinaryTrie::traverse(shared_ptr<BinaryTrieNode> node, vector<bool> &current) {
    if (!node->left && !node->right) {
        codes[static_cast<unsigned char>(node->character)] = 
            current;
    } else {
        current.push_back(false);
        if (node->left)
            traverse(node->left, current);
        current.back() = true;
        if (node->right)
            traverse(node->right, current);
        current.pop_back();
    }
}

void Huffman::compress(const string &file_in, const string &file_out) {
    ifstream in;
    ofstream out;
    open_files(in, out, file_in, file_out);

    frequency.assign(kAlphabetSize, 0);
    char byte;
    while (in.read(&byte, 1))
        ++frequency[static_cast<unsigned char>(byte)];
    BinaryTrie binary_trie;
    binary_trie.build(frequency);

    vector<bool> output;
    in.clear();
    in.seekg(0, ios::beg);
    while (in.read(&byte, 1)) {
        auto code = binary_trie.getCode(byte);
        output.insert(end(output), begin(code), end(code));
    }

    for (int i = 0; i < kAlphabetSize; ++i)
        write_int(out, frequency[i], kBytesPerFrequencyEntry);
    int total_length = static_cast<int>(output.size());
    int alignment = (total_length % kBitsInByte == 0) ? 0 :
            (kBitsInByte - total_length % 8);
    for (int i = 0; i < alignment; ++i)
        output.push_back(false);
    write_int(out, alignment, 1);
    for (int i = 0; i < static_cast<int>(output.size()); i += kBitsInByte) {
        unsigned char temp = 0;
        for (int j = 0; j < kBitsInByte; ++j)
            if (output[i + j])
                temp |= (1 << j);
        out.put(static_cast<char>(temp));
    }

    in.clear();
    in.seekg(0, ios::end);
    cerr << in.tellg() << '\n' << (total_length + alignment) / kBitsInByte << 
        '\n' << 1 + kAlphabetSize * kBytesPerFrequencyEntry << '\n';
}

void Huffman::uncompress(const string &file_in, const string &file_out) {
    ifstream in;
    ofstream out;
    open_files(in, out, file_in, file_out);

    frequency.assign(kAlphabetSize, 0);
    for (int i = 0; i < kAlphabetSize; ++i)
        frequency[i] = read_int(in, kBytesPerFrequencyEntry);
    BinaryTrie binary_trie;
    binary_trie.build(frequency);

    vector<bool> input;
    int alignment = read_int(in, 1);
    char byte;
    while (in.read(&byte, 1)) {
        for (int i = 0; i < kBitsInByte; ++i)
            input.push_back(bit(byte, i));
    }
    int input_size = input.size();
    if (input_size < alignment)
            throw "error: reading source file failed, maybe the file is wrong";
    for (int i = 0; i < alignment; ++i)
        input.pop_back();
    reverse(begin(input), end(input));

    int total_length = 0;
    while (input.size()) {
        out.put(binary_trie.nextCharacter(input));
        ++total_length;
    }

    cerr << input_size / kBitsInByte << '\n' << 
        total_length << '\n' <<
        1 + kAlphabetSize * kBytesPerFrequencyEntry << '\n';
}

void Arguments::parse(int argc, char **argv) {
    bool command_provided = false;
    bool file_in_provided = false;
    bool file_out_provided = false;
    for (int i = 1; i < argc; ++i)
    {
        if (!strcmp(argv[i], "-c") || !strcmp(argv[i], "-u")) {
            if (command_provided)
                throw "error: only one command allowed";
            command_provided = true;
            if (argv[i][1] == 'c')
                compress = true;
            else
                compress = false;
        } else if (!strcmp(argv[i], "-f") || !strcmp(argv[i], "--file")) {
            if (file_in_provided)
                throw "error: only one source file allowed";
            file_in_provided = true;
            ++i;
            if (i == argc)
                throw "error: -f/--file must have an argument";
            file_in = string(argv[i]);
        } else if (!strcmp(argv[i], "-o") || !strcmp(argv[i], "--output")) {
            if (file_out_provided)
                throw "error: only one output file allowed";
            file_out_provided = true;
            ++i;
            if (i == argc)
                throw "error: -o/--output must have an argument";
            file_out = string(argv[i]);
        }
        else
            throw "error: unknown command line argument";
    }
    if (!command_provided)
        throw "error: no command provided";
    if (!file_in_provided)
        throw "error: no source file provided";
    if (!file_out_provided)
        throw "error: no output file provided";
}
