#ifndef HUFFMAN_TEST_H_
#define HUFFMAN_TEST_H_

#include "autotest.h"
#include <string>

using namespace std;

const int kBigFileSize = 5000000;
const int kReadSize = 4096;

struct FileCompareTestInterface : TestInterface {
    virtual ~FileCompareTestInterface() = default;
    virtual bool operator ()();
    virtual void generateTest() = 0;
    bool compareFiles(const string &file_a, const string &file_b);
    string test_file_prefix;
};

struct TestBig : FileCompareTestInterface {
    using TestInterface::test_name;
    TestBig();
    virtual void generateTest();
};

struct TestOneCharacter : FileCompareTestInterface {
    using TestInterface::test_name;
    TestOneCharacter();
    virtual void generateTest();
};

struct TestEmpty : FileCompareTestInterface {
    using TestInterface::test_name;
    TestEmpty();
    virtual void generateTest();
};

class HuffmanIntegrationTest : public Autotest {
public:
    HuffmanIntegrationTest();
};

struct TestBinaryTrie : TestInterface {
    using TestInterface::test_name;
    TestBinaryTrie();
    virtual ~TestBinaryTrie() = default;
    virtual bool operator ()();
};

struct TestParser : TestInterface {
    using TestInterface::test_name;
    TestParser();
    virtual ~TestParser() = default;
    virtual bool operator ()();
};

class HuffmanUnitTest : public Autotest {
public:
    HuffmanUnitTest();
};

#endif
