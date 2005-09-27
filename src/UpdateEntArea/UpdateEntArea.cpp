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
#include "pluto_main/Table_Device_EntertainArea.h"
#include "pluto_main/Table_DeviceCategory.h"
#include "pluto_main/Table_DeviceTemplate.h"
#include "pluto_main/Table_Installation.h"
#include "pluto_main/Table_Room.h"
#include "pluto_main/Define_FloorplanObjectType.h"
#include "pluto_main/Define_Template.h"
#include "pluto_main/Define_Array.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "pluto_main/Define_MediaType.h"
#include "pluto_main/Define_Variable.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Table_Command.h"
#include "pluto_main/Table_CommandGroup.h"
#include "pluto_main/Table_CommandGroup_Command.h"
#include "pluto_main/Table_CommandGroup_Command_CommandParameter.h"
#include "pluto_main/Table_CommandGroup_EntertainArea.h"
#include "pluto_main/Table_CommandGroup_Room.h"
#include "pluto_main/Table_CommandParameter.h"
#include "pluto_main/Table_Users.h"

#define  VERSION "<=version=>"

using namespace std;
using namespace DCE;

UpdateEntArea::UpdateEntArea(int PK_Installation,string host, string user, string pass, string db_name, int port)
{
	m_pDatabase_pluto_main = new Database_pluto_main( );
	if( !m_pDatabase_pluto_main->Connect( host, user, pass, db_name, port ) )
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "Cannot connect to database!" );
		return;
	}
	m_iPK_Installation=PK_Installation;

	vector<Row_Device *> vectRow_Device;
	string sql = "JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_DeviceCategory=" + StringUtils::itos(DEVICECATEGORY_Media_Plugins_CONST);
	if( m_iPK_Installation!=-1 )
		sql += " AND FK_Installation=" + StringUtils::itos(m_iPK_Installation);
	m_pDatabase_pluto_main->Device_get()->GetRows(sql,&vectRow_Device);
	if( vectRow_Device.size()!=1 )
	{
		cerr << "Cannot find a media plugin" << endl;
		throw "No Media Plugin";
	}
	m_dwPK_Device_MediaPlugIn=vectRow_Device[0]->PK_Device_get();
	m_iPK_Installation = vectRow_Device[0]->FK_Installation_get();

	vectRow_Device.clear();
	sql = "JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_DeviceCategory=" + StringUtils::itos(DEVICECATEGORY_Orbiter_Plugins_CONST);
	if( m_iPK_Installation!=-1 )
		sql += " AND FK_Installation=" + StringUtils::itos(m_iPK_Installation);
	m_pDatabase_pluto_main->Device_get()->GetRows(sql,&vectRow_Device);
	if( vectRow_Device.size()!=1 )
	{
		cerr << "Cannot find an orbiter plugin" << endl;
		throw "No Orbiter Plugin";
	}
	m_dwPK_Device_OrbiterPlugIn=vectRow_Device[0]->PK_Device_get();

	vectRow_Device.clear();
	sql = "JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_DeviceCategory=" + StringUtils::itos(DEVICECATEGORY_Lighting_Plugins_CONST);
	if( m_iPK_Installation!=-1 )
		sql += " AND FK_Installation=" + StringUtils::itos(m_iPK_Installation);
	m_pDatabase_pluto_main->Device_get()->GetRows(sql,&vectRow_Device);
	if( vectRow_Device.size()!=1 )
	{
		cerr << "Cannot find a lighting plugin" << endl;
		throw "No Orbiter Plugin";
	}
	m_dwPK_Device_LightingPlugIn=vectRow_Device[0]->PK_Device_get();
}

void UpdateEntArea::DoIt()
{
	if( !m_pDatabase_pluto_main->m_bConnected )
	{
		cerr << "Cannot connect to database" << endl;
		exit(1);
	}

	bool bChangedEAs=false;  // If we change anything, we're going to need to regenerate all the Orbiters

	// Get all the entertainment areas and populate them with all the devices in those areas
	Row_Installation *pRow_Installation = m_pDatabase_pluto_main->Installation_get( )->GetRow( m_iPK_Installation );

	vector<Row_Room *> vectRow_Room; // Ent Areas are specified by room. Get all the rooms first
	pRow_Installation->Room_FK_Installation_getrows( &vectRow_Room );
	for( size_t iRoom=0;iRoom<vectRow_Room.size( );++iRoom )
	{
		Row_Room *pRow_Room=vectRow_Room[iRoom];
		if( pRow_Room->ManuallyConfigureEA_get()!=1 )
		{
			vector<Row_Device *> vectRow_Device; 
			pRow_Room->Device_FK_Room_getrows(&vectRow_Device);
			g_pPlutoLogger->Write(LV_STATUS,"Room: %d %s is auto configure",pRow_Room->PK_Room_get(),pRow_Room->Description_get().c_str());
			// We'll want to configure this room's entertainment area automatically.  See if there's a media director in the room
			bool bContainsMD = false;
			for(size_t s=0;s<vectRow_Device.size();++s)
			{
				Row_Device *pRow_Device = vectRow_Device[s];

				// Check up 3 generations of DeviceCategories to see if this is a media director
				if( pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_get()==DEVICECATEGORY_Media_Director_CONST ||
					pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_getrow()->FK_DeviceCategory_Parent_get()==DEVICECATEGORY_Media_Director_CONST ||
					(pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_getrow()->FK_DeviceCategory_Parent_get() &&
					pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_getrow()->FK_DeviceCategory_Parent_getrow()->FK_DeviceCategory_Parent_get()==DEVICECATEGORY_Media_Director_CONST) )
				{
					bContainsMD = true;
					PutMDsChildrenInRoom(pRow_Device);
				}
			}

			bool bContainsOtherVideo=false,bContainsAudio=false;
			if( !bContainsMD )
			{
				g_pPlutoLogger->Write(LV_STATUS,"Room: %d %s has no md",pRow_Room->PK_Room_get(),pRow_Room->Description_get().c_str());
				// See if there is some other video device

				for(size_t s=0;s<vectRow_Device.size();++s)
				{
					Row_Device *pRow_Device = vectRow_Device[s];

					// Check up 3 generations of DeviceCategories to see if this is a tv
					if( pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_get()==DEVICECATEGORY_TVsPlasmasLCDsProjectors_CONST ||
						pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_getrow()->FK_DeviceCategory_Parent_get()==DEVICECATEGORY_TVsPlasmasLCDsProjectors_CONST ||
						(pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_getrow()->FK_DeviceCategory_Parent_get() &&
						pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_getrow()->FK_DeviceCategory_Parent_getrow()->FK_DeviceCategory_Parent_get()==DEVICECATEGORY_TVsPlasmasLCDsProjectors_CONST) )
					{
						bContainsOtherVideo = true;
						break;
					}
				}
				if( !bContainsOtherVideo )
				{
					g_pPlutoLogger->Write(LV_STATUS,"Room: %d %s has no video",pRow_Room->PK_Room_get(),pRow_Room->Description_get().c_str());
					for(size_t s=0;s<vectRow_Device.size();++s)
					{
						Row_Device *pRow_Device = vectRow_Device[s];
						// Check up 3 generations of DeviceCategories to see if this is some other audio device
						if( pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_get()==DEVICECATEGORY_AV_CONST ||
							pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_getrow()->FK_DeviceCategory_Parent_get()==DEVICECATEGORY_AV_CONST ||
							(pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_getrow()->FK_DeviceCategory_Parent_get() &&
							pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_getrow()->FK_DeviceCategory_Parent_getrow()->FK_DeviceCategory_Parent_get()==DEVICECATEGORY_AV_CONST) )
						{
							bContainsAudio = true;
							break;
						}
					}
				}
			}

			vector<Row_EntertainArea *> vectEntertainArea;
			pRow_Room->EntertainArea_FK_Room_getrows(&vectEntertainArea);
			if( !bContainsMD && !bContainsOtherVideo && !bContainsAudio )
			{
				// There's no media in here.  Be sure there is no record in the EntArea database
				if( vectEntertainArea.size() )
					bChangedEAs=true;
				for(size_t s=0;s<vectEntertainArea.size();++s)
					DeleteEntertainArea(vectEntertainArea[s]);
			}
			else
			{
				// There is media in this room.  Add an entertainment area and any a/v equipment to it
				// First, see if there are already too many ent areas (maybe the user changed from advanced to manual)
				if( vectEntertainArea.size()>1 )
					bChangedEAs=true;

				for(size_t s=1;s<vectEntertainArea.size();++s)
					DeleteEntertainArea(vectEntertainArea[s]);

				Row_EntertainArea *pRow_EntertainArea = vectEntertainArea.size() ? vectEntertainArea[0] : NULL;
				if( !pRow_EntertainArea )
				{
					bChangedEAs=true;

					pRow_EntertainArea = m_pDatabase_pluto_main->EntertainArea_get()->AddRow();
					g_pPlutoLogger->Write(LV_STATUS,"set ent area %d %s to Room: %d %s creating default commands",pRow_EntertainArea->PK_EntertainArea_get(),pRow_EntertainArea->Description_get().c_str(),pRow_Room->PK_Room_get(),pRow_Room->Description_get().c_str());
					pRow_EntertainArea->FK_Room_set(pRow_Room->PK_Room_get());
					pRow_EntertainArea->Description_set(pRow_Room->Description_get());
					m_pDatabase_pluto_main->EntertainArea_get()->Commit();
				}

				pRow_EntertainArea->Description_set(pRow_Room->Description_get());
				if( bContainsMD )
					pRow_EntertainArea->FK_FloorplanObjectType_set(FLOORPLANOBJECTTYPE_ENTERTAINMENT_PLUTO_MEDIA_DIR_CONST);
				else if( bContainsOtherVideo )
					pRow_EntertainArea->FK_FloorplanObjectType_set(FLOORPLANOBJECTTYPE_ENTERTAINMENT_NONPLUTO_TV_CONST);
				else if( bContainsAudio )
					pRow_EntertainArea->FK_FloorplanObjectType_set(FLOORPLANOBJECTTYPE_ENTERTAINMENT_AUDIO_ZONE_CONST);

				AddAVDevicesToEntArea(pRow_EntertainArea);
				AddMDsDevicesToEntArea(pRow_EntertainArea);

				// Be sure all the devices in this entertainment area are only in the one entertainment area
				vector<Row_Device_EntertainArea *> vectRow_Device_EntertainArea;
				pRow_EntertainArea->Device_EntertainArea_FK_EntertainArea_getrows(&vectRow_Device_EntertainArea);
				for(size_t s=0;s<vectRow_Device_EntertainArea.size();++s)
				{
					Row_Device_EntertainArea *pRow_Device_EntertainArea = vectRow_Device_EntertainArea[s];
					if( pRow_Device_EntertainArea )
					{
						vector<Row_Device_EntertainArea *> vectRow_Device_EntertainArea2;
						m_pDatabase_pluto_main->Device_EntertainArea_get()->GetRows("WHERE FK_EntertainArea<>" +
							StringUtils::itos(pRow_Device_EntertainArea->FK_EntertainArea_get()) + " AND FK_Device=" + StringUtils::itos(pRow_Device_EntertainArea->FK_Device_get()),
							&vectRow_Device_EntertainArea2);
						for(size_t s2=0;s2<vectRow_Device_EntertainArea2.size();++s2)
							vectRow_Device_EntertainArea2[s2]->Delete();
					}
				}
				m_pDatabase_pluto_main->EntertainArea_get()->Commit();
				m_pDatabase_pluto_main->Device_EntertainArea_get()->Commit();
				AddDefaultCommandsToEntArea(pRow_EntertainArea);
				m_pDatabase_pluto_main->Commit();
			}
		}
		AddDefaultCommandsToRoom(pRow_Room);
		/* todo flag all orbiters
		if( bChangedEAs )
		{
		ListDeviceData_Router *pListMediaDirectors = m_pRouter->m_mapDeviceByCategory_Find(DEVICECATEGORY_Orbiter_CONST);
		for(ListDeviceData_Router::iterator it=pListMediaDirectors->begin();it!=pListMediaDirectors->end();++it)
		{
		DeviceData_Router *pDeviceData_Router = *it;
		pDeviceData_Router->m_pRow_Device->NeedConfigure_set(1);
		}
		m_pDatabase_pluto_main->Device_get()->Commit();
		}
		*/
	}
}

void UpdateEntArea::PutMDsChildrenInRoom(Row_Device *pRow_Device)
{
	vector<Row_Device *> vectRow_Device;
	pRow_Device->Device_FK_Device_ControlledVia_getrows(&vectRow_Device);
	for(size_t s=0;s<vectRow_Device.size();++s)
	{
		Row_Device *pRow_Device_Child = vectRow_Device[s];
		pRow_Device_Child->FK_Room_set(pRow_Device->FK_Room_get());
		pRow_Device_Child->Table_Device_get()->Commit();
		if( pRow_Device_Child->FK_DeviceTemplate_getrow()->FK_DeviceCategory_get()==DEVICECATEGORY_Orbiter_CONST ||
			pRow_Device_Child->FK_DeviceTemplate_getrow()->FK_DeviceCategory_getrow()->FK_DeviceCategory_Parent_get()==DEVICECATEGORY_Orbiter_CONST ||
			(pRow_Device_Child->FK_DeviceTemplate_getrow()->FK_DeviceCategory_getrow()->FK_DeviceCategory_Parent_get() &&
			pRow_Device_Child->FK_DeviceTemplate_getrow()->FK_DeviceCategory_getrow()->FK_DeviceCategory_Parent_getrow()->FK_DeviceCategory_Parent_get()==DEVICECATEGORY_Orbiter_CONST) )
		{
			PutMDsChildrenInRoom(pRow_Device_Child);
		}
	}
}

void UpdateEntArea::DeleteEntertainArea(Row_EntertainArea *pRow_EntertainArea)
{
	vector<Row_CommandGroup_EntertainArea *> vectRow_CommandGroup_EntertainArea;
	pRow_EntertainArea->CommandGroup_EntertainArea_FK_EntertainArea_getrows(&vectRow_CommandGroup_EntertainArea);
	for(size_t s=0;s<vectRow_CommandGroup_EntertainArea.size();++s)
	{
		Row_CommandGroup_EntertainArea *pRow_CommandGroup_EntertainArea = vectRow_CommandGroup_EntertainArea[s];
		pRow_CommandGroup_EntertainArea->Delete();
	}

	vector<Row_Device_EntertainArea *> vectRow_Device_EntertainArea;
	pRow_EntertainArea->Device_EntertainArea_FK_EntertainArea_getrows(&vectRow_Device_EntertainArea);
	for(size_t s=0;s<vectRow_Device_EntertainArea.size();++s)
	{
		Row_Device_EntertainArea *pRow_Device_EntertainArea = vectRow_Device_EntertainArea[s];
		pRow_Device_EntertainArea->Delete();
	}

	pRow_EntertainArea->Delete();

	m_pDatabase_pluto_main->EntertainArea_get()->Commit();
	m_pDatabase_pluto_main->CommandGroup_EntertainArea_get()->Commit();
	m_pDatabase_pluto_main->Device_EntertainArea_get()->Commit();
}

void UpdateEntArea::AddDefaultCommandsToRoom(Row_Room *pRow_Room,int iPK_Template)
{
	int PK_CommandGroup;

	if( (!iPK_Template || iPK_Template==TEMPLATE_Lighting_Automatic_CONST) && (PK_CommandGroup=FindCommandGroupByTemplate(pRow_Room,TEMPLATE_Lighting_Automatic_CONST,ARRAY_Lighting_Scenarios_CONST,"On",1))!=0 )
	{
		AddCommand(PK_CommandGroup,m_dwPK_Device_LightingPlugIn,COMMAND_Set_Level_CONST,1,COMMANDPARAMETER_Level_CONST,"100");
	}

	if( (!iPK_Template || iPK_Template==TEMPLATE_Lighting_Automatic_CONST) && (PK_CommandGroup=FindCommandGroupByTemplate(pRow_Room,TEMPLATE_Lighting_Automatic_CONST,ARRAY_Lighting_Scenarios_CONST,"Off",0))!=0 )
	{
		AddCommand(PK_CommandGroup,m_dwPK_Device_LightingPlugIn,COMMAND_Set_Level_CONST,1,COMMANDPARAMETER_Level_CONST,"0");
	}

	if( (!iPK_Template || iPK_Template==TEMPLATE_Telecom_Automatic_CONST) && (PK_CommandGroup=FindCommandGroupByTemplate(pRow_Room,TEMPLATE_Telecom_Automatic_CONST,ARRAY_Communication_Scenarios_CONST,"Phone",0))!=0 )
	{
		AddCommand(PK_CommandGroup,DEVICETEMPLATE_This_Orbiter_CONST,COMMAND_Goto_Screen_CONST,1,COMMANDPARAMETER_PK_DesignObj_CONST,StringUtils::itos(DESIGNOBJ_mnuSecurityPanel_CONST).c_str());
	}

	{
		vector<Row_Users *> vectRow_Users;
		m_pDatabase_pluto_main->Users_get()->GetRows("true",&vectRow_Users);
		for(size_t s=0;s<vectRow_Users.size();++s)
		{
			Row_Users *pRow_Users = vectRow_Users[s];
			if( !pRow_Users->HasMailbox_get() )
				continue;
// TODO -- Add voicemail indicators
			if( (!iPK_Template || iPK_Template==TEMPLATE_Telecom_Automatic_CONST) && (PK_CommandGroup=FindCommandGroupByTemplate(pRow_Room,TEMPLATE_Telecom_Automatic_CONST,ARRAY_Communication_Scenarios_CONST,"Phone",0))!=0 )
			{
				AddCommand(PK_CommandGroup,DEVICETEMPLATE_This_Orbiter_CONST,COMMAND_Goto_Screen_CONST,1,COMMANDPARAMETER_PK_DesignObj_CONST,StringUtils::itos(DESIGNOBJ_mnuSecurityPanel_CONST).c_str());
			}
		}
	}

	if( (!iPK_Template || iPK_Template==TEMPLATE_Security_Arm_Disarm_CONST) && (PK_CommandGroup=FindCommandGroupByTemplate(pRow_Room,TEMPLATE_Security_Arm_Disarm_CONST,ARRAY_Security_Scenarios_CONST,"Security"))!=0 )
	{
		AddCommand(PK_CommandGroup,DEVICETEMPLATE_This_Orbiter_CONST,COMMAND_Goto_Screen_CONST,1,COMMANDPARAMETER_PK_DesignObj_CONST,StringUtils::itos(DESIGNOBJ_mnuMakeCallFavorites_CONST).c_str());
	}

	{
		vector<Row_DeviceGroup *> vectRow_DeviceGroup;
		m_pDatabase_pluto_main->DeviceGroup_get()->GetRows("true",&vectRow_DeviceGroup);
		for(size_t s=0;s<vectRow_DeviceGroup.size();++s)
		{
			Row_DeviceGroup *pRow_DeviceGroup = vectRow_DeviceGroup[s];
			if( pRow_DeviceGroup->Type_get()==1 )
				continue;
			if( (!iPK_Template || iPK_Template==TEMPLATE_Security_Arm_Disarm_CONST) && (PK_CommandGroup=FindCommandGroupByTemplate(pRow_Room,TEMPLATE_Security_Arm_Disarm_CONST,ARRAY_Security_Scenarios_CONST,"Security",pRow_DeviceGroup->PK_DeviceGroup_get()))!=0 )
			{
				AddCommand(PK_CommandGroup,DEVICETEMPLATE_This_Orbiter_CONST,COMMAND_Set_Variable_CONST,2,COMMANDPARAMETER_PK_Variable_CONST,StringUtils::itos(VARIABLE_PK_DeviceGroup_CONST).c_str(),
					COMMANDPARAMETER_Value_To_Assign_CONST,StringUtils::itos(pRow_DeviceGroup->PK_DeviceGroup_get()));
				AddCommand(PK_CommandGroup,DEVICETEMPLATE_This_Orbiter_CONST,COMMAND_Goto_Screen_CONST,1,COMMANDPARAMETER_PK_DesignObj_CONST,StringUtils::itos(DESIGNOBJ_mnuSecurityPanel_CONST).c_str());
			}
		}
	}

	if( (!iPK_Template || iPK_Template==TEMPLATE_Security_SOS_CONST) && (PK_CommandGroup=FindCommandGroupByTemplate(pRow_Room,TEMPLATE_Security_SOS_CONST,ARRAY_Security_Scenarios_CONST,"*SOS*"))!=0 )
	{
		AddCommand(PK_CommandGroup,DEVICETEMPLATE_This_Orbiter_CONST,COMMAND_Goto_Screen_CONST,1,COMMANDPARAMETER_PK_DesignObj_CONST,StringUtils::itos(DESIGNOBJ_mnuSecurityCameras_CONST).c_str());
	}

	if( (!iPK_Template || iPK_Template==TEMPLATE_Security_View_Cameras_CONST) )
	{
		string sSQL="SELECT PK_Device,Device.Description FROM Device "
			"JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate "
			"WHERE FK_DeviceCategory=" + StringUtils::itos(DEVICECATEGORY_Surveillance_Cameras_CONST);

		PlutoSqlResult result_set;
		MYSQL_ROW row;
		if( (result_set.r=m_pDatabase_pluto_main->mysql_query_result(sSQL)) )
		{
			while ((row = mysql_fetch_row(result_set.r)))
			{
				string sDesc = string("View ") + row[1];
				if( (PK_CommandGroup=FindCommandGroupByTemplate(pRow_Room,TEMPLATE_Security_View_Cameras_CONST,ARRAY_Security_Scenarios_CONST,sDesc,atoi(row[0])))!=0 )
				{
					AddCommand(PK_CommandGroup,DEVICETEMPLATE_This_Orbiter_CONST,COMMAND_Set_Variable_CONST,2,COMMANDPARAMETER_PK_Variable_CONST,StringUtils::itos(VARIABLE_PK_Device_CONST).c_str(),
						COMMANDPARAMETER_Value_To_Assign_CONST,row[0]);
					AddCommand(PK_CommandGroup,DEVICETEMPLATE_This_Orbiter_CONST,COMMAND_Goto_Screen_CONST,1,COMMANDPARAMETER_PK_DesignObj_CONST,StringUtils::itos(DESIGNOBJ_mnuSingleCameraViewOnly_CONST).c_str());
				}
			}
		}
	}
}

void UpdateEntArea::AddDefaultCommandsToEntArea(Row_EntertainArea *pRow_EntertainArea,int iPK_Template)
{
	string sSQL;
	int PK_CommandGroup;

	if( (!iPK_Template || iPK_Template==TEMPLATE_Media_Wiz_Pluto_Sources_CONST) )
	{

		sSQL="SELECT PK_Device,Description FROM Device_EntertainArea "
			"JOIN Device ON FK_Device=PK_Device "
			"JOIN DeviceTemplate_MediaType ON Device.FK_DeviceTemplate=DeviceTemplate_MediaType.FK_DeviceTemplate "
			"WHERE FK_MediaType=" + StringUtils::itos(MEDIATYPE_pluto_LiveTV_CONST) + " AND FK_EntertainArea=" + StringUtils::itos(pRow_EntertainArea->PK_EntertainArea_get());

		PlutoSqlResult result_set;
		MYSQL_ROW row;
		if( (result_set.r=m_pDatabase_pluto_main->mysql_query_result(sSQL)) )
		{
			while ((row = mysql_fetch_row(result_set.r)))
			{
				string sDesc = "TV";
				if( result_set.r->row_count>1 )
					sDesc += string("\n") + row[1];
				if( (PK_CommandGroup=FindCommandGroupByTemplate(pRow_EntertainArea,TEMPLATE_Media_Wiz_Pluto_Sources_CONST,sDesc,atoi(row[0]),MEDIATYPE_pluto_LiveTV_CONST))!=0 )
				{
					AddCommand(PK_CommandGroup,m_dwPK_Device_MediaPlugIn,COMMAND_MH_Play_Media_CONST,2,COMMANDPARAMETER_PK_MediaType_CONST,StringUtils::itos(MEDIATYPE_pluto_LiveTV_CONST).c_str(),COMMANDPARAMETER_PK_Device_CONST,row[0]);
				}
			}
		}
	}
	if( (!iPK_Template || iPK_Template==TEMPLATE_Media_Wiz_FileDisc_CONST) && (PK_CommandGroup=FindCommandGroupByTemplate(pRow_EntertainArea,TEMPLATE_Media_Wiz_FileDisc_CONST,"Movies",0,MEDIATYPE_pluto_DVD_CONST))!=0 )
	{
		AddCommand(PK_CommandGroup,DEVICETEMPLATE_This_Orbiter_CONST,COMMAND_Show_File_List_CONST,1,COMMANDPARAMETER_PK_MediaType_CONST,StringUtils::itos(MEDIATYPE_pluto_DVD_CONST).c_str());
	}
	if( (!iPK_Template || iPK_Template==TEMPLATE_Media_Wiz_FileDisc_CONST) && (PK_CommandGroup=FindCommandGroupByTemplate(pRow_EntertainArea,TEMPLATE_Media_Wiz_FileDisc_CONST,"Videos",0,MEDIATYPE_pluto_StoredVideo_CONST))!=0 )
	{
		AddCommand(PK_CommandGroup,DEVICETEMPLATE_This_Orbiter_CONST,COMMAND_Show_File_List_CONST,1,COMMANDPARAMETER_PK_MediaType_CONST,StringUtils::itos(MEDIATYPE_pluto_StoredVideo_CONST).c_str());
	}
	if( (!iPK_Template || iPK_Template==TEMPLATE_Media_Wiz_FileDisc_CONST) && (PK_CommandGroup=FindCommandGroupByTemplate(pRow_EntertainArea,TEMPLATE_Media_Wiz_FileDisc_CONST,"Music",0,MEDIATYPE_pluto_StoredAudio_CONST))!=0 )
	{
		AddCommand(PK_CommandGroup,DEVICETEMPLATE_This_Orbiter_CONST,COMMAND_Show_File_List_CONST,1,COMMANDPARAMETER_PK_MediaType_CONST,StringUtils::itos(MEDIATYPE_pluto_StoredAudio_CONST).c_str());
	}
	if( (!iPK_Template || iPK_Template==TEMPLATE_Media_Wiz_FileDisc_CONST) && (PK_CommandGroup=FindCommandGroupByTemplate(pRow_EntertainArea,TEMPLATE_Media_Wiz_FileDisc_CONST,"Playlists",0,MEDIATYPE_misc_Playlist_CONST))!=0 )
	{
		AddCommand(PK_CommandGroup,DEVICETEMPLATE_This_Orbiter_CONST,COMMAND_Show_File_List_CONST,1,COMMANDPARAMETER_PK_MediaType_CONST,StringUtils::itos(MEDIATYPE_misc_Playlist_CONST).c_str());
	}
	if( !iPK_Template || iPK_Template==TEMPLATE_Media_Wiz_FileDisc_CONST )
	{
		// Only add this button if there's a disk drive
		string sSQL = "SELECT PK_Device FROM Device_EntertainArea "
			"JOIN Device ON FK_Device=PK_Device "
			"JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate "
			"WHERE FK_DeviceCategory=" + StringUtils::itos(DEVICECATEGORY_Disc_Drives_CONST) + " AND FK_EntertainArea=" + StringUtils::itos(pRow_EntertainArea->PK_EntertainArea_get());
		PlutoSqlResult result_set;
		if( (result_set.r=m_pDatabase_pluto_main->mysql_query_result(sSQL)) && result_set.r->row_count>0 && 
			(PK_CommandGroup=FindCommandGroupByTemplate(pRow_EntertainArea,TEMPLATE_Media_Wiz_FileDisc_CONST,"Play Disc",0,0))!=0 )
			AddCommand(PK_CommandGroup,m_dwPK_Device_MediaPlugIn,COMMAND_MH_Play_Media_CONST,0);
	}
	if( (!iPK_Template || iPK_Template==TEMPLATE_Media_Wiz_FileDisc_CONST) && (PK_CommandGroup=FindCommandGroupByTemplate(pRow_EntertainArea,TEMPLATE_Media_Wiz_FileDisc_CONST,"Pictures",0,MEDIATYPE_pluto_Pictures_CONST))!=0 )
	{
		AddCommand(PK_CommandGroup,DEVICETEMPLATE_This_Orbiter_CONST,COMMAND_Show_File_List_CONST,1,COMMANDPARAMETER_PK_MediaType_CONST,StringUtils::itos(MEDIATYPE_pluto_Pictures_CONST).c_str());
	}
	if( (!iPK_Template || iPK_Template==TEMPLATE_Media_Wiz_FileDisc_CONST) && (PK_CommandGroup=FindCommandGroupByTemplate(pRow_EntertainArea,TEMPLATE_Media_Wiz_FileDisc_CONST,"Docs",0,MEDIATYPE_misc_DocViewer_CONST))!=0 )
	{
		AddCommand(PK_CommandGroup,DEVICETEMPLATE_This_Orbiter_CONST,COMMAND_Show_File_List_CONST,1,COMMANDPARAMETER_PK_MediaType_CONST,StringUtils::itos(MEDIATYPE_misc_DocViewer_CONST).c_str());
	}

	{
		// Find all non-pluto media
		string sSQL="SELECT PK_Device,FK_MediaType,MediaType.Description as MediaType "
			"FROM Device_EntertainArea "
			"JOIN Device ON FK_Device=PK_Device "
			"JOIN DeviceTemplate_MediaType ON Device.FK_DeviceTemplate=DeviceTemplate_MediaType.FK_DeviceTemplate "
			"JOIN MediaType ON FK_MediaType=PK_MediaType "
			"WHERE FK_EntertainArea=" + StringUtils::itos(pRow_EntertainArea->PK_EntertainArea_get()) + " AND FK_MediaType NOT IN (1,2,3,4,5,6,7,20,21,22,23,24)";

		PlutoSqlResult result_set;
		MYSQL_ROW row;
		if( (result_set.r=m_pDatabase_pluto_main->mysql_query_result(sSQL)) )
		{
			while ((row = mysql_fetch_row(result_set.r)))
			{
				Row_Device *pRow_Device = m_pDatabase_pluto_main->Device_get()->GetRow(atoi(row[0]));
				if( !pRow_Device->FK_Device_RouteTo_isNull() )
					pRow_Device = pRow_Device->FK_Device_RouteTo_getrow();

				if( !pRow_Device )
				{
					g_pPlutoLogger->Write(LV_CRITICAL,"Error device %s cannot be located",row[0]);
					continue;
				}

				string sDesc = row[2] + string("\n") + pRow_Device->Description_get();
				if( (PK_CommandGroup=FindCommandGroupByTemplate(pRow_EntertainArea,TEMPLATE_Media_Wiz_NP_Sources_CONST,sDesc,atoi(row[0]),atoi(row[1])))!=0 )
				{
					AddCommand(PK_CommandGroup,m_dwPK_Device_MediaPlugIn,COMMAND_MH_Play_Media_CONST,2,COMMANDPARAMETER_PK_MediaType_CONST,row[1],COMMANDPARAMETER_PK_Device_CONST,row[0]);
				}
			}
		}
	}
}

int UpdateEntArea::FindCommandGroupByTemplate(Row_Room *pRow_Room,int PK_Template,int PK_Array,string sDescription,int TemplateParm1,int TemplateParm2)
{
	string SQL = "JOIN CommandGroup_Room ON FK_CommandGroup=PK_CommandGroup WHERE FK_Room=" + StringUtils::itos(pRow_Room->PK_Room_get()) + 
		" AND FK_Template=" + StringUtils::itos(PK_Template) + " AND FK_Array=" + StringUtils::itos(PK_Array) +
		" AND TemplateParm1=" + StringUtils::itos(TemplateParm1) +
		" AND TemplateParm2=" + StringUtils::itos(TemplateParm2);

	vector<Row_CommandGroup *> vectRow_CommandGroup;
	m_pDatabase_pluto_main->CommandGroup_get()->GetRows(SQL,&vectRow_CommandGroup);
	if( vectRow_CommandGroup.size() )
		return 0;

	Row_CommandGroup *pRow_CommandGroup = m_pDatabase_pluto_main->CommandGroup_get()->AddRow();
	pRow_CommandGroup->Hint_set( pRow_Room->Description_get() );
	pRow_CommandGroup->FK_Template_set(PK_Template);
	pRow_CommandGroup->Description_set(sDescription);
	pRow_CommandGroup->FK_Installation_set( m_iPK_Installation );
	pRow_CommandGroup->FK_Array_set( PK_Array );
	pRow_CommandGroup->TemplateParm1_set(TemplateParm1);
	pRow_CommandGroup->TemplateParm2_set(TemplateParm2);
	pRow_CommandGroup->AutoGenerated_set(true);
	m_pDatabase_pluto_main->CommandGroup_get()->Commit();
	g_pPlutoLogger->Write(LV_STATUS,"Added entertainment area %d %s ea: %d %s hint: %s",
		pRow_CommandGroup->PK_CommandGroup_get(),pRow_CommandGroup->Description_get().c_str(),
		pRow_Room->PK_Room_get(), pRow_Room->Description_get().c_str(),
		pRow_CommandGroup->Hint_get().c_str());
	Row_CommandGroup_Room *pRow_CommandGroup_Room = m_pDatabase_pluto_main->CommandGroup_Room_get()->AddRow();
	pRow_CommandGroup_Room->FK_CommandGroup_set(pRow_CommandGroup->PK_CommandGroup_get());
	pRow_CommandGroup_Room->FK_Room_set(pRow_Room->PK_Room_get());
	pRow_CommandGroup_Room->Sort_set(pRow_CommandGroup->PK_CommandGroup_get());
	m_pDatabase_pluto_main->CommandGroup_Room_get()->Commit();

	return pRow_CommandGroup->PK_CommandGroup_get();
}
int UpdateEntArea::FindCommandGroupByTemplate(Row_EntertainArea *pRow_EntertainArea,int PK_Template,string sDescription,int TemplateParm1,int TemplateParm2)
{
	string SQL = "JOIN CommandGroup_EntertainArea ON FK_CommandGroup=PK_CommandGroup WHERE FK_EntertainArea=" + StringUtils::itos(pRow_EntertainArea->PK_EntertainArea_get()) + 
		" AND FK_Template=" + StringUtils::itos(PK_Template) +
		" AND TemplateParm1=" + StringUtils::itos(TemplateParm1) +
		" AND TemplateParm2=" + StringUtils::itos(TemplateParm2);

	vector<Row_CommandGroup *> vectRow_CommandGroup;
	m_pDatabase_pluto_main->CommandGroup_get()->GetRows(SQL,&vectRow_CommandGroup);
	if( vectRow_CommandGroup.size() )
		return 0;

	Row_CommandGroup *pRow_CommandGroup = m_pDatabase_pluto_main->CommandGroup_get()->AddRow();
	pRow_CommandGroup->Hint_set( pRow_EntertainArea->Description_get() );
	pRow_CommandGroup->FK_Template_set(PK_Template);
	pRow_CommandGroup->Description_set(sDescription);
	pRow_CommandGroup->FK_Installation_set( m_iPK_Installation );
	pRow_CommandGroup->FK_Array_set( ARRAY_Media_Scenarios_CONST );
	pRow_CommandGroup->TemplateParm1_set(TemplateParm1);
	pRow_CommandGroup->TemplateParm2_set(TemplateParm2);
	pRow_CommandGroup->AutoGenerated_set(true);
	m_pDatabase_pluto_main->CommandGroup_get()->Commit();
	g_pPlutoLogger->Write(LV_STATUS,"Added entertainment area %d %s ea: %d %s hint: %s",
		pRow_CommandGroup->PK_CommandGroup_get(),pRow_CommandGroup->Description_get().c_str(),
		pRow_EntertainArea->PK_EntertainArea_get(), pRow_EntertainArea->Description_get().c_str(),
		pRow_CommandGroup->Hint_get().c_str());
	Row_CommandGroup_EntertainArea *pRow_CommandGroup_EntertainArea = m_pDatabase_pluto_main->CommandGroup_EntertainArea_get()->AddRow();
	pRow_CommandGroup_EntertainArea->FK_CommandGroup_set(pRow_CommandGroup->PK_CommandGroup_get());
	pRow_CommandGroup_EntertainArea->FK_EntertainArea_set(pRow_EntertainArea->PK_EntertainArea_get());
	pRow_CommandGroup_EntertainArea->Sort_set(pRow_CommandGroup->PK_CommandGroup_get());
	m_pDatabase_pluto_main->CommandGroup_EntertainArea_get()->Commit();

	return pRow_CommandGroup->PK_CommandGroup_get();
}

void UpdateEntArea::AddCommand(int PK_CommandGroup,int PK_Device,int PK_Command,int NumParms,...)
{
	Row_CommandGroup_Command *pRow_CommandGroup_Command = m_pDatabase_pluto_main->CommandGroup_Command_get()->AddRow();
	pRow_CommandGroup_Command->FK_CommandGroup_set(PK_CommandGroup);
	pRow_CommandGroup_Command->FK_Command_set(PK_Command);
	pRow_CommandGroup_Command->FK_Device_set(PK_Device);
	m_pDatabase_pluto_main->CommandGroup_Command_get()->Commit();

	va_list marker;
	va_start( marker, NumParms );
	for( unsigned long i=0; i < NumParms; i++ )
	{
		Row_CommandGroup_Command_CommandParameter *pRow_CommandGroup_Command_CommandParameter = m_pDatabase_pluto_main->CommandGroup_Command_CommandParameter_get()->AddRow();
		pRow_CommandGroup_Command_CommandParameter->FK_CommandGroup_Command_set(pRow_CommandGroup_Command->PK_CommandGroup_Command_get());
		pRow_CommandGroup_Command_CommandParameter->FK_CommandParameter_set(va_arg( marker, unsigned long ));
		pRow_CommandGroup_Command_CommandParameter->IK_CommandParameter_set(va_arg( marker, char* ));
		m_pDatabase_pluto_main->CommandGroup_Command_CommandParameter_get()->Commit();
	}
	va_end( marker );
}


void UpdateEntArea::AddAVDevicesToEntArea(Row_EntertainArea *pRow_EntertainArea)
{
	vector<Row_Device *> vectRow_Device; 

	// First fix up any embedded devices
	pRow_EntertainArea->FK_Room_getrow()->Device_FK_Room_getrows(&vectRow_Device);
	for(size_t s=0;s<vectRow_Device.size();++s)
	{
		Row_Device *pRow_Device = vectRow_Device[s];
		vector<Row_Device *> vectRow_Device_Embedded; 
		pRow_Device->Device_FK_Device_RouteTo_getrows(&vectRow_Device_Embedded);
		for(size_t s2=0;s2<vectRow_Device_Embedded.size();++s2)
		{
			Row_Device *pRow_Device_Child = vectRow_Device_Embedded[s2];
			if( pRow_Device_Child->FK_Room_get()!=pRow_Device->FK_Room_get() )
			{
				pRow_Device_Child->FK_Room_set(pRow_Device->FK_Room_get());
				pRow_Device_Child->Table_Device_get()->Commit();
			}
		}
	}

	vectRow_Device.clear();
	pRow_EntertainArea->FK_Room_getrow()->Device_FK_Room_getrows(&vectRow_Device);
	for(size_t s=0;s<vectRow_Device.size();++s)
	{
		Row_Device *pRow_Device = vectRow_Device[s];
		// Check up 3 generations of DeviceCategories to see if this is av equipment
		if( pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_get()==DEVICECATEGORY_AV_CONST ||
			pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_getrow()->FK_DeviceCategory_Parent_get()==DEVICECATEGORY_AV_CONST ||
			(pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_getrow()->FK_DeviceCategory_Parent_get() &&
			pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_getrow()->FK_DeviceCategory_Parent_getrow()->FK_DeviceCategory_Parent_get()==DEVICECATEGORY_AV_CONST) )
		{
			Row_Device_EntertainArea *pRow_Device_EntertainArea = m_pDatabase_pluto_main->Device_EntertainArea_get()->GetRow(pRow_Device->PK_Device_get(),pRow_EntertainArea->PK_EntertainArea_get());
			if( !pRow_Device_EntertainArea )
			{
				g_pPlutoLogger->Write( LV_WARNING, "adding device %d %s to ent area %d %s",pRow_Device->PK_Device_get(),pRow_Device->Description_get().c_str(),
					pRow_EntertainArea->PK_EntertainArea_get(),pRow_EntertainArea->Description_get().c_str());
				pRow_Device_EntertainArea = m_pDatabase_pluto_main->Device_EntertainArea_get()->AddRow();
				pRow_Device_EntertainArea->FK_Device_set(pRow_Device->PK_Device_get());
				pRow_Device_EntertainArea->FK_EntertainArea_set(pRow_EntertainArea->PK_EntertainArea_get());
				m_pDatabase_pluto_main->Device_EntertainArea_get()->Commit();
			}
		}
	}
}

void UpdateEntArea::AddMDsDevicesToEntArea(Row_EntertainArea *pRow_EntertainArea)
{
	vector<Row_Device *> vectRow_Device; 
	pRow_EntertainArea->FK_Room_getrow()->Device_FK_Room_getrows(&vectRow_Device);
	for(size_t s=0;s<vectRow_Device.size();++s)
	{
		Row_Device *pRow_Device = vectRow_Device[s];
		// Check up 3 generations of DeviceCategories to see if this is a media director
		if( pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_get()==DEVICECATEGORY_Media_Director_CONST ||
			pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_getrow()->FK_DeviceCategory_Parent_get()==DEVICECATEGORY_Media_Director_CONST ||
			(pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_getrow()->FK_DeviceCategory_Parent_get() &&
			pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_getrow()->FK_DeviceCategory_Parent_getrow()->FK_DeviceCategory_Parent_get()==DEVICECATEGORY_Media_Director_CONST) )
		{
			Row_Device_EntertainArea *pRow_Device_EntertainArea = m_pDatabase_pluto_main->Device_EntertainArea_get()->GetRow(pRow_Device->PK_Device_get(),pRow_EntertainArea->PK_EntertainArea_get());
			if( !pRow_Device_EntertainArea )
			{
				g_pPlutoLogger->Write( LV_CRITICAL, "adding device %d %s to ent area %d %s",pRow_Device->PK_Device_get(),pRow_Device->Description_get().c_str(),pRow_EntertainArea->PK_EntertainArea_get(),pRow_EntertainArea->Description_get().c_str());
				pRow_Device_EntertainArea = m_pDatabase_pluto_main->Device_EntertainArea_get()->AddRow();
				pRow_Device_EntertainArea->FK_Device_set(pRow_Device->PK_Device_get());
				pRow_Device_EntertainArea->FK_EntertainArea_set(pRow_EntertainArea->PK_EntertainArea_get());
				m_pDatabase_pluto_main->Device_EntertainArea_get()->Commit();
			}

			vector<Row_Device *> vectRow_Device_Child;
			pRow_Device->Device_FK_Device_ControlledVia_getrows(&vectRow_Device_Child);
			for(size_t s=0;s<vectRow_Device_Child.size();++s)
			{
				Row_Device *pRow_Device_Child = vectRow_Device_Child[s];
				Row_Device_EntertainArea *pRow_Device_EntertainArea = m_pDatabase_pluto_main->Device_EntertainArea_get()->GetRow(pRow_Device_Child->PK_Device_get(),pRow_EntertainArea->PK_EntertainArea_get());
				if( !pRow_Device_EntertainArea )
				{
					g_pPlutoLogger->Write( LV_CRITICAL, "adding device %d %s to ent area %d %s",
						pRow_Device_Child->PK_Device_get(),pRow_Device_Child->Description_get().c_str(),pRow_EntertainArea->PK_EntertainArea_get(),pRow_EntertainArea->Description_get().c_str());
					pRow_Device_EntertainArea = m_pDatabase_pluto_main->Device_EntertainArea_get()->AddRow();
					pRow_Device_EntertainArea->FK_Device_set(pRow_Device_Child->PK_Device_get());
					pRow_Device_EntertainArea->FK_EntertainArea_set(pRow_EntertainArea->PK_EntertainArea_get());
					m_pDatabase_pluto_main->Device_EntertainArea_get()->Commit();
				}
				vector<Row_Device *> vectRow_Device_GrandChild;
				pRow_Device_Child->Device_FK_Device_ControlledVia_getrows(&vectRow_Device_GrandChild);
				for(size_t s=0;s<vectRow_Device_GrandChild.size();++s)
				{
					Row_Device *Row_Device_GrandChild = vectRow_Device_GrandChild[s];
					Row_Device_EntertainArea *pRow_Device_EntertainArea = m_pDatabase_pluto_main->Device_EntertainArea_get()->GetRow(Row_Device_GrandChild->PK_Device_get(),pRow_EntertainArea->PK_EntertainArea_get());
					if( !pRow_Device_EntertainArea )
					{
						pRow_Device_EntertainArea = m_pDatabase_pluto_main->Device_EntertainArea_get()->AddRow();
						pRow_Device_EntertainArea->FK_Device_set(Row_Device_GrandChild->PK_Device_get());
						pRow_Device_EntertainArea->FK_EntertainArea_set(pRow_EntertainArea->PK_EntertainArea_get());
						m_pDatabase_pluto_main->Device_EntertainArea_get()->Commit();
					}
				}
			}
		}
	}
}
