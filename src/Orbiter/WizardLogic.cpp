#include "Orbiter.h"
#include "WizardLogic.h"
#include "CreateDevice/UserUtils.h"
#include "pluto_main/Define_Country.h"
#include "pluto_main/Define_DeviceData.h"

WizardLogic::WizardLogic(Orbiter *pOrbiter)
{
	m_pOrbiter = pOrbiter;
	m_pUserUtils = new UserUtils(this,m_pOrbiter->m_pData->m_dwPK_Installation);
}

WizardLogic::~WizardLogic()
{
	delete m_pUserUtils;
}

bool WizardLogic::Setup()
{
	if( !MySQLConnect("10.2.1.162" /*m_pOrbiter->m_sIPAddress*/,"root", "", "pluto_main") )
		return false;

	string sSQL;
	MYSQL_ROW row;

	sSQL = "SELECT PK_RoomType,Description FROM RoomType ORDER BY Description";

	PlutoSqlResult result_set_roomtypes;
	if( (result_set_roomtypes.r=mysql_query_result(sSQL)) )
		while ((row = mysql_fetch_row(result_set_roomtypes.r)))
		{
			m_listRoomTypes.push_back( make_pair<int,string> (atoi(row[0]),row[1]) );
			m_mapRoomTypes[atoi(row[0])] = row[1];
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

string WizardLogic::GetCityRegion(int PK_Country) 
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
			(row[1] ? string(",State=") + StringUtils::SQLEscape(row[1]) : string("")) + 
			(row[2] ? string(",FK_City=") + StringUtils::SQLEscape(row[2]) : string(""));
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
				"WHERE FK_Device=" + StringUtils::itos(pDevice_Event_Plugin->m_dwPK_Device) +
				" AND FK_DeviceData=" + StringUtils::itos(DEVICEDATA_Longitude_CONST);
			threaded_mysql_query(sSQL);

			sSQL = string("UPDATE Device_DeviceData SET IK_DeviceData='") + row[4] + "'" +
				"WHERE FK_Device=" + StringUtils::itos(pDevice_Event_Plugin->m_dwPK_Device) +
				" AND FK_DeviceData=" + StringUtils::itos(DEVICEDATA_Latitude_CONST);
			threaded_mysql_query(sSQL);
		}
		return true;
	}
	return false;
}
