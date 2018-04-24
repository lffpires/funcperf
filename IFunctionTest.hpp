#pragma once

#include "ITestParams.hpp"
#include "ITest.hpp"

#include <vector>
#include <memory>

namespace funcperf {

class IFunctionTest
{
public:
	virtual std::string getFunctionName() = 0;
	virtual std::vector<std::shared_ptr<ITestParams>> getTestsParams() = 0;
	virtual std::shared_ptr<ITest> getTest(const ITestParams& testParams) = 0;
};

}
