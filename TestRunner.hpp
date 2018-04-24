#pragma once

#include "ITest.hpp"
#include <stdint.h>

namespace funcperf {

class TestRunner
{
public:
	TestRunner();
	int64_t runTest(ITest& test, void* func, int iterations, bool* verifyResult);
};

}
