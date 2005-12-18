#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "Logger.h"
#include "UpdateEntArea.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <list>

#ifdef WIN32
#include <direct.h>
#include <conio.h>
#define chdir _chdir  // Why, Microsoft, why?
#define mkdir _mkdir  // Why, Microsoft, why?
#else

#endif

#include "pluto_main/Table_Device_DeviceData.h"
#include "pluto_main/Table_DeviceData.h"
#include "pluto_main/Table_DeviceGroup.h"
#include "pluto_main/Table_DeviceCategory.h"
#include "pluto_main/Table_DeviceTemplate.h"
#include "pluto_main/Table_Installation.h"
#include "pluto_main/Table_Room.h"
#include "pluto_main/Define_FloorplanObjectType.h"
#include "pluto_main/Define_Template.h"
#include "pluto_main/Define_Array.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "pluto_main/Define_Variable.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_UserMode.h"
#include "pluto_main/Define_CommandParameter.h"
#include "pluto_main/Define_Icon.h"
#include "pluto_main/Table_Command.h"
#include "pluto_main/Table_CommandGroup.h"
#include "pluto_main/Table_CommandGroup_Command.h"
#include "pluto_main/Table_CommandGroup_Command_CommandParameter.h"
#include "pluto_main/Table_CommandGroup_Room.h"
#include "pluto_main/Table_CommandParameter.h"
#include "pluto_main/Table_Users.h"
#include "pluto_main/Define_Screen.h"
#include "pluto_main/Define_RoomType.h"

#include "CommandGroupArray.h"

using namespace std;
using namespace DCE;
using namespace DefaultScenarios;

bool UpdateEntArea::IsLight(int PK_FloorplanType)
{
	return PK_FloorplanType==FLOORPLANOBJECTTYPE_LIGHT_CEILING_LIGHT_CONST ||
		PK_FloorplanType==FLOORPLANOBJECTTYPE_LIGHT_TABLE_LAMP_CONST ||
		PK_FloorplanType==FLOORPLANOBJECTTYPE_LIGHT_WALL_SCONCE_CONST ||
		PK_FloorplanType==FLOORPLANOBJECTTYPE_LIGHT_FLOOR_LAMP_CONST ||
		PK_FloorplanType==FLOORPLANOBJECTTYPE_LIGHT_CHANDALIER_CONST ||
		PK_FloorplanType==FLOORPLANOBJECTTYPE_LIGHT_PICTURE_LIGHT_CONST ||
		PK_FloorplanType==FLOORPLANOBJECTTYPE_LIGHT_ACCENT_LIGHT_CONST;
}

bool UpdateEntArea::IsPublicInteriorRoom(int PK_RoomType)
{
	return PK_RoomType==ROOMTYPE_Living_Room_CONST ||
		PK_RoomType==ROOMTYPE_ClosetStorage_Space_CONST || 
		PK_RoomType==ROOMTYPE_Garage_CONST || 
		PK_RoomType==ROOMTYPE_Kitchen_CONST || 
		PK_RoomType==ROOMTYPE_Dining_Room_CONST || 
		PK_RoomType==ROOMTYPE_Hallway_CONST;
}

bool UpdateEntArea::IsInteriorRoom(int PK_RoomType)
{
	return PK_RoomType==ROOMTYPE_Living_Room_CONST ||
		PK_RoomType==ROOMTYPE_ClosetStorage_Space_CONST || 
		PK_RoomType==ROOMTYPE_Garage_CONST || 
		PK_RoomType==ROOMTYPE_Kitchen_CONST || 
		PK_RoomType==ROOMTYPE_Dining_Room_CONST || 
		PK_RoomType==ROOMTYPE_Hallway_CONST ||
		PK_RoomType==ROOMTYPE_Bedroom_CONST ||
		PK_RoomType==ROOMTYPE_Master_Bedroom_CONST;
}

bool UpdateEntArea::IsExterior(int PK_RoomType)
{
	return PK_RoomType==ROOMTYPE_Patio_CONST ||
		PK_RoomType==ROOMTYPE_Outside_Area_CONST;
}

void UpdateEntArea::AddDefaultLightingScenarios()
{
	for(map<int, pair<LevelOfMedia, bool> >::iterator it=m_mapRoom_Media.begin();it!=m_mapRoom_Media.end();++it)
	{
		Row_Room *pRow_Room = m_pDatabase_pluto_main->Room_get()->GetRow(it->first);
		if( pRow_Room )
			AddDefaultLightingScenarios(pRow_Room);
	}
}

void UpdateEntArea::AddDefaultLightingScenarios(Row_Room *pRow_Room)
{
	CommandGroup *pCommandGroup;
	string sSQL;
	CommandGroupArray commandGroupArray(pRow_Room,ARRAY_Lighting_Scenarios_CONST,true);

	map<int,int> map_Device_Type; // Where type is a floorplan type
	map<int,list<int> > mapType; // For each type a list of the devices 
	GetDevicesTypes(DEVICECATEGORY_Lighting_Device_CONST,pRow_Room->PK_Room_get(),
		&map_Device_Type,&mapType);

	bool bRoomHasLights=false;
	for(map<int,int>::iterator it=map_Device_Type.begin();it!=map_Device_Type.end();++it)
		if( IsLight(it->second) )
			bRoomHasLights=true;

	if( bRoomHasLights )
	{
		pCommandGroup = commandGroupArray.FindCommandGroupByTemplate(TEMPLATE_Lighting_Automatic_CONST,"On",ICON_Lights_On_CONST,1,0);
		if( pCommandGroup )
		{
			for(map<int,int>::iterator it=map_Device_Type.begin();it!=map_Device_Type.end();++it)
				if( IsLight(it->second) )
					pCommandGroup->AddCommand(it->first,COMMAND_Set_Level_CONST,1,1,COMMANDPARAMETER_Level_CONST,"100");
		}

		pCommandGroup = commandGroupArray.FindCommandGroupByTemplate(TEMPLATE_Lighting_Automatic_CONST,"Off",ICON_Lights_Off_CONST,0,0);
		if( pCommandGroup )
		{
			for(map<int,int>::iterator it=map_Device_Type.begin();it!=map_Device_Type.end();++it)
				if( IsLight(it->second) )
					pCommandGroup->AddCommand(it->first,COMMAND_Generic_Off_CONST,1,0);
		}
	}

	// If there are any lights or blinds at all, we'll create a ShowTime scenario if there's also a TV
	if( map_Device_Type.size() )
	{
		map<int,int> map_Device_Type_TV; // Where type is a floorplan type
		GetDevicesTypes(DEVICECATEGORY_TVsPlasmasLCDsProjectors_CONST,pRow_Room->PK_Room_get(),
			&map_Device_Type_TV,NULL);

		// If there's a TV in the room, we'll add a Showtime scenario
		if( map_Device_Type_TV.size() )
		{
			pCommandGroup = commandGroupArray.FindCommandGroupByTemplate(TEMPLATE_Lighting_Automatic_CONST,"Showtime",ICON_Showtime_CONST,2,0);  // Showtime is parm1=2
			if( pCommandGroup )
			{
				for(map<int,int>::iterator it=map_Device_Type.begin();it!=map_Device_Type.end();++it)
				{
					// Put ceiling lights at 10%, accent lights at 30%, blinds closed, other lights off
					if( it->second==FLOORPLANOBJECTTYPE_LIGHT_CEILING_LIGHT_CONST )
						pCommandGroup->AddCommand(it->first,COMMAND_Set_Level_CONST,1,1,COMMANDPARAMETER_Level_CONST,"10");
					else if( it->second==FLOORPLANOBJECTTYPE_LIGHT_ACCENT_LIGHT_CONST || it->second==FLOORPLANOBJECTTYPE_LIGHT_PICTURE_LIGHT_CONST )
						pCommandGroup->AddCommand(it->first,COMMAND_Set_Level_CONST,1,1,COMMANDPARAMETER_Level_CONST,"30");
					else if( it->second==FLOORPLANOBJECTTYPE_LIGHT_BLINDS_CONST || it->second==FLOORPLANOBJECTTYPE_LIGHTS_DRAPES_CONST )
						pCommandGroup->AddCommand(it->first,COMMAND_Generic_Off_CONST,1,0);
					else if( IsLight(it->second) )
						pCommandGroup->AddCommand(it->first,COMMAND_Generic_Off_CONST,1,0);
				}
			}
		}
	}

	if( pRow_Room->FK_RoomType_get()==ROOMTYPE_Bedroom_CONST || pRow_Room->FK_RoomType_get()==ROOMTYPE_Master_Bedroom_CONST )
	{
		pCommandGroup = commandGroupArray.FindCommandGroupByTemplate(TEMPLATE_Lighting_Automatic_CONST,"Sleep",ICON_Sleep_CONST,3,0);  // Bedtime is parm1=3
		if( pCommandGroup )
		{
			for(map<int,int>::iterator it=map_Device_Type.begin();it!=map_Device_Type.end();++it)
			{
				if( IsLight(it->second) )
					pCommandGroup->AddCommand(it->first,COMMAND_Generic_Off_CONST,1,0);
				else if( it->second==FLOORPLANOBJECTTYPE_LIGHT_BLINDS_CONST || it->second==FLOORPLANOBJECTTYPE_LIGHTS_DRAPES_CONST )
					pCommandGroup->AddCommand(it->first,COMMAND_Generic_Off_CONST,1,0);
			}
			pCommandGroup->AddCommand(m_dwPK_Device_MediaPlugIn,COMMAND_MH_Stop_Media_CONST,1,0);
			pCommandGroup->AddCommand(m_dwPK_Device_TelecomPlugIn,COMMAND_Set_User_Mode_CONST,1,1,
				COMMANDPARAMETER_PK_UserMode_CONST,StringUtils::itos(USERMODE_Sleeping_CONST).c_str());
		}

		pCommandGroup = commandGroupArray.FindCommandGroupByTemplate(TEMPLATE_Lighting_Automatic_CONST,"Wakeup",ICON_Sleep_CONST,4,0);  // Wakeup is parm1=4
		if( pCommandGroup )
		{
			for(map<int,int>::iterator it=map_Device_Type.begin();it!=map_Device_Type.end();++it)
			{
				if( IsLight(it->second) )
					pCommandGroup->AddCommand(it->first,COMMAND_Set_Level_CONST,1,1,COMMANDPARAMETER_Level_CONST,"30");
				else if( it->second==FLOORPLANOBJECTTYPE_LIGHT_BLINDS_CONST || it->second==FLOORPLANOBJECTTYPE_LIGHTS_DRAPES_CONST )
					pCommandGroup->AddCommand(it->first,COMMAND_Generic_On_CONST,1,0);
			}
			pCommandGroup->AddCommand(m_dwPK_Device_TelecomPlugIn,COMMAND_Set_User_Mode_CONST,1,1,
				COMMANDPARAMETER_PK_UserMode_CONST,StringUtils::itos(USERMODE_At_Home_CONST).c_str());
		}
	}

	if( pRow_Room->FK_RoomType_get()==ROOMTYPE_Master_Bedroom_CONST )
	{
		pCommandGroup = commandGroupArray.FindCommandGroupByTemplate(TEMPLATE_Lighting_Automatic_CONST,"House to sleep mode",ICON_House_to_Sleep_Mode_CONST,5,0);  // House to sleep mode is parm1=5
		if( pCommandGroup )
		{
			int PK_CommandGroup; // Get the sleeping group for this room
			commandGroupArray.FindCommandGroupByTemplate(TEMPLATE_Lighting_Automatic_CONST,"Sleep",ICON_Sleep_CONST,3,0,&PK_CommandGroup);  // Bedtime is parm1=3

			map<int,pair<int,int> > map_Device_Type_RoomType;
			GetDevicesTypesAndRoomTypes(DEVICECATEGORY_Lighting_Device_CONST,&map_Device_Type_RoomType);

			for(map<int,pair<int,int> >::iterator it=map_Device_Type_RoomType.begin();it!=map_Device_Type_RoomType.end();++it)
			{
				if( IsPublicInteriorRoom(it->second.second) )
				{
					pCommandGroup->AddCommand(it->first,COMMAND_Generic_Off_CONST,1,0);
					vector<Row_EntertainArea *> vectRow_EntertainArea;
					pRow_Room->EntertainArea_FK_Room_getrows(&vectRow_EntertainArea);
					for(size_t s=0;s<vectRow_EntertainArea.size();++s)
						pCommandGroup->AddCommand(m_dwPK_Device_MediaPlugIn,COMMAND_MH_Stop_Media_CONST,1,1,
						COMMANDPARAMETER_PK_EntertainArea_CONST,StringUtils::itos(vectRow_EntertainArea[s]->PK_EntertainArea_get()).c_str());
				}
				else if( IsExterior(it->second.second) )
					pCommandGroup->AddCommand(it->first,COMMAND_Generic_On_CONST,1,0);
			}
		}
	}
}

