#include "StrcpyTest.hpp"

#include <cstdlib>
#include <cstring>
#include <algorithm>

namespace funcperf {
namespace string {

StrcpyTest::StrcpyTest(int bytesToCopy, int srcOffset, int dstOffset, char* (*func)(char* dst, const char* src))
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

StrcpyTest::~StrcpyTest()
{
	free(m_srcBuffer);
	free(m_dstBuffer);
}

void StrcpyTest::run()
{
	(*m_func)(m_dstBuffer + m_dstOffset, m_srcBuffer + m_srcOffset);
}

bool StrcpyTest::verify()
{
	return strcmp(m_srcBuffer + m_srcOffset, m_dstBuffer + m_dstOffset) == 0;
}

}
}
