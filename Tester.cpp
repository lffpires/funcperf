#include "TestRunner.hpp"

#include "IFunctionTest.hpp"
#include "string/MemcpyFunctionTest.hpp"

#include <string>
#include <iostream>
#include <cstring>
#include <cstdint>
#include <map>
#include <memory>

#include <dlfcn.h>

int main(int argc, char** argv)
{
	const char* pLibFilename = nullptr;
	std::shared_ptr<funcperf::IFunctionTest> pFunctionTest = nullptr;

	funcperf::TestLength testLength = funcperf::TestLength::normalTest;

	std::map<std::string, std::shared_ptr<funcperf::IFunctionTest>> funcTestMap = {
		// possible tests
		{"memcpy", std::make_shared<funcperf::string::MemcpyFunctionTest>()},
	};

	// for now, using ugly homemade arg parsing, to avoid adding dependencies. I'm not proud of it, though
	int curArg = 1;
	bool cmdlineParseError = false;
	while (curArg < argc) {
		if (strcmp(argv[curArg], "--lib") == 0) {
			curArg++;

			if (curArg >= argc) {
				cmdlineParseError = true;
				break;
			}

			pLibFilename = argv[curArg];

		} else if (strcmp(argv[curArg], "--test") == 0) {
			curArg++;

			auto findIt = funcTestMap.find(argv[curArg]);
			if (findIt == funcTestMap.end()) {
				cmdlineParseError = true;
				break;
			}

			pFunctionTest = findIt->second;

		} else if (strcmp(argv[curArg], "--length") == 0) {
			curArg++;
			if (curArg >= argc) {
				cmdlineParseError = true;
				break;
			}

			if (strcmp(argv[curArg], "short") == 0) {
				testLength = funcperf::TestLength::shortTest;

			} else if (strcmp(argv[curArg], "normal") == 0) {
				testLength = funcperf::TestLength::normalTest;

			} else if (strcmp(argv[curArg], "long") == 0) {
				testLength = funcperf::TestLength::longTest;

			} else {
				cmdlineParseError = true;
				break;
			}

		} else  {
			cmdlineParseError = true;
			break;
		}

		curArg++;
	}

	if (pLibFilename == nullptr || pFunctionTest == nullptr || cmdlineParseError) {
		std::cerr << "Usage: " << argv[0] << " --lib <libFilename> --test <testId> [--length short|normal|long]" << std::endl;
		std::cerr << std::endl;
		std::cerr << "Possible values for 'testId':" << std::endl;
		for (auto& kv : funcTestMap) {
			std::cerr << "\t" << kv.first << std::endl;
		}

		return 1;
	}

	// load shared library
	void *soHandle = dlopen(pLibFilename, RTLD_NOW);
	if (soHandle == nullptr) {
		std::cerr << "Error loading shared library " << pLibFilename << std::endl;

		return 2;
	}

	funcperf::TestRunner testRunner;

	// get pointer to tested function
	void* func = dlsym(soHandle, pFunctionTest->getFunctionName().c_str());
	if (func == nullptr) {
		std::cerr << "Symbol " << pFunctionTest->getFunctionName() << " not found in " << pLibFilename << std::endl;

		dlclose(soHandle);
		return 3;
	}

	auto testsParams = pFunctionTest->getTestsParams();

	bool printHeader = true;
	for (auto& pTestParams : testsParams) {
		bool testResult;

		if (printHeader) {
			std::cout << "id\t" << pTestParams->getTSVHeaders() << "\titerations\tavgNanos\ttestResult" << std::endl;
			printHeader = false;
		}

		auto pTest = pFunctionTest->getTest(*pTestParams);
		int iterations = pTestParams->getIterations(testLength);
		int64_t nanos = testRunner.runTest(*pTest, func, iterations, &testResult);

		std::cout << pTestParams->getId() << "\t" << pTestParams->getTSVValues() << "\t" << iterations << "\t";
		std::cout << nanos << "\t" << (testResult ? "SUCCESS" : "FAILURE") << std::endl;
	}

	dlclose(soHandle);

	return 0;
}
