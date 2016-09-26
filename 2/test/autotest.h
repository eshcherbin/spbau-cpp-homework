#ifndef AUTOTEST_H_
#define AUTOTEST_H_

#include <ctime>
#include <string>
#include <vector>
#include <algorithm>
#include <memory>

using namespace std;

struct TestInterface {
    virtual ~TestInterface() = default;
    virtual bool operator ()() = 0;
    string test_name;
};

class Autotest {
public:
    void testAll();
protected:
    string test_group_name;
    vector<shared_ptr<TestInterface> > tests;
    int successful_tests;
    clock_t total_time;
};

#endif
