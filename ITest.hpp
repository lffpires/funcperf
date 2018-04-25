#pragma once

#include <string>

namespace funcperf {

class ITest
{
public:
	virtual ~ITest() {};
	virtual std::string getId() = 0;
	virtual void run(void* func) = 0;
	virtual bool verify() = 0;
};

}
