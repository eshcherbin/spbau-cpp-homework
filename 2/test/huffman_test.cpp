#include "huffman_test.h"
#include "../src/huffman.h"
#include <memory.h>
#include <iostream>
#include <iomanip>

bool FileCompareTestInterface::compareFiles(const string &file_a, 
        const string &file_b) {
    ifstream in_a(file_a, ios::binary | ios::in);
    ifstream in_b(file_b, ios::binary | ios::in);
    char buffer_a[kReadSize];
    char buffer_b[kReadSize];
    while (!in_a.eof() && !in_b.eof())
    {
        in_a.read(buffer_a, kReadSize);
        in_b.read(buffer_b, kReadSize);
        if (in_a.gcount() != in_b.gcount() ||
                memcmp(buffer_a, buffer_b, in_a.gcount()))
            return false;
    }
    if (!(in_a.eof() && in_b.eof()))
        return false;
    return true;
}

bool FileCompareTestInterface::operator()() {
    this->generateTest();

    Huffman huffman;
    cerr << "Starting Huffman (" << test_file_prefix << ".src -> " << 
        test_file_prefix << ".bin -> " << test_file_prefix << ".res)\n";
    clock_t start = clock();
    huffman.compress(test_file_prefix + ".src", test_file_prefix + ".bin");
    clock_t finish = clock();
    cerr << "Compressing done in " << setprecision(2) << fixed << 
        static_cast<double>(finish - start) / CLOCKS_PER_SEC << " sec \n";
    start = clock();
    huffman.uncompress(test_file_prefix + ".bin", test_file_prefix + ".res");
    finish = clock();
    cerr << "Uncompressing done in " << setprecision(2) << fixed << 
        static_cast<double>(finish - start) / CLOCKS_PER_SEC << " sec \n";
    bool result = 
        compareFiles(test_file_prefix + ".src", test_file_prefix + ".res");
    if (result)
        cerr << "Files are identical\n\n";
    else
        cerr << "Files are different\n\n";
    return result;
}

TestBig::TestBig() {
    test_name = "Big random test";
    test_file_prefix = "big";
}

void TestBig::generateTest() {
    ofstream source_file(test_file_prefix + ".src", ios::out | ios::binary);
    srand(time(NULL));
    for (int i = 0; i < kBigFileSize; ++i)
        source_file.put(rand() % kAlphabetSize);
}

TestOneCharacter::TestOneCharacter() {
    test_name = "Big test with only one character";
    test_file_prefix = "onec";
}

void TestOneCharacter::generateTest() {
    ofstream source_file(test_file_prefix + ".src", ios::out | ios::binary);
    for (int i = 0; i < kBigFileSize; ++i)
        source_file.put(0);
    source_file.close();
}

TestEmpty::TestEmpty() {
    test_name = "Empty file test";
    test_file_prefix = "empty";
}

void TestEmpty::generateTest() {
    ofstream source_file(test_file_prefix + ".src", ios::out | ios::binary);
    source_file.close();
}

TestBinaryTrie::TestBinaryTrie() {
    test_name = "Binary trie test";
}

HuffmanIntegrationTest::HuffmanIntegrationTest() {
    test_group_name = "Integration tests";
    tests = vector<shared_ptr<TestInterface> >({
                shared_ptr<TestInterface>(new TestBig()),
                shared_ptr<TestInterface>(new TestOneCharacter()),
                shared_ptr<TestInterface>(new TestEmpty()),
            });
}

bool TestBinaryTrie::operator()() {
    cerr << "BinaryTrie test\n";
    cerr << "Testing on string \"\aaabab\\n\"\n";
    cerr << "Frequency table: 0 0 0 0 0 0 0 0 0 0 1 0 "
        "<84 skipped> 0 4 2 0 0 ...\n";
    string test_string = "aaabab\n";
    vector<int> frequency(kAlphabetSize, 0);
    for (auto c : test_string)
        ++frequency[static_cast<unsigned char>(c)];
    BinaryTrie binary_trie;
    binary_trie.build(frequency);
    bool result = true, cur_result;
    cerr << "checking code for 'a': should be \"1\"... ";
    cur_result = (binary_trie.getCode('a') == vector<bool>({true}));
    if (cur_result)
        cerr << "OK\n";
    else
        cerr << "FAIL\n";
    result |= cur_result;
    cerr << "checking code for 'b': should be \"01\"... ";
    cur_result = (binary_trie.getCode('b') == vector<bool>({false, true}));
    if (cur_result)
        cerr << "OK\n";
    else
        cerr << "FAIL\n";
    result |= cur_result;
    cerr << "checking code for '\\n': should be \"00\"... ";
    cur_result = binary_trie.getCode('\n') == vector<bool>({false, false});
    if (cur_result)
        cerr << "OK\n";
    else
        cerr << "FAIL\n";
    result |= cur_result;
    vector<bool> test_vector = vector<bool>({true});
    cerr << "checking character for \"1\": should be 'a'... ";
    cur_result = (binary_trie.nextCharacter(test_vector) == 'a');
    if (cur_result)
        cerr << "OK\n";
    else
        cerr << "FAIL\n";
    result |= cur_result;
    test_vector = vector<bool>({true, false});
    cerr << "checking character for \"01\": should be 'b'... ";
    cur_result = (binary_trie.nextCharacter(test_vector) == 'b');
    if (cur_result)
        cerr << "OK\n";
    else
        cerr << "FAIL\n";
    result |= cur_result;
    test_vector = vector<bool>({false, false});
    cerr << "checking character for \"00\": should be '\\n'... ";
    cur_result = (binary_trie.nextCharacter(test_vector) == '\n');
    if (cur_result)
        cerr << "OK\n";
    else
        cerr << "FAIL\n";
    result |= cur_result;
    return result;
}

TestParser::TestParser() {
    test_name = "Arguments parsing test";
}

bool TestParser::operator()() {
    int argc = 6;
    char **argv = new char*[argc];
    argv[0] = new char[8];
    strcpy(argv[0], "huffman");
    argv[1] = new char[3];
    strcpy(argv[1], "-c");
    argv[2] = new char[3];
    strcpy(argv[2], "-f");
    argv[3] = new char[9];
    strcpy(argv[3], "test.src");
    argv[4] = new char[3];
    strcpy(argv[4], "-o");
    argv[5] = new char[9];
    strcpy(argv[5], "test.bin");
    Arguments parser;
    cerr << "Checking for arguments: ";
    for (int i = 0; i < argc; ++i)
        cerr << argv[i] << ' ';
    cerr << '\n';
    try {
        parser.parse(argc, argv);
    } catch (...) {
        cerr << "Parser throws an exception, test failed\n";
        return false;
    };
    bool result = true, cur_result;
    cerr << "checking parsed command: should be \"-c\"... ";
    cur_result = parser.compress == true;
    if (cur_result)
        cerr << "OK\n";
    else
        cerr << "FAIL\n";
    result |= cur_result;
    cerr << "checking parsed source file: should be \"test.src\"... ";
    cur_result = (parser.file_in == "test.src");
    if (cur_result)
        cerr << "OK\n";
    else
        cerr << "FAIL\n";
    result |= cur_result;
    cerr << "checking parsed output file: should be \"test.bin\"... ";
    cur_result = (parser.file_out == "test.bin");
    if (cur_result)
        cerr << "OK\n";
    else
        cerr << "FAIL\n";
    result |= cur_result;
    for (int i = 0; i < argc; ++i)
        delete [] argv[i];
    delete [] argv;

    argc = 6;
    argv = new char*[argc];
    argv[0] = new char[8];
    strcpy(argv[0], "huffman");
    argv[1] = new char[9];
    strcpy(argv[1], "--output");
    argv[2] = new char[9];
    strcpy(argv[2], "test.res");
    argv[3] = new char[3];
    strcpy(argv[3], "-u");
    argv[4] = new char[7];
    strcpy(argv[4], "--file");
    argv[5] = new char[9];
    strcpy(argv[5], "test.bin");
    cerr << "Checking for arguments: ";
    for (int i = 0; i < argc; ++i)
        cerr << argv[i] << ' ';
    cerr << '\n';
    try {
        parser.parse(argc, argv);
    } catch (...) {
        cerr << "Parser throws an exception, test failed\n";
        return false;
    };
    cerr << "checking parsed command: should be \"-u\"... ";
    cur_result = (parser.compress == false);
    if (cur_result)
        cerr << "OK\n";
    else
        cerr << "FAIL\n";
    result |= cur_result;
    cerr << "checking parsed source file: should be \"test.bin\"... ";
    cur_result = (parser.file_in == "test.bin");
    if (cur_result)
        cerr << "OK\n";
    else
        cerr << "FAIL\n";
    result |= cur_result;
    cerr << "checking parsed output file: should be \"test.res\"... ";
    cur_result = (parser.file_out == "test.res");
    if (cur_result)
        cerr << "OK\n";
    else
        cerr << "FAIL\n";
    result |= cur_result;
    for (int i = 0; i < argc; ++i)
        delete [] argv[i];
    delete [] argv;

    argc = 5;
    argv = new char*[argc];
    argv[0] = new char[8];
    strcpy(argv[0], "huffman");
    argv[1] = new char[9];
    strcpy(argv[1], "--output");
    argv[2] = new char[9];
    strcpy(argv[2], "test.res");
    argv[3] = new char[3];
    strcpy(argv[3], "-u");
    argv[4] = new char[7];
    strcpy(argv[4], "--file");
    cerr << "Checking for arguments: ";
    for (int i = 0; i < argc; ++i)
        cerr << argv[i] << ' ';
    cerr << '\n';
    bool exception_thrown = false;
    try {
        parser.parse(argc, argv);
    } catch (...) {
        exception_thrown = true;
    };
    cerr << "should've thrown an exception... ";
    cur_result = (exception_thrown == true);
    if (cur_result)
        cerr << "OK\n";
    else
        cerr << "FAIL\n";
    result |= cur_result;
    for (int i = 0; i < argc; ++i)
        delete [] argv[i];
    delete [] argv;
    return result;
}

HuffmanUnitTest::HuffmanUnitTest() {
    test_group_name = "Unit tests";
    tests = vector<shared_ptr<TestInterface> >({
                shared_ptr<TestInterface>(new TestBinaryTrie()),
                shared_ptr<TestInterface>(new TestParser()),
            });
}
