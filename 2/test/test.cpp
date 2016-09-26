#include "huffman_test.h"

using namespace std;

int main() {
    HuffmanIntegrationTest huffman_integration_test;
    HuffmanUnitTest huffman_unit_test;
    huffman_integration_test.testAll();
    huffman_unit_test.testAll();

    return 0;
}
