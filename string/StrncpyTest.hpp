#pragma once

#include "ITest.hpp"

#include "MemcpyTestParams.hpp"

#include <cstddef>

namespace funcperf {
namespace string {

class StrncpyTest : public funcperf::ITest
{
public:
	StrncpyTest(const MemcpyTestParams& testParams);
	virtual ~StrncpyTest();

	std::string getId();
	void run(void* func);
	bool verify();

private:
	MemcpyTestParams m_testParams;

	char* m_srcBuffer;
	char* m_dstBuffer;
	char* m_verifyBuffer;
};

}
}
