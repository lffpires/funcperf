#pragma once

#include <string>

namespace funcperf {

enum class TestLength {
	shortTest,
	normalTest,
	longTest,
};

class ITestParams
{
public:
	virtual std::string getId() = 0;
	virtual std::string getTSVHeaders() = 0;
	virtual std::string getTSVValues() = 0;
	virtual int getIterations(TestLength length) = 0;
};

}
