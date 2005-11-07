/* 
	LinuxSerialUSB
	
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation
	
	www.plutohome.com		
	
	Phone: +1 (877) 758-8648
	
	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty 
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
	
	See the GNU General Public License for more details.
*/

#include "PlutoUtils/LinuxSerialUSB.h"	 

string TranslateSerialUSB(string sInput)
{
	if( sInput.size()<6 || sInput.substr(0,3)!="usb" )
	{
		g_pPlutoLogger->Write(LV_STATUS,"TranslateSerialUSB %s isn't serial usb",sInput.c_str());
		return sInput;
	}

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
	g_pPlutoLogger->Write(LV_STATUS,"TranslateSerialUSB %s couldn't find a match",sInput.c_str());
	return sInput;
}
