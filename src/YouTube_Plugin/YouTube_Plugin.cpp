/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#include "YouTube_Plugin.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
YouTube_Plugin::YouTube_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: YouTube_Plugin_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
YouTube_Plugin::~YouTube_Plugin()
//<-dceag-dest-e->
{
	
}

//<-dceag-getconfig-b->
bool YouTube_Plugin::GetConfig()
{
	if( !YouTube_Plugin_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated
	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool YouTube_Plugin::Register()
//<-dceag-reg-e->
{
	m_pMedia_Plugin=( Media_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Media_Plugin_CONST);
	if( !m_pMedia_Plugin )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find sister plugins to youtube plugin");
		return false;
	}

	vector<int> vectPK_DeviceTemplate;
	vectPK_DeviceTemplate.push_back(DEVICETEMPLATE_YouTube_Plugin_CONST);  // No media associated with this. we're just going to populate the grids ourselves and maange our own media
	m_pMedia_Plugin->RegisterMediaPlugin( this, this, vectPK_DeviceTemplate, true );

	return Connect(PK_DeviceTemplate_get()); 
}

//<-dceag-createinst-b->!

/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will 
	get all	commands for your children in ReceivedCommandForChild, where 
	pDeviceData_Base is the child device.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void YouTube_Plugin::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void YouTube_Plugin::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN COMMAND";
}

//<-dceag-sample-b->!

/*

	COMMANDS TO IMPLEMENT

*/

class MediaStream *YouTube_Plugin::CreateMediaStream( class MediaHandlerInfo *pMediaHandlerInfo, int iPK_MediaProvider, vector<class EntertainArea *> &vectEntertainArea, MediaDevice *pMediaDevice, int iPK_Users, deque<MediaFile *> *dequeFilenames, int StreamID )
{
	return new MediaStream(pMediaHandlerInfo,iPK_MediaProvider,pMediaDevice,iPK_Users,DCE::st_Broadcast,StreamID);
}

bool YouTube_Plugin::StartMedia( class MediaStream *pMediaStream,string &sError )
{
	return true;
}

bool YouTube_Plugin::StopMedia( class MediaStream *pMediaStream )
{
	return true;
}

void YouTube_Plugin::PopulateDataGrid(string sToken,MediaListGrid *pMediaListGrid,int PK_MediaType, string sPK_Attribute, int PK_AttributeType_Sort, bool bShowFiles, string &sPK_MediaSubType, string &sPK_FileFormat, string &sPK_Attribute_Genres, string &sPK_Sources, string &sPK_Users_Private, int PK_Users, int iLastViewed, int *iPK_Variable, string *sValue_To_Assign )
{
	if( sPK_Attribute.empty()==false )
	{
		// If this empty we're drilling down to another directory
		if( sPK_Attribute=="URL for sub folder")
		{
			FileBrowserInfo *pFileBrowserInfo = new FileBrowserInfo("B 1-sub folder","!e," + sToken + ",URL2 for sub folder",100,StringUtils::SQLDateTime("2008-02-10"));
			pFileBrowserInfo->m_PK_Picture = 100;
			pMediaListGrid->m_listFileBrowserInfo.push_back(pFileBrowserInfo);

			pFileBrowserInfo = new FileBrowserInfo("B 2-immediate media file","!E," + sToken + ",URL for B2",100,StringUtils::SQLDateTime("2008-02-9"));
			pFileBrowserInfo->m_PK_Picture = 101;
			pMediaListGrid->m_listFileBrowserInfo.push_back(pFileBrowserInfo);

			pFileBrowserInfo = new FileBrowserInfo("B 3-download media file","!o," + sToken + ",URL for B3",100,StringUtils::SQLDateTime("2008-02-8"));
			pFileBrowserInfo->m_PK_Picture = 102;
			pMediaListGrid->m_listFileBrowserInfo.push_back(pFileBrowserInfo);
		}
		else
		{
			FileBrowserInfo *pFileBrowserInfo = new FileBrowserInfo("C 2-immediate media file","!E," + sToken + ",URL for C2",100,StringUtils::SQLDateTime("2008-02-9"));
			pFileBrowserInfo->m_PK_Picture = 101;
			pMediaListGrid->m_listFileBrowserInfo.push_back(pFileBrowserInfo);

			pFileBrowserInfo = new FileBrowserInfo("C 3-download media file","!o," + sToken + ",URL for C3",100,StringUtils::SQLDateTime("2008-02-8"));
			pFileBrowserInfo->m_PK_Picture = 102;
			pMediaListGrid->m_listFileBrowserInfo.push_back(pFileBrowserInfo);
		}
		return;
	}
	FileBrowserInfo *pFileBrowserInfo = new FileBrowserInfo("A 1-sub folder","!e," + sToken + ",URL for sub folder",100,StringUtils::SQLDateTime("2008-02-10"));
	pFileBrowserInfo->m_PK_Picture = 100;
	pMediaListGrid->m_listFileBrowserInfo.push_back(pFileBrowserInfo);

	pFileBrowserInfo = new FileBrowserInfo("A 2-immediate media file","!E," + sToken + ",URL for A2",100,StringUtils::SQLDateTime("2008-02-9"));
	pFileBrowserInfo->m_PK_Picture = 101;
	pMediaListGrid->m_listFileBrowserInfo.push_back(pFileBrowserInfo);

	pFileBrowserInfo = new FileBrowserInfo("A 3-download media file","!o," + sToken + ",URL for A3",100,StringUtils::SQLDateTime("2008-02-8"));
	pFileBrowserInfo->m_PK_Picture = 102;
	pMediaListGrid->m_listFileBrowserInfo.push_back(pFileBrowserInfo);
}

void YouTube_Plugin::GetExtendedAttributes(string sType, string sPK_MediaSource, string sURL, string *sValue_To_Assign)
{
	*sValue_To_Assign = "FILE\tDummy you tube file" 
			"\tTITLE\tTitle of you tube video"
			"\SYNOPSIS\tSynopsis"
			"\tPICTURE\t/home/mediapics/100.jpg";	
}
