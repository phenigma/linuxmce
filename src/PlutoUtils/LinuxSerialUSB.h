#ifndef LinuxSerialUSB_h
#define LinuxSerialUSB_h

#include "FileUtils.h"
#include "DCE/Logger.h"

using namespace DCE;

#ifdef WIN32
string TranslateSerialUSB(string sInput) { return sInput; }
#else
string TranslateSerialUSB(string sInput);
#endif

#endif


