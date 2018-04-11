#include "StrncpyTest.hpp"

#include <cstdlib>
#include <cstring>
#include <algorithm>

namespace funcperf {
namespace string {

StrncpyTest::StrncpyTest(int bytesToCopy, int srcOffset, int dstOffset, char* (*func)(char* dst, const char* src, size_t len))
: m_bytesToCopy(bytesToCopy)
, m_srcOffset(srcOffset)
, m_dstOffset(dstOffset)
, m_func(func)
{
	m_bufferSize = std::max(srcOffset, dstOffset) + bytesToCopy + sizeof(long);
	m_srcBuffer = (char*)malloc(m_bufferSize);
//	m_srcBuffer[bufferSize-1] = '\0';
	m_dstBuffer = (char*)malloc(m_bufferSize);
//	m_dstBuffer[bufferSize-1] = '\0';

	m_srcBuffer[m_srcOffset + m_bytesToCopy-1] = '\0';
	m_dstBuffer[m_dstOffset + m_bytesToCopy-1] = '\0';

	char* longPtr = m_srcBuffer + m_srcOffset;
	for (int i = 0; i < m_bytesToCopy-2; i++) {
		char rand = i % 0xfe;
		if (rand == '\0') rand = 'a';
		longPtr[i] = rand;
	}
}

StrncpyTest::~StrncpyTest()
{
	free(m_srcBuffer);
	free(m_dstBuffer);
}

void StrncpyTest::run()
{
	(*m_func)(m_dstBuffer + m_dstOffset, m_srcBuffer + m_srcOffset, m_bytesToCopy);
}

bool StrncpyTest::verify()
{
	return strcmp(m_srcBuffer + m_srcOffset, m_dstBuffer + m_dstOffset) == 0;
}

}
}
