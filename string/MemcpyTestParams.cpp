#include "MemcpyTestParams.hpp"

#include <sstream>

namespace funcperf {
namespace string {

MemcpyTestParams::MemcpyTestParams(int bytesToCopy, int srcOffset, int dstOffset)
: m_bytesToCopy(bytesToCopy)
, m_srcOffset(srcOffset)
, m_dstOffset(dstOffset)
{
}

int MemcpyTestParams::getBytesToCopy()
{
	return m_bytesToCopy;
}

int MemcpyTestParams::getSrcOffset()
{
	return m_srcOffset;
}

int MemcpyTestParams::getDstOffset()
{
	return m_dstOffset;
}

std::string MemcpyTestParams::getId()
{
	std::stringstream ss;

	ss << "MEMCPY_" << m_bytesToCopy << "_" << m_srcOffset << "_" << m_dstOffset;

	return ss.str();
}

std::string MemcpyTestParams::getTSVHeaders()
{
	return "bytesToCopy\tsrcOffset\tdstOffset";
}

std::string MemcpyTestParams::getTSVValues()
{
	std::stringstream ss;

	ss << m_bytesToCopy << "\t" << m_srcOffset << "\t" << m_dstOffset;

	return ss.str();
}

int MemcpyTestParams::getIterations(TestLength length)
{
	int iterations;

	switch (length) {
	case TestLength::shortTest:
		iterations = 1;
		break;

	case TestLength::normalTest:
		if (m_bytesToCopy < 256) {
			iterations = 2000;
		} else if (m_bytesToCopy < 1024*1024) {
			iterations = 2000;
		} else {
			iterations = 100;
		}
		break;

	case TestLength::longTest:
		if (m_bytesToCopy < 256) {
			iterations = 200000;
		} else if (m_bytesToCopy < 1024*1024) {
			iterations = 20000;
		} else {
			iterations = 1000;
		}
		break;
	}

	return iterations;
}

}
}
