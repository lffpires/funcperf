#pragma once

#include "ITest.hpp"
#include <stdint.h>

namespace funcperf {

class TestRunner
{
public:
	TestRunner();
	int64_t runTest(ITest& test, int iterations, bool* verifyResult);
};

}
