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
	virtual std::string getCSVHeaders(const std::string& sep) = 0;
	virtual std::string getCSVValues(const std::string& sep) = 0;
	virtual int getIterations(TestLength length) = 0;
};

}
