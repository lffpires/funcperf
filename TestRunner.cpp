#include "TestRunner.hpp"

#include <ctime>

namespace funcperf {

TestRunner::TestRunner()
{
}

int64_t TestRunner::runTest(ITest& test, int iterations)
{
	int64_t result = -1;
	int res;
	struct timespec tp0, tp1;

	res = clock_gettime(CLOCK_MONOTONIC_PRECISE, &tp0);

	for (int iteration = 0; iteration < iterations; iteration++) {
		test.run();
	}

	res |= clock_gettime(CLOCK_MONOTONIC_PRECISE, &tp1);

	if (res == 0)
	{
		uint64_t nano0 = (1000000000LL * tp0.tv_sec) + tp0.tv_nsec;
		uint64_t nano1 = (1000000000LL * tp1.tv_sec) + tp1.tv_nsec;

		result = nano1 - nano0;
	}

	return result;
}

}
