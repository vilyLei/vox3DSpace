#ifndef VOXOL_TEST_CONCURRENT_H
#define VOXOL_TEST_CONCURRENT_H

#include <iostream>
#include "TestLockFree.h"

namespace Voxol::Test
{
class TestConcurrent
{
private:
public:
    TestConcurrent()  = default;
    ~TestConcurrent() = default;

public:
    void init();
};

} // namespace Voxol::Test
#endif