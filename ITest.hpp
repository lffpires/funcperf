#pragma once

namespace funcperf {

class ITest
{
public:
	virtual void run() = 0;
	virtual bool verify() = 0;
};

}
