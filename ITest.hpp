#pragma once

namespace funcperf {

class ITest
{
public:
	virtual void run(void* func) = 0;
	virtual bool verify() = 0;
};

}
