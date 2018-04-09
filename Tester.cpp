#include "TestRunner.hpp"
#include "MemcpyTest.hpp"

#include <iostream>
#include <cstring>

#include <stdint.h>
#include <dlfcn.h>

void runTest(int bytesToCopy, int srcOffset, int dstOffset, int iterations);

int main(int argc, char** argv)
{
	bool fast = false;
	bool medium = false;
	// TODO: implement proper arg handling
	if (argc > 1 && strcmp(argv[1], "--fast") == 0) {
		fast = true;
	} else if (argc > 1 && strcmp(argv[1], "--medium") == 0) {
		medium = true;
	}

	std::cout << "bytesToCopy\tsrcOffset\tdstOffset\titerations\tnanoReference\tnanoCandidate\ttestResult" << std::endl;

	for (int bytesToCopy = 1; bytesToCopy <= 8*1024*1024; bytesToCopy *= 2) {
		int iterations;
		if (fast) {
			iterations = 1;
		} else if (medium) {
			if (bytesToCopy < 256) {
				iterations = 2000;
			} else if (bytesToCopy < 1024*1024) {
				iterations = 2000;
			} else {
				iterations = 100;
			}
		} else {
			if (bytesToCopy < 256) {
				iterations = 200000;
			} else if (bytesToCopy < 1024*1024) {
				iterations = 20000;
			} else {
				iterations = 1000;
			}
		}

		int srcOffset = 0;
		for (int srcOffsetIdx = 0; srcOffsetIdx < 8; srcOffsetIdx++) {
			int dstOffset = 0;
			for (int dstOffsetIdx = 0; dstOffsetIdx < 8; dstOffsetIdx++) {
				// overlapping test
				runTest(bytesToCopy, srcOffset, dstOffset, iterations);

				// non-overlapping test
				runTest(bytesToCopy, srcOffset, 8*1024*1024 + dstOffset, iterations);
				runTest(bytesToCopy, 8*1024*1024 + srcOffset, dstOffset, iterations);

				if (dstOffset > 0) {
					dstOffset *= 2;
				} else {
					dstOffset = 1;
				}
			}

			if (srcOffset > 0) {
				srcOffset *= 2;
			} else {
				srcOffset = 1;
			}
		}
	}

	return 0;
}

void runTest(int bytesToCopy, int srcOffset, int dstOffset, int iterations)
{
	//std::cout << "Testing bytesToCopy=[" << bytesToCopy << "] srcOffset=[" << srcOffset << "] dstOffset=[" << dstOffset <<
	//		"] iterations=[" << iterations << "]..." << std::endl;

	funcperf::TestRunner testRunner;

	funcperf::string::MemcpyTest memcpyTestReference(bytesToCopy, srcOffset, dstOffset, &memcpy);
	int64_t nanoReference = testRunner.runTest(memcpyTestReference, iterations, NULL);

	// load candidate shared library
	void *soHandle = dlopen("/usr/obj/usr/src/powerpc.powerpc64/lib/libc/libc.so.7", RTLD_NOW);
	if (soHandle == NULL) {
		std::cerr << "Error loading shared library\n" << std::endl;
	}

	void* (*candidateFunc)(void* dst, const void* src, size_t len) = (void* (*)(void*, const void*, size_t))dlfunc(soHandle, "memcpy");

	funcperf::string::MemcpyTest memcpyTestCandidate(bytesToCopy, srcOffset, dstOffset, candidateFunc);
	//int64_t nanoCandidate = testRunner.runTest(memcpyTestCandidate, iterations);
	bool testResult;
	int64_t nanoCandidate = testRunner.runTest(memcpyTestCandidate, iterations, &testResult);

	//bool testResult = memcpyTestCandidate.verify();

	//std::cout << "Test result: " << (testResult ? "SUCCESS" : "FAILURE") << std::endl;

	//std::cout << "Reference time: " << static_cast<double>(nanoReference) / 1000  << "us." << std::endl;
	//std::cout << "Candidate time: " << static_cast<double>(nanoCandidate) / 1000  << "us." << "(" << (100.0 * nanoCandidate) / nanoReference << "%)" << std::endl;

	std::cout << bytesToCopy << "\t" << srcOffset << "\t" << dstOffset << "\t" << iterations << "\t";
	std::cout << nanoReference << "\t" << nanoCandidate << "\t" << (testResult ? "SUCCESS" : "FAILURE") << std::endl;

	dlclose(soHandle);
}
