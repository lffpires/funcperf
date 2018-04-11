#pragma once

#include "ITest.hpp"

#include <cstddef>

namespace funcperf {
namespace string {

class StrncpyTest : public funcperf::ITest
{
public:
	StrncpyTest(int bytesToCopy, int srcOffset, int dstOffset, char* (*func)(char* dst, const char* src, size_t len));
	virtual ~StrncpyTest();

	void run();
	bool verify();

private:
	int m_bufferSize;
	int m_bytesToCopy;
	int m_srcOffset;
	int m_dstOffset;
	char* (*m_func)(char* dst, const char* src, size_t len);
	char* m_srcBuffer;
	char* m_dstBuffer;
};

}
}
