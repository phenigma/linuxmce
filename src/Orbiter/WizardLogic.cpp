#include "Orbiter.h"
#include "WizardLogic.h"

WizardLogic::WizardLogic(Orbiter *pOrbiter)
{
	m_pOrbiter = pOrbiter;
}

WizardLogic::~WizardLogic()
{
}

bool WizardLogic::Setup()
{
	if( !MySQLConnect(m_pOrbiter->m_sIPAddress,"root", "", "pluto_main") )
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
			"VALUES(" + StringUtils::itos(m_pOrbiter->m_pData->m_dwPK_Installation) + "," +
			StringUtils::itos(PK_RoomType) + ",'" + StringUtils::SQLEscape(sDescription) + "');";
		threaded_mysql_query(sSQL);
	}
}
