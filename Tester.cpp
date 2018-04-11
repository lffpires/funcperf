#include "TestRunner.hpp"
#include "MemcpyTest.hpp"
#include "StrncpyTest.hpp"
#include "StrcpyTest.hpp"

#include <iostream>
#include <cstring>

#include <stdint.h>
#include <dlfcn.h>
#include <map>

enum type {
	memcpy_t,
	strncpy_t,
	strcpy_t
};

std::map<std::string, type> typeMap;

void runTest(char* testType, int bytesToCopy, int srcOffset, int dstOffset, int iterations);

int main(int argc, char** argv)
{
	bool fast = false;
	bool medium = false;

	// TODO: implement proper arg handling
        if (argc <= 1) {
		std::cerr << "Error: At least the test type must be specified!" << std::endl;
                std::cerr << "Test type available options: {memcpy; strncpy; strcpy}" << std::endl;
		return 1;
	}

	if (argc > 2 && strcmp(argv[2], "--fast") == 0) {
		fast = true;
	} else if (argc > 2 && strcmp(argv[2], "--medium") == 0) {
		medium = true;
	}

	typeMap["memcpy"]	= memcpy_t;
	typeMap["strncpy"]	= strncpy_t;
	typeMap["strcpy"]       = strncpy_t;

	std::cout << "Running tests using " << argv[1] << "." << std::endl;

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
				runTest(argv[1], bytesToCopy, srcOffset, dstOffset, iterations);

				// non-overlapping test
				runTest(argv[1], bytesToCopy, srcOffset, 8*1024*1024 + dstOffset, iterations);
				runTest(argv[1], bytesToCopy, 8*1024*1024 + srcOffset, dstOffset, iterations);

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

void runTest(char* testType, int bytesToCopy, int srcOffset, int dstOffset, int iterations)
{
	//std::cout << "Testing bytesToCopy=[" << bytesToCopy << "] srcOffset=[" << srcOffset << "] dstOffset=[" << dstOffset <<
	//		"] iterations=[" << iterations << "]..." << std::endl;

	funcperf::TestRunner testRunner;
	funcperf::ITest* testReference;
	funcperf::ITest* testCandidate;

	//std::cout << "Loading candidate shared library...  ";
	// load candidate shared library
	void *soHandle = dlopen("/usr/obj/usr/src/powerpc.powerpc64/lib/libc/libc.so.7", RTLD_NOW);
	if (soHandle == NULL) {
		std::cerr << "Error loading shared library\n" << std::endl;
	}
	//std::cout << "OK."  << std::endl;

	//std::cout << "Creating test objects... ";
	switch (typeMap[testType]) {
		case memcpy_t:
			testReference = new funcperf::string::MemcpyTest(bytesToCopy, srcOffset, dstOffset, &memcpy);
			testCandidate = new funcperf::string::MemcpyTest(bytesToCopy, srcOffset, dstOffset, (void* (*)(void*, const void*, size_t))dlfunc(soHandle, testType));
			break;
		case strncpy_t:
			testReference = new funcperf::string::StrncpyTest(10*1024*1024, &strncpy);
			testCandidate = new funcperf::string::StrncpyTest(10*1024*1024, (char* (*)(char*, const char*, size_t))dlfunc(soHandle, testType));
			break;
		case strcpy_t:
			testReference = new funcperf::string::StrcpyTest(10*1024*1024, &strcpy);
			testCandidate = new funcperf::string::StrcpyTest(10*1024*1024, (char* (*)(char*, const char*))dlfunc(soHandle, testType));
			break;
	}

	//std::cout << "OK." << std::endl;

	//std::cout << "Running reference... ";
	int64_t nanoReference = testRunner.runTest(*testReference, iterations, NULL);
	//std::cout << "OK." << std::endl;

	//int64_t nanoCandidate = testRunner.runTest(memcpyTestCandidate, iterations);
	bool testResult;

	//std::cout << "Running candidate... ";
	int64_t nanoCandidate = testRunner.runTest(*testCandidate, iterations, &testResult);
	//std::cout << "OK." << std::endl;

	//bool testResult = memcpyTestCandidate.verify();

	//std::cout << "Test result: " << (testResult ? "SUCCESS" : "FAILURE") << std::endl;

	//std::cout << "Reference time: " << static_cast<double>(nanoReference) / 1000  << "us." << std::endl;
	//std::cout << "Candidate time: " << static_cast<double>(nanoCandidate) / 1000  << "us." << "(" << (100.0 * nanoCandidate) / nanoReference << "%)" << std::endl;

	std::cout << bytesToCopy << "\t" << srcOffset << "\t" << dstOffset << "\t" << iterations << "\t";
	std::cout << nanoReference << "\t" << nanoCandidate << "\t" << (testResult ? "SUCCESS" : "FAILURE") << std::endl;

	dlclose(soHandle);
}
