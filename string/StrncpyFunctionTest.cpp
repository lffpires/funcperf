#include "StrncpyFunctionTest.hpp"

#include "StrncpyTest.hpp"
#include "MemcpyTestParams.hpp"

namespace funcperf {
namespace string {

std::string StrncpyFunctionTest::getFunctionName()
{
	return "strncpy";
}

std::vector<std::shared_ptr<funcperf::ITestParams>> StrncpyFunctionTest::getTestsParams()
{
	std::vector<std::shared_ptr<funcperf::ITestParams>> result;

	for (int bytesToCopy = 2; bytesToCopy <= 8*1024*1024; bytesToCopy *= 2) {
		for (int srcOffset = 0; srcOffset < 8; srcOffset++) {
			for (int dstOffset = 0; dstOffset < 8; dstOffset++) {
				result.push_back(std::make_shared<MemcpyTestParams>(bytesToCopy, srcOffset, dstOffset));
			}
		}
	}

	return result;
}

std::shared_ptr<funcperf::ITest> StrncpyFunctionTest::getTest(const funcperf::ITestParams& testParams)
{
	return std::make_shared<StrncpyTest>(dynamic_cast<const MemcpyTestParams&>(testParams));
}

}
}
