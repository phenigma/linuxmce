#ifndef LinuxSerialUSB_h
#define LinuxSerialUSB_h

#include "FileUtils.h"

string TranslateSerialUSB(string sInput)
{
	if( sInput.size()<6 || sInput.substr(0,3)!="usb" )
		return sInput;

	list<string> listFiles;
	FileUtils::FindFiles(listFiles,"/sys/bus/usb-serial/devices","*");
	g_pPlutoLogger->Write(LV_STATUS,"TranslateSerialUSB found %d linux usbserial devices",listFiles.size());
	for(list<string>::iterator it=listFiles.begin();it!=listFiles.end();++it)
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


