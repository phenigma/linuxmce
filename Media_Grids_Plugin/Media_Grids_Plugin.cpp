//<-dceag-d-b->
#include "Media_Grids_Plugin.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "Media_Grids_Plugin.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"

#include "DCE/DeviceData_Router.h"
#include "DCE/DataGrid.h"
#include "DCERouter/DCERouter.h"
#include "Datagrid_Plugin/Datagrid_Plugin.h"
#include "Media_Plugin/Media_Plugin.h"
#include "Orbiter_Plugin/Orbiter_Plugin.h"
#include "pluto_main/Table_DeviceTemplate.h"
#include "pluto_main/Table_DeviceCategory.h"
#include "pluto_main/Table_MediaType.h"
#include "pluto_main/Table_DataGrid.h"

#include <iostream>
using namespace std;

// If you don't use our directory structure, you may need to change the following path
#include "Gen_Devices/AllCommandsRequests.h"

//<-dceag-const-b->
Media_Grids_Plugin::Media_Grids_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Media_Grids_Plugin_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
}

//<-dceag-dest-b->
Media_Grids_Plugin::~Media_Grids_Plugin()
//<-dceag-dest-e->
{
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Media_Grids_Plugin::Register()
//<-dceag-reg-e->
{
	m_pDatagrid_Plugin=NULL;
	ListCommand_Impl *pListCommand_Impl = m_pRouter->m_mapPlugIn_DeviceTemplate_Find(DEVICETEMPLATE_Datagrid_Plugin_CONST);

	if( !pListCommand_Impl || pListCommand_Impl->size()!=1 )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Media grids cannot find datagrid handler %s",(pListCommand_Impl ? "There were more than 1" : ""));
		return false;
	}

	m_pDatagrid_Plugin=(Datagrid_Plugin *) pListCommand_Impl->front();

	m_pMedia_Plugin=NULL;
	pListCommand_Impl = m_pRouter->m_mapPlugIn_DeviceTemplate_Find(DEVICETEMPLATE_Media_Plugin_CONST);

	if( !pListCommand_Impl || pListCommand_Impl->size()!=1 )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Media grids cannot find Media plug-in %s",(pListCommand_Impl ? "There were more than 1" : ""));
		return false;
	}

	m_pMedia_Plugin=(Media_Plugin *) pListCommand_Impl->front();

	m_pOrbiter_Plugin=NULL;
	pListCommand_Impl = m_pRouter->m_mapPlugIn_DeviceTemplate_Find(DEVICETEMPLATE_Orbiter_Plugin_CONST);

	if( !pListCommand_Impl || pListCommand_Impl->size()!=1 )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Orbiter grids cannot find Orbiter plug-in %s",(pListCommand_Impl ? "There were more than 1" : ""));
		return false;
	}

	m_pOrbiter_Plugin=(Orbiter_Plugin *) pListCommand_Impl->front();
/*
	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&File_Grids_Plugin::DevicesCommands)),
		DGTYPE_DEVICES_ACTIONS);
*/

	/* todo - this is now obsolete -- see media info
	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&Media_Grids_Plugin::MediaTracks))
		,DATAGRID_Media_Tracks_CONST);
*/

	return Connect(); 
}

class DataGridTable *Media_Grids_Plugin::MediaTracks(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage)
{
	// We need to find the orbiter that is requesting the tracks, see what entertainment area it is in, and then show the tracks
	// The grid is is in the format xxxx_controllerid
	string::size_type pos2 = 0;
	StringUtils::Tokenize(GridID, "_", pos2);
	int PK_Controller = atoi(StringUtils::Tokenize(GridID, "_", pos2).c_str());

	class EntertainArea *pEntertainArea;
	OH_Orbiter *pOH_Orbiter = m_pOrbiter_Plugin->m_mapOH_Orbiter_Find(PK_Controller);
	if( !pOH_Orbiter || 
			(pEntertainArea=m_pMedia_Plugin->m_mapEntertainAreas_Find(pOH_Orbiter->m_iPK_EntertainArea))==NULL ||
			!pEntertainArea->m_pMediaStream	)
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"An orbiter is requesting the track list, but I can't find a stream in it's entertainment area");
		return NULL;
	}
			
	DataGridTable *pDataGrid = new DataGridTable();
	DataGridCell *pCell;

	int Track=0;
	for(deque<string>::iterator it=pEntertainArea->m_pMediaStream->m_dequeFilename.begin();
		it!=pEntertainArea->m_pMediaStream->m_dequeFilename.end();++it)
	{
		string Filename = FileUtils::FilenameWithoutPath(*it);
		pCell = new DataGridCell(Filename,StringUtils::itos(Track));
		DCE::CMD_Jump_Position_In_Playlist_Cat cmd(PK_Controller, DEVICECATEGORY_Media_Plugins_CONST, false, BL_SameHouse,StringUtils::itos(Track));
		pCell->m_pMessage = cmd.m_pMessage;
		pDataGrid->SetData(0,Track++,pCell);
	}

	return pDataGrid;
}


/*

	COMMANDS TO IMPLEMENT

*/


//<-dceag-sample-b->! no sample
