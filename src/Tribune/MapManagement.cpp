#include "MapManagement.h"
#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/StringUtils.h"
#include "CommonFunctions.h"
#include "Database.h"

#include <iostream>
#include <sstream>
#include <map>
#include <string>

map <string,string> MapManagement::GetProgramRecordMap(){

	map<string,string> mapPrimaryKey_Timestam_ProgramRecord;

	std::ostringstream sSQL;
	
	sSQL << "SELECT PK_ProgramRecord, TimestampF FROM ProgramRecord";

	PlutoSqlResult res;
	MYSQL_ROW row=NULL;
	res.r = g_GlobalConfig.m_pDatabase->mysql_query_result( sSQL.str( ) );
	if( !res.r )
	{
		cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
	}
	else
	{
		while( ( row = mysql_fetch_row( res.r ) ) )
		{
			if( !row[0] )
			{
				cerr << "Found NULL in query: " << sSQL.str() << endl;
				
			}
			mapPrimaryKey_Timestam_ProgramRecord[ row[0] ] = row[1];
		}
	}

	return mapPrimaryKey_Timestam_ProgramRecord;
}

map <int,string> MapManagement::GetStationMap(){

	map<int,string> mapPrimaryKey_Timestam_Station;

	std::ostringstream sSQL;
	
	sSQL << "SELECT PK_Station, TimestampF FROM Station";

	PlutoSqlResult res;
	MYSQL_ROW row=NULL;
	res.r = g_GlobalConfig.m_pDatabase->mysql_query_result( sSQL.str( ) );
	if( !res.r )
	{
		cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
	}
	else
	{
		while( ( row = mysql_fetch_row( res.r ) ) )
		{
			if( !row[0] )
			{
				cerr << "Found NULL in query: " << sSQL.str() << endl;
			}
			mapPrimaryKey_Timestam_Station[ atoi(row[0]) ] = row[1];
		}
	}

	return mapPrimaryKey_Timestam_Station;
}

map <string,string> MapManagement::GetScheduleMap(){

	map<string,string> mapPrimaryKey_Timestam_Schedule;

	std::ostringstream sSQL;
	
	sSQL << "SELECT PK_Schedule, TimestampF FROM Schedule";

	PlutoSqlResult res;
	MYSQL_ROW row=NULL;
	res.r = g_GlobalConfig.m_pDatabase->mysql_query_result( sSQL.str( ) );
	if( !res.r )
	{
		cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
	}
	else
	{
		while( ( row = mysql_fetch_row( res.r ) ) )
		{
			if( !row[0] )
			{
				cerr << "Found NULL in query: " << sSQL.str() << endl;
				
			}
			mapPrimaryKey_Timestam_Schedule[ row[0] ] = row[1];
		}
	}

	return mapPrimaryKey_Timestam_Schedule;
}

map <string,string> MapManagement::GetActorMap(){

	map<string,string> mapPrimaryKey_Timestam_Actor;

	std::ostringstream sSQL;
	
	sSQL << "SELECT PK_Actor, TimestampF FROM Actor";

	PlutoSqlResult res;
	MYSQL_ROW row=NULL;
	res.r = g_GlobalConfig.m_pDatabase->mysql_query_result( sSQL.str( ) );
	if( !res.r )
	{
		cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
	}
	else
	{
		while( ( row = mysql_fetch_row( res.r ) ) )
		{
			if( !row[0] )
			{
				cerr << "Found NULL in query: " << sSQL.str() << endl;
				
			}
			mapPrimaryKey_Timestam_Actor[ row[0] ] = row[1];
		}
	}

	return mapPrimaryKey_Timestam_Actor;
}

map <string,string> MapManagement::GetGenreMap(){

	map<string,string> mapPrimaryKey_Timestam_Genre;

	std::ostringstream sSQL;
	
	sSQL << "SELECT PK_Genre, TimestampF FROM Genre";

	PlutoSqlResult res;
	MYSQL_ROW row=NULL;
	res.r = g_GlobalConfig.m_pDatabase->mysql_query_result( sSQL.str( ) );
	if( !res.r )
	{
		cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
	}
	else
	{
		while( ( row = mysql_fetch_row( res.r ) ) )
		{
			if( !row[0] )
			{
				cerr << "Found NULL in query: " << sSQL.str() << endl;
				
			}
			mapPrimaryKey_Timestam_Genre[ row[0] ] = row[1];
		}
	}

	return mapPrimaryKey_Timestam_Genre;
}

map <string,string> MapManagement::GetRoleMap(){

	map<string,string> mapPrimaryKey_Timestam_Role;

	std::ostringstream sSQL;
	
	sSQL << "SELECT PK_Role, TimestampF FROM Role";

	PlutoSqlResult res;
	MYSQL_ROW row=NULL;
	res.r = g_GlobalConfig.m_pDatabase->mysql_query_result( sSQL.str( ) );
	if( !res.r )
	{
		cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
	}
	else
	{
		while( ( row = mysql_fetch_row( res.r ) ) )
		{
			if( !row[0] )
			{
				cerr << "Found NULL in query: " << sSQL.str() << endl;
				
			}
			mapPrimaryKey_Timestam_Role[ row[0] ] = row[1];
		}
	}

	return mapPrimaryKey_Timestam_Role;
}
