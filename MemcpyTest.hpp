#pragma once

#include "ITest.hpp"

#include <cstddef>

namespace funcperf {
namespace string {

class MemcpyTest : public funcperf::ITest
{
public:
	MemcpyTest(int bytesToCopy, int srcOffset, int dstOffset, void* (*func)(void* dst, const void* src, size_t len));
	virtual ~MemcpyTest();

	void run();
	bool verify();

private:
	int m_bytesToCopy;
	int m_srcOffset;
	int m_dstOffset;
	void* (*m_func)(void* dst, const void* src, size_t len);
	char* m_buffer;
	char* m_verifyBuffer;
	int m_bufferSize;
};

}
}
