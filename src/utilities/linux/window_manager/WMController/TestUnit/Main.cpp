#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>

#include "../WMController.h"

int main(int argc, char *argv[])
{
 	int opt;	
	
	//-r win : window class name
	string sWindowName;
	//-l : list windows 
	bool bListWindows = false;	
	//-p x,y,h,w : position
	string sPosition;
	//-v 1 or 0 : visibility
	string sVisible;
	//-f 1 or 0 : fullscreen?
	string sFullScreen;	
	//-z above|below|normal : layer
	string sLayer;
	
    while ((opt = getopt(argc, argv, "lr:p:v:z:f:")) != -1) 
	{
        switch (opt) 
		{
            case 'r':
				sWindowName = optarg;
                break;
            case 'p':
				sPosition = optarg;
                break;
			case 'l':
				bListWindows = true;
				break;
			case 'v':
				sVisible = optarg;
				break;
			case 'z':
				sLayer = optarg;
				break;
			case 'f':
				sFullScreen = optarg;
				break;
				
			default:
				printf("Unknown option\n\n");
				printf("Usage: plutowmctrl <options>\n"
					"\nOptions: \n"
					"\t-l\t\t\t list all windows]\n"
					"\t-r\t\t\t specify the window class name\n"
					"\t-p x,y,h,w\t\t the position\n"
					"\t-v 0|1\t\t\t visibility\n"
					"\t-f 0|1\t\t\t fullscreen or not\n"					
					"\t-z above|normal|below\t the layer (z order)\n"
				);
				return -1;
		}
	}
	
	//printf("list %d,\nwindow name '%s',\nposition '%s',\nvisible '%s'\n",
//		bListWindows, sWindowName.c_str(), sPosition.c_str(), sVisible.c_str());

	if(bListWindows)
		WMController::Instance().ListWindows();
		
	if(sWindowName != "")
	{
		if(sPosition != "")
		{
			//TODO: parse params
			WMController::Instance().SetPosition(sWindowName, 10, 10, 100, 100);
		}
		
		if(sVisible == "1" || sVisible == "0")
		{
			WMController::Instance().SetVisible(sWindowName, sVisible == "1");
		}

		if(sFullScreen == "1" || sFullScreen == "0")
		{
			WMController::Instance().SetFullScreen(sWindowName, sFullScreen == "1");
		}		
				
		if(sLayer != "")
		{
			WMController::Instance().SetLayer(sWindowName, 
				sLayer == "above" ? wlAbove : 
					sLayer == "below" ? wlBelow : 
						sLayer == "normal" ? wlNormal : wlUnknown);
		}
	}

	return 0;
}
