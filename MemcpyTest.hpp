#pragma once

#include "ITest.hpp"

#include <cstddef>

namespace funcperf {
namespace string {

class MemcpyTest : public funcperf::ITest
{
public:
	MemcpyTest(int bufferSize, void* (*func)(void* dst, const void* src, size_t len));
	virtual ~MemcpyTest();

	void run();

private:
	int m_bufferSize;
	void* (*m_func)(void* dst, const void* src, size_t len);
	void* m_srcBuffer;
	void* m_dstBuffer;
};

}
}
