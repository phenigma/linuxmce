#include <iostream>

#include "Log.h"

void Log(string sMessage) {
	std::cout << "[" << getpid() << "]\t" << sMessage << std::endl;
}

