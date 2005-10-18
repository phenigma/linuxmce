#ifndef LinuxSerialUSB_h
#define LinuxSerialUSB_h

#include "FileUtils.h"

string TranslateSerialUSB(string sInput)
{
	if( sInput.size()<6 || sInput.substr(0,3)!="usb" )
		return sInput;

	char tmpFile[40] = "/tmp/devusbXXXXXX";
	mktemp(tmpFile);
	system(("ls -l /sys/bus/usb-serial/devices/ > " + string(tmpFile)).c_str());
	
	vector<string> vectStr;
	FileUtils::ReadFileIntoVector(tmpFile,vectStr);
	for(vector<string>::iterator it=vectStr.begin();it!=vectStr.end();++it)
	{
		if( (*it).find(sInput)!=string::npos )
		{
			g_pPlutoLogger->Write(LV_STATUS,"TranslateSerialUSB found %s, returning %s",
				(*it).c_str(),("/dev/" + FileUtils::FilenameWithoutPath(*it)).c_str());
			return "/dev/" + FileUtils::FilenameWithoutPath(*it);
		}
	}
	return sInput;
}

#endif


