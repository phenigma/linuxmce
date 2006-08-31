//<-dceag-d-b->
#include "Photo_Screen_Saver.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->
#include "Gallery.h"
#include "../pluto_main/Define_DeviceData.h"

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Photo_Screen_Saver::Photo_Screen_Saver(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Photo_Screen_Saver_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
	, ThreadID(0)
{
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
Photo_Screen_Saver::Photo_Screen_Saver(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: Photo_Screen_Saver_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
}

//<-dceag-dest-b->
Photo_Screen_Saver::~Photo_Screen_Saver()
//<-dceag-dest-e->
{
	
}

//<-dceag-getconfig-b->
bool Photo_Screen_Saver::GetConfig()
{
	if( !Photo_Screen_Saver_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated
	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Photo_Screen_Saver::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
Photo_Screen_Saver_Command *Create_Photo_Screen_Saver(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new Photo_Screen_Saver(pPrimaryDeviceCommand, pData, pEvent, pRouter);
}
//<-dceag-createinst-e->

/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will 
	get all	commands for your children in ReceivedCommandForChild, where 
	pDeviceData_Base is the child device.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void Photo_Screen_Saver::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
	sCMD_Result = "UNHANDLED CHILD";
}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void Photo_Screen_Saver::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

//<-dceag-sample-b->!

/*

	COMMANDS TO IMPLEMENT

*/

class GallerySetup
{
	int Width_, Height_;
	string SearchImagesPath_;
	int ZoomTime, FaddingTime;
public:
	pthread_t* ThreadID;
	
	GallerySetup(string Width, string Height, int ZoomTime, int FaddingTime, string SearchImagesPath);
	int GetWidth();
	int GetHeight();
	int GetZoomTime();
	int GetFaddingTime();
	string GetSearchImagesPath();
};

GallerySetup::GallerySetup(string Width, string Height, int ZoomTime, int FaddingTime, string SearchImagesPath)
{
	this->Width_ = atoi(Width.c_str());
	this->Height_ = atoi(Height.c_str());
	this->SearchImagesPath_ = SearchImagesPath;

	this->ZoomTime = ZoomTime;
	this->FaddingTime = FaddingTime;
}

int GallerySetup::GetWidth()
{
	return this->Width_;
}
int GallerySetup::GetHeight()
{
	return Height_;
}

string GallerySetup::GetSearchImagesPath()
{
	return SearchImagesPath_;
}

int GallerySetup::GetZoomTime()
{
	return ZoomTime;
}

int GallerySetup::GetFaddingTime()
{
	return FaddingTime;
}



//<-dceag-c192-b->

	/** @brief COMMAND: #192 - On */
	/** Turn on the device */
		/** @param #97 PK_Pipe */
			/** Normally when a device is turned on all the inputs and outputs are selected automatically.  If this parameter is specified, only the settings along this pipe will be set. */
		/** @param #98 PK_Device_Pipes */
			/** Normally when a device is turned on the corresponding "pipes" are enabled by default. if this parameter is blank.  If this parameter is 0, no pipes will be enabled.  This can also be a comma seperated list of devices, meaning only the pipes to those devic */

void Photo_Screen_Saver::CMD_On(int iPK_Pipe,string sPK_Device_Pipes,string &sCMD_Result,Message *pMessage)
//<-dceag-c192-e->
{
	cout << "Need to implement command #192 - On" << endl;
	cout << "Parm #97 - PK_Pipe=" << iPK_Pipe << endl;
	cout << "Parm #98 - PK_Device_Pipes=" << sPK_Device_Pipes << endl;

	string SearchImagesPath = this->DATA_Get_Directories();

	string w = this->GetCurrentDeviceData(m_pData->m_dwPK_Device_ControlledVia, DEVICEDATA_ScreenWidth_CONST);
	string h = this->GetCurrentDeviceData(m_pData->m_dwPK_Device_ControlledVia, DEVICEDATA_ScreenHeight_CONST);
	
	GallerySetup* SetupInfo = new GallerySetup(w, h, DATA_Get_ZoomTime(), DATA_Get_FadeTime(), SearchImagesPath);
	if(0 == ThreadID)
	{
		SetupInfo->ThreadID = &ThreadID;
		pthread_create(&ThreadID, NULL, ThreadAnimation, SetupInfo);
	}
}

//<-dceag-c193-b->

	/** @brief COMMAND: #193 - Off */
	/** Turn off the device */
		/** @param #97 PK_Pipe */
			/** Normally when a device is turned on all the inputs and outputs are selected automatically.  If this parameter is specified, only the settings along this pipe will be set. */

void Photo_Screen_Saver::CMD_Off(int iPK_Pipe,string &sCMD_Result,Message *pMessage)
//<-dceag-c193-e->
{
	cout << "Need to implement command #193 - Off" << endl;
	cout << "Parm #97 - PK_Pipe=" << iPK_Pipe << endl;

	WM_Event Event;
	Event.Quit();
	Gallery::Instance()->EvaluateEvent(Event);
	pthread_join(ThreadID, NULL);
	ThreadID = 0;
}

void* ThreadAnimation(void* ThreadInfo)
{
	GallerySetup* Info = (GallerySetup*) ThreadInfo;
	Gallery::Instance()->Setup(Info->GetWidth(), Info->GetHeight(), 
		Info->GetFaddingTime(),
		Info->GetZoomTime(),
		Info->GetSearchImagesPath());
	Gallery::Instance()->MainLoop(); 
	Gallery::Instance()->CleanUp();

	*(Info->ThreadID) = 0;
	delete Info;
	return NULL;
}

