#pragma once

#include "ITestParams.hpp"

#include <string>

namespace funcperf {
namespace string {

class MemcpyTestParams : public funcperf::ITestParams
{
public:
	MemcpyTestParams(int bytesToCopy, int srcOffset, int dstOffset);
	int getBytesToCopy();
	int getSrcOffset();
	int getDstOffset();

	std::string getId();
	std::string getTSVHeaders();
	std::string getTSVValues();
	int getIterations(TestLength length);

private:
	int m_bytesToCopy;
	int m_srcOffset;
	int m_dstOffset;
};

}
}
