#ifndef HUFFMAN_H_
#define HUFFMAN_H_

#include <fstream>
#include <vector>
#include <memory>

using namespace std;

const int kBitsInByte = 8;
const int kAlphabetSize = 1 << kBitsInByte;
const int kBytesPerFrequencyEntry = 3; // max input size is 5 Mb < 2^24

struct BinaryTrieNode {
    BinaryTrieNode(char character_ = 0);
    shared_ptr<BinaryTrieNode> left, right;
    char character;
};

class BinaryTrie {
public:
    void build(vector<int> &frequency);
    vector<bool> getCode(char character);
    char nextCharacter(vector<bool> &rev_input);
private:
    void traverse(shared_ptr<BinaryTrieNode> node, vector<bool> &current);
    shared_ptr<BinaryTrieNode> root;
    vector<vector<bool> > codes;
};

class Huffman {
public:
    void compress(const string &file_in, const string &file_out);
    void uncompress(const string &file_in, const string &file_out);
private:
    vector<int> frequency;
};

struct Arguments {
    void parse(int argc, char **argv);
    string file_in;
    string file_out;
    bool compress;
};

#endif
