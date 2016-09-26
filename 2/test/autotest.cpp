#include "autotest.h"
#include <iostream>
#include <iomanip>

void Autotest::testAll() {
    cerr << ">> Started " << test_group_name << '\n';
    clock_t start_time = clock();
    successful_tests = 0;
    for (auto test_ptr : tests) {
        cerr << "\n>> " << test_ptr->test_name << '\n';
        if (!test_ptr->operator()()) 
            cerr << ">> Test failed\n";
        else {
            cerr << ">> Test succeded\n";
            ++successful_tests;
        }
    }
    clock_t finish_time = clock();
    total_time = finish_time - start_time;
    cerr << ">> Finished " << test_group_name << '\n';
    cerr << ">> " << successful_tests << " out of " << tests.size() << 
        " tests were successful\n";
    cerr << ">> Total time: " << fixed << setprecision(2) << 
        static_cast<double>(total_time) / CLOCKS_PER_SEC << " sec\n\n";
}
