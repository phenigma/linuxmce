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

#include "pluto_main/Table_Room.h"
#include "pluto_main/Table_Device_EntertainArea.h"
#include "pluto_main/Table_DeviceCategory.h"
#include "pluto_main/Table_DeviceTemplate.h"
#include "pluto_main/Table_Installation.h"
#include "pluto_main/Table_CommandGroup_EntertainArea.h"

#include "pluto_main/Define_FloorplanObjectType.h"
#include "pluto_main/Define_DeviceData.h"

using namespace std;
using namespace DCE;
using namespace DefaultScenarios;

bool UpdateEntArea::Connect(int PK_Installation,string host, string user, string pass, string db_name, int port)
{
	g_pPlutoLogger->Write( LV_STATUS, "Starting UpdateEnt Areas" );
	m_pDatabase_pluto_main = new Database_pluto_main( );
	if( !m_pDatabase_pluto_main->Connect( host, user, pass, db_name, port ) )
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "Cannot connect to database!" );
		return false;
	}
	m_iPK_Installation=PK_Installation;

	// ******** Media Plugin ******************
	vector<Row_Device *> vectRow_Device;
	string sql = "JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_DeviceCategory=" + StringUtils::itos(DEVICECATEGORY_Media_Plugins_CONST);
	if( m_iPK_Installation!=-1 )
		sql += " AND FK_Installation=" + StringUtils::itos(m_iPK_Installation);
	m_pDatabase_pluto_main->Device_get()->GetRows(sql,&vectRow_Device);
	if( vectRow_Device.size()!=1 )
	{
		g_pPlutoLogger->Write( LV_STATUS, "Cannot find a media plugin" );
		return false; // It's useless without one of these
	}
	m_dwPK_Device_MediaPlugIn=vectRow_Device[0]->PK_Device_get();
	m_iPK_Installation = vectRow_Device[0]->FK_Installation_get();

	Row_Installation *pRow_Installation = m_pDatabase_pluto_main->Installation_get()->GetRow(m_iPK_Installation);
	if( !pRow_Installation )
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "Got Media Plugin %d but installation %d is invalid",
			m_dwPK_Device_MediaPlugIn,m_iPK_Installation);
		return false;
	}
	else
		g_pPlutoLogger->Write( LV_STATUS, "Using Media Plugin %d Installation %d",
			m_dwPK_Device_MediaPlugIn,m_iPK_Installation);

	// ******** Orbiter Plugin ******************
	vectRow_Device.clear();
	sql = "JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_DeviceCategory=" + 
		StringUtils::itos(DEVICECATEGORY_Orbiter_Plugins_CONST) + " AND FK_Installation=" + StringUtils::itos(m_iPK_Installation);

	m_pDatabase_pluto_main->Device_get()->GetRows(sql,&vectRow_Device);
	if( vectRow_Device.size()!=1 )
	{
		g_pPlutoLogger->Write( LV_STATUS, "Cannot find a Orbiter plugin" );
		return false; // It's useless without one of these
	}
	else
		m_dwPK_Device_OrbiterPlugIn=vectRow_Device[0]->PK_Device_get();

	// ******** Lighting Plugin ******************
	vectRow_Device.clear();
	sql = "JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_DeviceCategory=" + 
		StringUtils::itos(DEVICECATEGORY_Lighting_Plugins_CONST) + " AND FK_Installation=" + StringUtils::itos(m_iPK_Installation);

	m_pDatabase_pluto_main->Device_get()->GetRows(sql,&vectRow_Device);
	if( vectRow_Device.size()!=1 )
	{
		g_pPlutoLogger->Write( LV_STATUS, "Cannot find a lighting plugin" );
		m_dwPK_Device_LightingPlugIn=0;
	}
	else
		m_dwPK_Device_LightingPlugIn=vectRow_Device[0]->PK_Device_get();

	// ******** Climate Plugin ******************
	vectRow_Device.clear();
	sql = "JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_DeviceCategory=" + 
		StringUtils::itos(DEVICECATEGORY_Climate_Plugins_CONST) + " AND FK_Installation=" + StringUtils::itos(m_iPK_Installation);

	m_pDatabase_pluto_main->Device_get()->GetRows(sql,&vectRow_Device);
	if( vectRow_Device.size()!=1 )
	{
		g_pPlutoLogger->Write( LV_STATUS, "Cannot find a Climate plugin" );
		m_dwPK_Device_ClimatePlugIn=0;
	}
	else
		m_dwPK_Device_ClimatePlugIn=vectRow_Device[0]->PK_Device_get();

	// ******** Security Plugin ******************
	vectRow_Device.clear();
	sql = "JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_DeviceCategory=" + 
		StringUtils::itos(DEVICECATEGORY_Security_Plugins_CONST) + " AND FK_Installation=" + StringUtils::itos(m_iPK_Installation);

	m_pDatabase_pluto_main->Device_get()->GetRows(sql,&vectRow_Device);
	if( vectRow_Device.size()!=1 )
	{
		g_pPlutoLogger->Write( LV_STATUS, "Cannot find a Security plugin" );
		m_dwPK_Device_SecurityPlugIn=0;
	}
	else
		m_dwPK_Device_SecurityPlugIn=vectRow_Device[0]->PK_Device_get();

	// ******** Telecom Plugin ******************
	vectRow_Device.clear();
	sql = "JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_DeviceCategory=" + 
		StringUtils::itos(DEVICECATEGORY_Telecom_Plugins_CONST) + " AND FK_Installation=" + StringUtils::itos(m_iPK_Installation);

	m_pDatabase_pluto_main->Device_get()->GetRows(sql,&vectRow_Device);
	if( vectRow_Device.size()!=1 )
	{
		g_pPlutoLogger->Write( LV_STATUS, "Cannot find a Telecom plugin" );
		m_dwPK_Device_TelecomPlugIn=0;
	}
	else
		m_dwPK_Device_TelecomPlugIn=vectRow_Device[0]->PK_Device_get();
	return true;
}

void UpdateEntArea::GetMediaAndRooms()
{
	// Get all the entertainment areas and populate them with all the devices in those areas
	Row_Installation *pRow_Installation = m_pDatabase_pluto_main->Installation_get( )->GetRow( m_iPK_Installation );
	m_mapRoom_Media.clear();

	if( !pRow_Installation )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"UpdateEntArea::GetMediaAndRooms cannot continue with invalid installation %d", m_iPK_Installation);
		return;
	}

	vector<Row_Room *> vectRow_Room; // Ent Areas are specified by room. Get all the rooms first
	pRow_Installation->Room_FK_Installation_getrows( &vectRow_Room );
	for( size_t iRoom=0;iRoom<vectRow_Room.size( );++iRoom )
	{
		Row_Room *pRow_Room=vectRow_Room[iRoom];
		LevelOfMedia levelOfMedia=lomNone;

		vector<Row_Device *> vectRow_Device; 
		pRow_Room->Device_FK_Room_getrows(&vectRow_Device);
		g_pPlutoLogger->Write(LV_STATUS,"Room: %d %s is auto configure",pRow_Room->PK_Room_get(),pRow_Room->Description_get().c_str());

		// We'll want to configure this room's entertainment area automatically.  See if there's a media director in the room
		for(size_t s=0;s<vectRow_Device.size();++s)
		{
			Row_Device *pRow_Device = vectRow_Device[s];

			// Check up 3 generations of DeviceCategories to see if this is a media director
			if( pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_get()==DEVICECATEGORY_Media_Director_CONST ||
				pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_getrow()->FK_DeviceCategory_Parent_get()==DEVICECATEGORY_Media_Director_CONST ||
				(pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_getrow()->FK_DeviceCategory_Parent_get() &&
				pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_getrow()->FK_DeviceCategory_Parent_getrow()->FK_DeviceCategory_Parent_get()==DEVICECATEGORY_Media_Director_CONST) )
			{
				levelOfMedia = lomContainsMD;
				if( !pRow_Room->ManuallyConfigureEA_get() )
					PutMDsChildrenInRoom(pRow_Device);
			}
		}

		if( levelOfMedia==lomNone )
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
					levelOfMedia=lomContainsOtherVideo;
					break;
				}
			}
			if( levelOfMedia==lomNone )
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
						levelOfMedia=lomContainsAudio;
						break;
					}
				}
			}
		}
		m_mapRoom_Media[pRow_Room->PK_Room_get()] = make_pair<LevelOfMedia, bool> (levelOfMedia,pRow_Room->ManuallyConfigureEA_get()==1);
	}
}

void UpdateEntArea::SetEAInRooms()
{
	for(map<int, pair<LevelOfMedia, bool> >::iterator it=m_mapRoom_Media.begin();it!=m_mapRoom_Media.end();++it)
	{
		Row_Room *pRow_Room = m_pDatabase_pluto_main->Room_get()->GetRow(it->first);
		if( it->second.second )
			continue;  // This is a manually configure.  So do nothing

		vector<Row_EntertainArea *> vectEntertainArea;
		m_pDatabase_pluto_main->EntertainArea_get()->GetRows("FK_Room=" + StringUtils::itos(it->first),&vectEntertainArea);
		if( it->second.first==lomNone )
		{
			// There is no media in here.  Delete any EA's
			for(size_t s=0;s<vectEntertainArea.size();++s)
				DeleteEntertainArea(vectEntertainArea[s]);
		}
		else
		{
			g_pPlutoLogger->Write(LV_STATUS,"Room: %d %s has media ea %d",pRow_Room->PK_Room_get(),pRow_Room->Description_get().c_str(),(int) vectEntertainArea.size());

			// There is media in this room.  Add an entertainment area and any a/v equipment to it
			// First, see if there are already too many ent areas (maybe the user changed from manual to automatic)
			for(size_t s=1;s<vectEntertainArea.size();++s)
				DeleteEntertainArea(vectEntertainArea[s]);

			Row_EntertainArea *pRow_EntertainArea = vectEntertainArea.size() ? vectEntertainArea[0] : NULL;

			if( !pRow_EntertainArea )
			{
				pRow_EntertainArea = m_pDatabase_pluto_main->EntertainArea_get()->AddRow();
				g_pPlutoLogger->Write(LV_STATUS,"set ent area %d %s to Room: %d %s creating default commands",pRow_EntertainArea->PK_EntertainArea_get(),pRow_EntertainArea->Description_get().c_str(),pRow_Room->PK_Room_get(),pRow_Room->Description_get().c_str());
				pRow_EntertainArea->FK_Room_set(pRow_Room->PK_Room_get());
				pRow_EntertainArea->Description_set(pRow_Room->Description_get());
				m_pDatabase_pluto_main->EntertainArea_get()->Commit();
			}
			m_mapEnt_Area_Auto_Media[pRow_EntertainArea->PK_EntertainArea_get()] = it->second.first;

			pRow_EntertainArea->Description_set(pRow_Room->Description_get());
			if( it->second.first==lomContainsMD )
				pRow_EntertainArea->FK_FloorplanObjectType_set(FLOORPLANOBJECTTYPE_ENTERTAINMENT_PLUTO_MEDIA_DIR_CONST);
			else if( it->second.first==lomContainsOtherVideo )
				pRow_EntertainArea->FK_FloorplanObjectType_set(FLOORPLANOBJECTTYPE_ENTERTAINMENT_NONPLUTO_TV_CONST);
			else if( it->second.first==lomContainsAudio )
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
			m_pDatabase_pluto_main->Commit();
		}
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

void UpdateEntArea::GetDevicesTypes(int PK_DeviceCategory,int PK_Room,map<int,int> *p_map_Device_Type,map<int,list<int> > *p_mapType)
{
	string sSQL = "SELECT PK_Device,IK_DeviceData "
		"FROM Device "
		"JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate "
		"JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory "
		"LEFT JOIN Device_DeviceData ON FK_Device=PK_Device AND FK_DeviceData=" + StringUtils::itos(DEVICEDATA_PK_FloorplanObjectType_CONST) +
		" WHERE (PK_DeviceCategory=" + StringUtils::itos(PK_DeviceCategory) + 
		" OR FK_DeviceCategory_Parent=" + StringUtils::itos(PK_DeviceCategory) +
		")";

	if( PK_Room )
		sSQL += " AND FK_Room=" + StringUtils::itos(PK_Room);
    
	PlutoSqlResult result_set;
	MYSQL_ROW row;
	if( (result_set.r=m_pDatabase_pluto_main->mysql_query_result(sSQL)) )
		while ((row = mysql_fetch_row(result_set.r)))
			(*p_map_Device_Type)[ atoi(row[0]) ] = row[1] ? atoi(row[1]) : 0;

	if( p_mapType )
		for(map<int,int>::iterator it=p_map_Device_Type->begin();it!=p_map_Device_Type->end();++it)
			(*p_mapType)[it->first].push_back(it->second);
}

void UpdateEntArea::GetDevicesTypesAndRoomTypes(int PK_DeviceCategory,map<int,pair<int,int> > *p_map_Device_Type_RoomType)
{
	string sSQL = "SELECT PK_Device,IK_DeviceData,FK_RoomType "
		"FROM Device "
		"LEFT JOIN Room ON FK_Room=PK_Room "
		"JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate "
		"JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory "
		"LEFT JOIN Device_DeviceData ON FK_Device=PK_Device AND FK_DeviceData=" + StringUtils::itos(DEVICEDATA_PK_FloorplanObjectType_CONST) +
		" WHERE (PK_DeviceCategory=" + StringUtils::itos(PK_DeviceCategory) + 
		" OR FK_DeviceCategory_Parent=" + StringUtils::itos(PK_DeviceCategory) +
		")";
	
	PlutoSqlResult result_set;
	MYSQL_ROW row;
	if( (result_set.r=m_pDatabase_pluto_main->mysql_query_result(sSQL)) )
		while ((row = mysql_fetch_row(result_set.r)))
			(*p_map_Device_Type_RoomType)[ atoi(row[0]) ] = make_pair<int,int> (row[1] ? atoi(row[1]) : 0,row[2] ? atoi(row[2]) : 0);
}
