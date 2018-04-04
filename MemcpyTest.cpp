#include "MemcpyTest.hpp"

#include <cstdlib>
#include <cstring>
#include <algorithm>

namespace funcperf {
namespace string {

MemcpyTest::MemcpyTest(int bytesToCopy, int srcOffset, int dstOffset, void* (*func)(void* dst, const void* src, size_t len))
: m_bytesToCopy(bytesToCopy)
, m_srcOffset(srcOffset)
, m_dstOffset(dstOffset)
, m_func(func)
{
	m_bufferSize = std::max(srcOffset, dstOffset) + bytesToCopy + sizeof(long);
	m_buffer = (char *)malloc(m_bufferSize);
	m_verifyBuffer = (char *)malloc(m_bufferSize);

	long* longPtr = (long*)m_buffer;
	for (int i = 0; i < m_bufferSize / sizeof(long); i++) {
		longPtr[i] = random();
	}

	// prepare verification buffer
	memcpy(m_verifyBuffer, m_buffer, m_bufferSize);
	memcpy(m_verifyBuffer + dstOffset, m_verifyBuffer + srcOffset, m_bytesToCopy);
}

MemcpyTest::~MemcpyTest()
{
	free(m_buffer);
	free(m_verifyBuffer);
}

void MemcpyTest::run()
{
	(*m_func)(m_buffer + m_dstOffset, m_buffer + m_srcOffset, m_bytesToCopy);
}

bool MemcpyTest::verify()

{
	return memcmp(m_buffer, m_verifyBuffer, m_bufferSize) == 0;
}

}
}
