#include "MapManagement.h"
#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/StringUtils.h"
#include "CommonFunctions.h"
#include "Database.h"

#include <iostream>
#include <sstream>
#include <map>
#include <set>
#include <string>

using namespace Tribune;

bool MapManagement::GetProgramRecordMap(map<string,string> & mapPrimaryKey_Timestam_ProgramRecord){

	std::ostringstream sSQL;
	
	sSQL << "SELECT PK_ProgramRecord, TimestampF FROM ProgramRecord";

	PlutoSqlResult res;
	MYSQL_ROW row=NULL;
	res.r = g_GlobalConfig.m_pDatabase->mysql_query_result( sSQL.str( ) );
	if( !res.r )
	{
		cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
		return false;
	}
	else
	{
		while( ( row = mysql_fetch_row( res.r ) ) )
		{
			if( !row[0] )
			{
				cerr << "Found NULL in query: " << sSQL.str() << endl;
				return false;
				
			}
			mapPrimaryKey_Timestam_ProgramRecord[ row[0] ] = row[1];
		}
	}

	return true;
}

bool MapManagement::GetStationMap(map<int,string> & mapPrimaryKey_Timestam_Station){

	std::ostringstream sSQL;
	
	sSQL << "SELECT PK_Station, TimestampF FROM Station";

	PlutoSqlResult res;
	MYSQL_ROW row=NULL;
	res.r = g_GlobalConfig.m_pDatabase->mysql_query_result( sSQL.str( ) );
	if( !res.r )
	{
		cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
		return false;
	}
	else
	{
		while( ( row = mysql_fetch_row( res.r ) ) )
		{
			if( !row[0] )
			{
				cerr << "Found NULL in query: " << sSQL.str() << endl;
				return false;
			}
			mapPrimaryKey_Timestam_Station[ atoi(row[0]) ] = row[1];
		}
	}

	return true;
}

bool MapManagement::GetScheduleMap(map<string,string> & mapPrimaryKey_Timestam_Schedule){

	std::ostringstream sSQL;
	
	sSQL << "SELECT PK_Schedule, TimestampF FROM Schedule";

	PlutoSqlResult res;
	MYSQL_ROW row=NULL;
	res.r = g_GlobalConfig.m_pDatabase->mysql_query_result( sSQL.str( ) );
	if( !res.r )
	{
		cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
		return false;
	}
	else
	{
		while( ( row = mysql_fetch_row( res.r ) ) )
		{
			if( !row[0] )
			{
				cerr << "Found NULL in query: " << sSQL.str() << endl;
				return false;
				
			}
			mapPrimaryKey_Timestam_Schedule[ row[0] ] = row[1];
		}
	}

	return true;
}

bool MapManagement::GetActorMap(map<string,string> & mapPrimaryKey_Timestam_Actor){

	std::ostringstream sSQL;
	
	sSQL << "SELECT PK_Actor FROM Actor";

	PlutoSqlResult res;
	MYSQL_ROW row=NULL;
	res.r = g_GlobalConfig.m_pDatabase->mysql_query_result( sSQL.str( ) );
	if( !res.r )
	{
		cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
		return false;
	}
	else
	{
		while( ( row = mysql_fetch_row( res.r ) ) )
		{
			if( !row[0] )
			{
				cerr << "Found NULL in query: " << sSQL.str() << endl;
				return false;
				
			}
			mapPrimaryKey_Timestam_Actor[ row[0] ] = "";
		}
	}

	return true;
}

bool MapManagement::GetGenreMap(map<string,string> & mapPrimaryKey_Timestam_Genre){

	std::ostringstream sSQL;
	
	sSQL << "SELECT PK_Genre FROM Genre";

	PlutoSqlResult res;
	MYSQL_ROW row=NULL;
	res.r = g_GlobalConfig.m_pDatabase->mysql_query_result( sSQL.str( ) );
	if( !res.r )
	{
		cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
		return false;
	}
	else
	{
		while( ( row = mysql_fetch_row( res.r ) ) )
		{
			if( !row[0] )
			{
				cerr << "Found NULL in query: " << sSQL.str() << endl;
				return false;
				
			}
			mapPrimaryKey_Timestam_Genre[ row[0] ] = "";
		}
	}

	return true;
}

bool MapManagement::GetRoleMap(map<string,string> & mapPrimaryKey_Timestam_Role){

	std::ostringstream sSQL;
	
	sSQL << "SELECT PK_Role FROM Role";

	PlutoSqlResult res;
	MYSQL_ROW row=NULL;
	res.r = g_GlobalConfig.m_pDatabase->mysql_query_result( sSQL.str( ) );
	if( !res.r )
	{
		cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
		return false;
	}
	else
	{
		while( ( row = mysql_fetch_row( res.r ) ) )
		{
			if( !row[0] )
			{
				cerr << "Found NULL in query: " << sSQL.str() << endl;
				return false;
				
			}
			mapPrimaryKey_Timestam_Role[ row[0] ] = "";
		}
	}

	return true;
}
