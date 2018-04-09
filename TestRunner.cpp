#include "TestRunner.hpp"

#include <algorithm>
#include <ctime>

namespace funcperf {

TestRunner::TestRunner()
{
}

int64_t TestRunner::runTest(ITest& test, int iterations, bool* verifyResult)
{
	int res;
	struct timespec tp0, tp1;
	int64_t nanos[iterations];
	int64_t totalSum = 0;
	int totalValidMeasures = 0;

	bool tmpVerifyResult = true;

	for (int iteration = 0; iteration < iterations; iteration++) {
		res = clock_gettime(CLOCK_MONOTONIC_PRECISE, &tp0);

		test.run();

		res |= clock_gettime(CLOCK_MONOTONIC_PRECISE, &tp1);

		if (res == 0) {
			uint64_t nano0 = (1000000000LL * tp0.tv_sec) + tp0.tv_nsec;
			uint64_t nano1 = (1000000000LL * tp1.tv_sec) + tp1.tv_nsec;

			nanos[totalValidMeasures++] = nano1 - nano0;
			//totalValidMeasures++;
			totalSum += (nano1 - nano0);
		}

		if (verifyResult != NULL && iteration == 0) {
			tmpVerifyResult = test.verify();
		}
	}

	if (verifyResult != NULL) {
		*verifyResult = tmpVerifyResult;
	}

	//return totalSum / totalValidMeasures;

	// return an approximation of the median
	std::sort(nanos, nanos + totalValidMeasures);
	return nanos[totalValidMeasures / 2];
}

}
