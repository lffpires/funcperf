#include "TestRunner.hpp"
#include "MemcpyTest.hpp"

#include <iostream>
#include <cstring>

#include <stdint.h>
#include <dlfcn.h>

int main(int argc, char** argv)
{
	funcperf::TestRunner testRunner;

	funcperf::string::MemcpyTest memcpyTestReference(10*1024*1024, &memcpy);
	int64_t nanoDiffReference = testRunner.runTest(memcpyTestReference, 1000);

	// load candidate shared library
	void *soHandle = dlopen("/usr/obj/usr/src/powerpc.powerpc64/lib/libc/libc.so.7", RTLD_NOW);
	if (soHandle == NULL) {
		std::cerr << "Error loading shared library\n" << std::endl;
		return 1;
	}

	void* (*candidateFunc)(void* dst, const void* src, size_t len) = (void* (*)(void*, const void*, size_t))dlfunc(soHandle, "memcpy");

	funcperf::string::MemcpyTest memcpyTestCandidate(10*1024*1024, candidateFunc);
	int64_t nanoDiffCandidate = testRunner.runTest(memcpyTestCandidate, 1000);

	std::cout << "Reference time: " << static_cast<double>(nanoDiffReference) / 1000000  << "ms." << std::endl;
	std::cout << "Candidate time: " << static_cast<double>(nanoDiffCandidate) / 1000000  << "ms." << "(" << (100.0 * nanoDiffCandidate) / nanoDiffReference << "%)" << std::endl;

	dlclose(soHandle);
}
