#include "MemcpyTest.hpp"

#include <cstdlib>

namespace funcperf {
namespace string {

MemcpyTest::MemcpyTest(int bufferSize, void* (*func)(void* dst, const void* src, size_t len))
: m_bufferSize(bufferSize)
, m_func(func)
{
	m_srcBuffer = malloc(bufferSize);
	m_dstBuffer = malloc(bufferSize);
}

MemcpyTest::~MemcpyTest()
{
	free(m_srcBuffer);
	free(m_dstBuffer);
}

void MemcpyTest::run()
{
	(*m_func)(m_dstBuffer, m_srcBuffer, m_bufferSize);
}

}
}
