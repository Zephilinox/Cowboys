#pragma once

#include "stdafx.h"

/** 
* https://stackoverflow.com/questions/18277304/using-stdcout-in-multiple-threadsThread 
* safe cout class
* Example of use:
*    PrintThread{} << "Hello world!" << std::endl;
*/

class PrintThread :
	public std::ostringstream
{
public:
	PrintThread() = default;
	~PrintThread();

private:
	static std::mutex cout_mtx;
};

