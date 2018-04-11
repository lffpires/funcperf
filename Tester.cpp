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

bool runTest(funcperf::ITest* testReference, funcperf::ITest* testCandidate, int iterations);
void runMemcpyTests (char* testType, void *soHandle, bool fast, bool medium);
void runStrTests(char* testType, void *soHandle,  bool fast, bool medium);
void runSingleTest(char* testType, void *soHandle, int bytesToCopy, int srcOffset, int dstOffset, int iterations);
int getIterations (int bytesToCopy, bool fast, bool medium);

int main(int argc, char** argv)
{
	bool fast = false;
	bool medium = false;

	// intialize map
	typeMap["memcpy"]	= memcpy_t;
	typeMap["strncpy"]	= strncpy_t;
	typeMap["strcpy"]	= strcpy_t;

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

	std::cout << "Running tests using " << argv[1] << "." << std::endl;

	//std::cout << "Loading candidate shared library...  ";
	// load candidate shared library
	void *soHandle = dlopen("/usr/obj/usr/src/powerpc.powerpc64/lib/libc/libc.so.7", RTLD_NOW);
	if (soHandle == NULL) {
		std::cerr << "Error loading shared library\n" << std::endl;
	}
	//std::cout << "OK."  << std::endl;


	switch (typeMap[argv[1]]) {
		case memcpy_t:
			runMemcpyTests (argv[1], soHandle, fast, medium);
			break;
		case strncpy_t:
		case strcpy_t:
			runStrTests (argv[1], soHandle, fast, medium);
			break;
	}

	dlclose(soHandle);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////
///////////////////////////////common////////////////////////////////////////////

int getIterations (int bytesToCopy, bool fast, bool medium) {
	if (fast) {
		return 1;
	} else if (medium) {
		if (bytesToCopy < 256) {
			return 2000;
		} else if (bytesToCopy < 1024*1024) {
			return 2000;
		} else {
			return 100;
		}
	} else {
		if (bytesToCopy < 256) {
			return 200000;
		} else if (bytesToCopy < 1024*1024) {
			return 20000;
		} else {
			return 1000;
		}
	}
}

void runSingleTest(char* testType, void *soHandle, int bytesToCopy, int srcOffset, int dstOffset, int iterations){
	funcperf::TestRunner testRunner;
	funcperf::ITest* testReference;
	funcperf::ITest* testCandidate;

	switch (typeMap[testType]) {
		case memcpy_t:
			testReference = new funcperf::string::MemcpyTest(bytesToCopy, srcOffset, dstOffset, &memcpy);
			testCandidate = new funcperf::string::MemcpyTest(bytesToCopy, srcOffset, dstOffset, (void* (*)(void*, const void*, size_t))dlfunc(soHandle, testType));
			break;
		case strncpy_t:
			testReference = new funcperf::string::StrncpyTest(bytesToCopy, srcOffset, dstOffset, &strncpy);
			testCandidate = new funcperf::string::StrncpyTest(bytesToCopy, srcOffset, dstOffset, (char* (*)(char*, const char*, size_t))dlfunc(soHandle, testType));
			break;
		case strcpy_t:
			testReference = new funcperf::string::StrcpyTest(bytesToCopy, srcOffset, dstOffset, &strcpy);
			testCandidate = new funcperf::string::StrcpyTest(bytesToCopy, srcOffset, dstOffset, (char* (*)(char*, const char*))dlfunc(soHandle, testType));
			break;
	}

	int64_t nanoReference = testRunner.runTest(*testReference, iterations, NULL);
	bool testResult;
	int64_t nanoCandidate = testRunner.runTest(*testCandidate, iterations, &testResult);

	std::cout << bytesToCopy << "\t" << srcOffset << "\t" << dstOffset << "\t" << iterations << "\t";
	std::cout << nanoReference << "\t" << nanoCandidate << "\t" << (testResult ? "SUCCESS" : "FAILURE") << std::endl;

	delete testReference;
	delete testCandidate;
}

///////////////////////////////common////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////str/////////////////////////////////////////////

void runStrTests(char* testType, void *soHandle,  bool fast, bool medium) {
	std::cout << "bytesToCopy\tsrcOffset\tdstOffset\titerations\tnanoReference\tnanoCandidate\ttestResult" << std::endl;

	for (int bytesToCopy = 2; bytesToCopy <= 8*1024*1024; bytesToCopy *= 2) {
		int iterations = getIterations(bytesToCopy, fast, medium);

		for (int srcOffsetIdx = 0; srcOffsetIdx < 8; srcOffsetIdx++) {
			for (int dstOffsetIdx = 0; dstOffsetIdx < 8; dstOffsetIdx++) {
				runSingleTest(testType, soHandle, bytesToCopy, srcOffsetIdx, dstOffsetIdx, iterations);
			}
		}
	}
}

/////////////////////////////////str/////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
////////////////////////////////memcpy///////////////////////////////////////////

void runMemcpyTests (char* testType, void *soHandle,  bool fast, bool medium) {
	std::cout << "bytesToCopy\tsrcOffset\tdstOffset\titerations\tnanoReference\tnanoCandidate\ttestResult" << std::endl;

	for (int bytesToCopy = 1; bytesToCopy <= 8*1024*1024; bytesToCopy *= 2) {
		int iterations = getIterations(bytesToCopy, fast, medium);
		int srcOffset = 0;
		for (int srcOffsetIdx = 0; srcOffsetIdx < 8; srcOffsetIdx++) {
			int dstOffset = 0;
			for (int dstOffsetIdx = 0; dstOffsetIdx < 8; dstOffsetIdx++) {
				// overlapping test
				runSingleTest(testType, soHandle, bytesToCopy, srcOffset, dstOffset, iterations);

				// non-overlapping test
				runSingleTest(testType, soHandle, bytesToCopy, srcOffset, 8*1024*1024 + dstOffset, iterations);
				runSingleTest(testType, soHandle, bytesToCopy, 8*1024*1024 + srcOffset, dstOffset, iterations);

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

}

////////////////////////////////memcpy///////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////



