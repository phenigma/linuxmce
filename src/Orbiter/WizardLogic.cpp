#include "Orbiter.h"
#include "WizardLogic.h"
#include "CreateDevice/UserUtils.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_Country.h"
#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_DeviceTemplate.h"
#include "Gen_Devices/AllCommandsRequests.h"

WizardLogic::WizardLogic(Orbiter *pOrbiter)
{
	m_pOrbiter = pOrbiter;
	m_pUserUtils = new UserUtils(this,m_pOrbiter->m_pData->m_dwPK_Installation);
	m_nPK_Device_ZWave = m_nPK_Device_AlarmPanel = 0;
}

WizardLogic::~WizardLogic()
{
	delete m_pUserUtils;
}

bool WizardLogic::Setup()
{
	if( !MySQLConnect("192.168.80.1"/*m_pOrbiter->m_sIPAddress*/,"root", "", "pluto_main") )
		return false;

	string sSQL;
	MYSQL_ROW row;

	sSQL = "SELECT PK_RoomType,Description FROM RoomType ORDER BY Description";
	{
		PlutoSqlResult result_set_roomtypes;
		if( (result_set_roomtypes.r=mysql_query_result(sSQL)) )
			while ((row = mysql_fetch_row(result_set_roomtypes.r)))
			{
				m_listRoomTypes.push_back( make_pair<int,string> (atoi(row[0]),row[1]) );
				m_mapRoomTypes[atoi(row[0])] = row[1];
			}
	}

	m_nPK_Device_TVProvider_External=m_nPK_Device_TV=m_nPK_Device_Receiver=
		m_nPK_Command_Input_Video_On_TV=0;
	m_bUsingReceiverForVideo=false;

	sSQL = "SELECT PK_Device FROM Device WHERE FK_DeviceTemplate=" + StringUtils::itos(DEVICETEMPLATE_ZWave_CONST);
	{
		PlutoSqlResult result_set;
		if( (result_set.r=mysql_query_result(sSQL)) && ((row = mysql_fetch_row(result_set.r))) )
			m_nPK_Device_ZWave = atoi( row[0] );
	}

	return true;
}

/*
	SETUP USERS
*/

bool WizardLogic::AlreadyHasUsers()
{
	return m_pUserUtils->AlreadyHasUsers();
}

int WizardLogic::AddUser(string sUsername)
{
	return m_pUserUtils->AddUser(sUsername);
}

void WizardLogic::RemoveUser(int PK_User)
{
	m_pUserUtils->RemoveUser(PK_User);
}

// Get a map of RoomTypes to number of Rooms of that Type, and the total number of rooms
int WizardLogic::PreSeedRoomInfo( map<int, int > &mapRooms )
{
	int iNumRooms=0;
	string sSQL = "SELECT FK_RoomType,count(PK_Room) FROM Room "
		"JOIN RoomType ON FK_RoomType=PK_RoomType "
		"GROUP BY FK_RoomType";

	PlutoSqlResult result_set_room;
	MYSQL_ROW row;
	if( (result_set_room.r=mysql_query_result(sSQL)) )
		while ((row = mysql_fetch_row(result_set_room.r)))
		{
			mapRooms[ atoi(row[0]) ] = atoi(row[1]);
			iNumRooms += atoi(row[1]);
		}

	return iNumRooms;
}

void WizardLogic::ProcessUpdatedRoomInfo( map<int, int > &mapRooms )
{
	for( map<int, int >::iterator it=mapRooms.begin();it!=mapRooms.end();++it)
	{
		int iNumRooms = 0;  // How many rooms do we already have of this type in the database
		string sSQL = "SELECT count(PK_Room) FROM Room WHERE FK_RoomType=" + StringUtils::itos(it->first);

		PlutoSqlResult result_set_room;
		MYSQL_ROW row;
		if( (result_set_room.r=mysql_query_result(sSQL)) && (row = mysql_fetch_row(result_set_room.r)) )
			iNumRooms = atoi(row[0]);

		if( iNumRooms == it->second )
			continue;  // We already have the correct number
		else if( iNumRooms > it->second )
			RemoveRoomsOfType( it->first, iNumRooms, it->second );
		else
			AddRoomsOfType( it->first, iNumRooms, it->second );
	}
}

void WizardLogic::RemoveRoomsOfType( int PK_RoomType, int NumRoomsCurrent, int NumRoomsDesired )
{
	for(int iRoom = NumRoomsCurrent; iRoom > NumRoomsDesired; --iRoom)
	{
		// Find the room of this type that has the least number of devices/command groups
		string sSQL = "SELECT PK_Room,count(PK_Device) as NumDevice,count(FK_CommandGroup) as NumCommandGroup FROM Room "
			"LEFT JOIN Device ON Device.FK_Room=PK_Room "
			"LEFT JOIN CommandGroup_Room ON CommandGroup_Room.FK_Room=PK_Room "
			"WHERE FK_RoomType=" + StringUtils::itos(PK_RoomType) +
			" GROUP BY PK_Room "
			"ORDER BY NumDevice,NumCommandGroup "
			"LIMIT 1;";

		int PK_Room=0;
		PlutoSqlResult result_set_room;
		MYSQL_ROW row;
		if( (result_set_room.r=mysql_query_result(sSQL)) && (row = mysql_fetch_row(result_set_room.r)) )
			PK_Room = atoi(row[0]);

		if( !PK_Room )
		{
			g_pPlutoLogger->Write(LV_CRITICAL,"WizardLogic::RemoveRoomsOfType cannot find a room to delete: type %d current %d desired %d",
				PK_RoomType, NumRoomsCurrent, NumRoomsDesired);
			continue;
		}

		sSQL = "DELETE FROM Room WHERE PK_Room=" + StringUtils::itos(PK_Room);
		threaded_mysql_query(sSQL);

		sSQL = "UPDATE Device SET FK_Room=NULL WHERE FK_Room=" + StringUtils::itos(PK_Room);
		threaded_mysql_query(sSQL);

		sSQL = "DELETE FROM CommandGroup_Room WHERE FK_Room=" + StringUtils::itos(PK_Room);
		threaded_mysql_query(sSQL);

		sSQL = "DELETE FROM EntertainArea WHERE FK_Room=" + StringUtils::itos(PK_Room);
		threaded_mysql_query(sSQL);

		sSQL = "DELETE FROM Room_Users WHERE FK_Room=" + StringUtils::itos(PK_Room);
		threaded_mysql_query(sSQL);
	}
}

void WizardLogic::AddRoomOfType(int PK_RoomType)
{
	string sSQL = "SELECT Count(*) FROM Room "
		"WHERE FK_RoomType=" + StringUtils::itos(PK_RoomType) +
		" AND FK_Installation = " + Installation_get();

	PlutoSqlResult result_get_rooms;
	MYSQL_ROW row;
	long nNumberOfRooms = 0;
	if((result_get_rooms.r=mysql_query_result(sSQL)) && (row = mysql_fetch_row(result_get_rooms.r)))
		nNumberOfRooms = atoi(row[0]);

	AddRoomsOfType(PK_RoomType, nNumberOfRooms, nNumberOfRooms + 1); //just add a room
}

void WizardLogic::RemoveRoomOfType(int PK_RoomType)
{
	string sSQL = "SELECT Count(*) FROM Room "
		"WHERE FK_RoomType=" + StringUtils::itos(PK_RoomType) +
		" AND FK_Installation = " + Installation_get();

	PlutoSqlResult result_get_rooms;
	MYSQL_ROW row;
	long nNumberOfRooms = 0;
	if((result_get_rooms.r=mysql_query_result(sSQL)) && (row = mysql_fetch_row(result_get_rooms.r)))
		nNumberOfRooms = atoi(row[0]);

	if(nNumberOfRooms)
		RemoveRoomsOfType(PK_RoomType, nNumberOfRooms, nNumberOfRooms - 1);//just remove one room

}

void WizardLogic::AddRoomsOfType( int PK_RoomType, int NumRoomsCurrent, int NumRoomsDesired )
{
	// Simple, just add a room to the database
	for(int iRoom = NumRoomsCurrent; iRoom < NumRoomsDesired; ++iRoom)
	{
		string sDescription = m_mapRoomTypes[PK_RoomType];
		if( NumRoomsDesired > 1 )
			sDescription += " #" + StringUtils::itos(iRoom);

		string sSQL = "INSERT INTO Room(FK_Installation,FK_RoomType,Description) "
			"VALUES(" + Installation_get() + "," +
			StringUtils::itos(PK_RoomType) + ",'" + StringUtils::SQLEscape(sDescription) + "');";
		threaded_mysql_query(sSQL);
	}
}

void WizardLogic::ChangeRoomName(int PK_Room, string sName)
{
	if(PK_Room)
	{
		string sSQL = "UPDATE Room SET Description = '" + sName + "' WHERE PK_Room = " + StringUtils::ltos(PK_Room);
		threaded_mysql_query(sSQL);
	}
}

int WizardLogic::GetCountry()
{
	string sSQL = "SELECT FK_Country FROM Installation WHERE PK_Installation=" + 
		Installation_get();


	PlutoSqlResult result_set;
	MYSQL_ROW row;
	if( (result_set.r=mysql_query_result(sSQL)) && 
		(row = mysql_fetch_row(result_set.r)) && row[0] && atoi(row[0]) )
			return atoi(row[0]);

	return COUNTRY_UNITED_STATES_CONST; 
}

void WizardLogic::SetCountry(int PK_Country)
{ 
	string sSQL = "UPDATE Installation SET City=NULL,State=NULL,Zip=NULL,FK_City=NULL,FK_PostalCode=NULL WHERE PK_Installation=" + 
		Installation_get() + 
		" AND FK_Country<>" + StringUtils::itos(PK_Country);

	threaded_mysql_query(sSQL); 

	sSQL = "UPDATE Installation SET FK_Country=" + StringUtils::itos(PK_Country) + 
		" WHERE PK_Installation=" + 
		Installation_get();

	threaded_mysql_query(sSQL); 
}

string WizardLogic::GetCityRegion() 
{ 
	string sSQL = "SELECT City,State FROM Installation WHERE PK_Installation=" + 
		Installation_get();

	PlutoSqlResult result_set;
	MYSQL_ROW row;
	if( (result_set.r=mysql_query_result(sSQL)) && 
		(row = mysql_fetch_row(result_set.r)) && row[0] )
		return string(row[0]) + (row[1] ? ", " + string(row[1]) : string(""));

	return ""; 
}

bool WizardLogic::SetPostalCode(string PostalCode) 
{
	int PK_Country = GetCountry();
	string sSQL = "SELECT City,State,FK_City,`Long`,Lat FROM PostalCode WHERE PostalCode='"
		+ StringUtils::SQLEscape(PostalCode) + "' AND FK_Country=" + StringUtils::itos(PK_Country);

	PlutoSqlResult result_set;
	MYSQL_ROW row;
	if( (result_set.r=mysql_query_result(sSQL)) && 
		(row = mysql_fetch_row(result_set.r)) && row[0] )
	{
		sSQL = "UPDATE Installation SET City='" + StringUtils::SQLEscape(row[0]) + 
			"'" +
			(row[1] ? string(",State='") + StringUtils::SQLEscape(row[1]) + "'": string("")) + 
			(row[2] ? string(",FK_City='") + StringUtils::SQLEscape(row[2]) + "'" : string(""));
		threaded_mysql_query(sSQL);
		
		DeviceData_Base *pDevice_Event_Plugin = 
			m_pOrbiter->m_pData->m_AllDevices.m_mapDeviceData_Base_FindFirstOfCategory(DEVICECATEGORY_Event_Plugins_CONST);
		if( pDevice_Event_Plugin )
		{
			sSQL = "INSERT INTO Device_DeviceData(FK_Device,FK_DeviceData) VALUES(" + 
				StringUtils::itos(pDevice_Event_Plugin->m_dwPK_Device) + "," + 
				StringUtils::itos(DEVICEDATA_Longitude_CONST) + ")";
			threaded_mysql_query(sSQL,true);  // Ignore errors, this may already be there

			sSQL = "INSERT INTO Device_DeviceData(FK_Device,FK_DeviceData) VALUES(" + 
				StringUtils::itos(pDevice_Event_Plugin->m_dwPK_Device) + "," + 
				StringUtils::itos(DEVICEDATA_Latitude_CONST) + ")";
			threaded_mysql_query(sSQL,true);  // Ignore errors, this may already be there

			sSQL = string("UPDATE Device_DeviceData SET IK_DeviceData='") + row[3] + "'" +
				" WHERE FK_Device=" + StringUtils::itos(pDevice_Event_Plugin->m_dwPK_Device) +
				" AND FK_DeviceData=" + StringUtils::itos(DEVICEDATA_Longitude_CONST);
			threaded_mysql_query(sSQL);

			sSQL = string("UPDATE Device_DeviceData SET IK_DeviceData='") + row[4] + "'" +
				" WHERE FK_Device=" + StringUtils::itos(pDevice_Event_Plugin->m_dwPK_Device) +
				" AND FK_DeviceData=" + StringUtils::itos(DEVICEDATA_Latitude_CONST);
			threaded_mysql_query(sSQL);
		}
		return true;
	}
	return false;
}

void WizardLogic::SetAvPath(int PK_Device_From,int PK_Device_To,int PK_Pipe,int PK_Command_Input)
{
	string sSQL = "DELETE FROM Device_Device_Pipe WHERE FK_Device_From=" + StringUtils::itos(PK_Device_From) + 
		" AND FK_Pipe=" + StringUtils::itos(PK_Pipe);
	threaded_mysql_query(sSQL);

	sSQL = "INSERT INTO Device_Device_Pipe(FK_Device_From,FK_Device_To,FK_Pipe,FK_Command_Input)"
		" VALUES(" + StringUtils::itos(PK_Device_From) + "," + StringUtils::itos(PK_Device_To) + ","
		+ StringUtils::itos(PK_Pipe) + "," + StringUtils::itos(PK_Command_Input) + ")";

	threaded_mysql_query(sSQL);
}

int WizardLogic::AddDevice(int PK_DeviceTemplate)
{
	int iPK_Device=0;
	DCE::CMD_Create_Device_Cat CMD_Create_Device_Cat(m_pOrbiter->m_dwPK_Device,DEVICECATEGORY_General_Info_Plugins_CONST,false,BL_SameHouse,
		PK_DeviceTemplate,"",m_pOrbiter->m_pData->m_dwPK_Room,"","",0,0,m_pOrbiter->m_pData->m_dwPK_Device,m_pOrbiter->m_pData->m_dwPK_Device,
		&iPK_Device);
	m_pOrbiter->SendCommand(CMD_Create_Device_Cat);
	return iPK_Device;
}

void WizardLogic::AddExternalTuner(int PK_Device_Tuner)
{
	DeviceData_Base *pDevice_PVR = m_pOrbiter->m_pData->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_PVR_Capture_Cards_CONST);
	if( pDevice_PVR )
	{
		SetAvPath(PK_Device_Tuner,pDevice_PVR->m_dwPK_Device,1,COMMAND_AV_1_CONST);
		SetAvPath(PK_Device_Tuner,pDevice_PVR->m_dwPK_Device,2,COMMAND_AV_1_CONST);
	}
}


void WizardLogic::DeleteDevicesInThisRoomOfType(int PK_DeviceCategory)
{
	string sSQL = "SELECT PK_Device FROM Device "
		"join DeviceTemplate ON FK_DeviceTemplate = PK_DeviceTemplate "
		"join DeviceCategory ON FK_DeviceCategory = PK_DeviceCategory "
		"where PK_DeviceCategory=" + StringUtils::itos(PK_DeviceCategory) + 
		" OR FK_DeviceCategory_Parent=" + StringUtils::itos(PK_DeviceCategory) + 
		" AND FK_Room=" + StringUtils::itos(m_pOrbiter->m_pData->m_dwPK_Room);

	PlutoSqlResult result_set;
	MYSQL_ROW row;
	if( (result_set.r=mysql_query_result(sSQL)) )
		while ((row = mysql_fetch_row(result_set.r)))
		{
			DCE::CMD_Delete_Device_Cat CMD_Delete_Device_Cat(m_pOrbiter->m_dwPK_Device,
				DEVICECATEGORY_General_Info_Plugins_CONST,false,BL_SameHouse,atoi(row[0]));
			m_pOrbiter->SendCommand(CMD_Delete_Device_Cat);
		}
}

string WizardLogic::GetDeviceStatus(long nPK_Device)
{
	string sSQL = "SELECT Status FROM Device WHERE PK_Device = " + StringUtils::ltos(nPK_Device);

	PlutoSqlResult result_set;
	MYSQL_ROW row;
	if((result_set.r = mysql_query_result(sSQL)) && (row = mysql_fetch_row(result_set.r)) && row[0])
		return row[0];

	return "";
}

int WizardLogic::GetNumLights(int &iNumLightsUnassigned)
{
	m_dequeNumLights.clear();
	string sSQL = "SELECT PK_Device,FK_Room FROM Device "
		"JOIN DeviceTemplate ON FK_DeviceTemplate = PK_DeviceTemplate "
		"JOIN DeviceCategory ON FK_DeviceCategory = PK_DeviceCategory "
		"where PK_DeviceCategory=" + StringUtils::itos(DEVICECATEGORY_Lighting_Device_CONST) + 
		" OR FK_DeviceCategory_Parent=" + StringUtils::itos(DEVICECATEGORY_Lighting_Device_CONST);

	PlutoSqlResult result_set;
	MYSQL_ROW row;
	if( (result_set.r=mysql_query_result(sSQL)) )
		while ((row = mysql_fetch_row(result_set.r)))
		{
			if( !row[1] || !atoi(row[1]) )
				m_dequeNumLights.push_back(atoi(row[0]));
		}

	return (int) result_set.r->row_count;
}
