#include "PlutoUtils/CommonIncludes.h"
#include "ClientFunctions.h"
#include "Database.h"
#include "CommonFunctions.h"
#include "R_GetLineups.h"
#include "R_GetChannels.h"
#include "R_GetDiffs.h"

#include <iostream>
#include <sstream>
#include <fstream>

#include <map>
#include <string>

map <int,pair<string,string> > ClientFunctions::GetLineups(int zipcode ){

	R_GetLineups r_GetLineups( "Headend", zipcode);
	ra_Processor->AddRequest( &r_GetLineups );
							
	while( ra_Processor->SendRequests(Connection, m_pSocket) );

	if( r_GetLineups.m_cProcessOutcome!=SUCCESSFULLY_PROCESSED )
	{
		cerr << "Cannot get list of records from server (clarifying local deletions destiny):" << (int) r_GetLineups.m_cProcessOutcome << endl;
	}

	map<string,string>::iterator iter;
	int i = 0;
	map <int,pair<string,string> > mapIndexLineupKey_To_Name;
	if (r_GetLineups.m_mapPrimaryKey_LineupName.empty()){
		return mapIndexLineupKey_To_Name;
	}
	for(iter = r_GetLineups.m_mapPrimaryKey_LineupName.begin(); iter != r_GetLineups.m_mapPrimaryKey_LineupName.end(); iter++ ) {
    		//cout << "Id-ul lineupului"<<iter->first << " numele lineupului " << iter->second << endl;
		mapIndexLineupKey_To_Name[i] = make_pair(iter->first,iter->second);
  	}

	return mapIndexLineupKey_To_Name;
}

map <string,string> ClientFunctions::GetChannels(int key, string extra_cond )
{
	R_GetChannels r_GetChannels( key, extra_cond );
	ra_Processor->AddRequest( &r_GetChannels );
							
	while( ra_Processor->SendRequests(Connection, m_pSocket) );

	if( r_GetChannels.m_cProcessOutcome!=SUCCESSFULLY_PROCESSED )
	{
		cerr << "Cannot get list of records from server (clarifying local deletions destiny):" << (int) r_GetChannels.m_cProcessOutcome << endl;
	}

	return r_GetChannels.m_mapPrimaryKey_ChannelName;
}

string ClientFunctions::GetBlackListChannels() {

	std::ostringstream sSQL;
	string blacklist;
	
	sSQL << "SELECT ChannelName FROM BlackListChannels";

	PlutoSqlResult res;
	MYSQL_ROW row=NULL;
	res.r = g_GlobalConfig.m_pDatabase->mysql_query_result( sSQL.str( ) );
	if( !res.r )
	{
		cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
		return blacklist;
	}
	else
	{
		while( ( row = mysql_fetch_row( res.r ) ) )
		{
			if( !row[0] )
			{
				cerr << "Found NULL in query: " << sSQL.str() << endl;
			}
			if (blacklist.length( )==0){
				blacklist = "'"+(string)row[0]+"'";
			}
			blacklist = blacklist+","+"'"+(string)row[0]+"'";
		}
	}

	return blacklist;
}

void ClientFunctions::SetClientLineup(string key, string lineup) {

	std::ostringstream sSQL;
	
	sSQL << "INSERT INTO ClientLineup(PK_ClientLineup, LineupName) VALUES (\""+key+"\",\""+lineup+"\")";

	g_GlobalConfig.m_pDatabase->mysql_query_result( sSQL.str( ) );

}

string ClientFunctions::GetClientLineup() {

	std::ostringstream sSQL;
	string id;
	
	sSQL << "SELECT PK_ClientLineup FROM ClientLineup";

	PlutoSqlResult res;
	MYSQL_ROW row=NULL;
	res.r = g_GlobalConfig.m_pDatabase->mysql_query_result( sSQL.str( ) );
	if( !res.r )
	{
		cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
		return id;
	}
	else
	{
		while( ( row = mysql_fetch_row( res.r ) ) )
		{
			if( !row[0] )
			{
				cerr << "Found NULL in query: " << sSQL.str() << endl;
			}
			id = (string) row[0];
		}
	}

	return id;
	
}

string ClientFunctions::CalcDiffs(string lineup,string blacklist, map <string,string> mapProgramRecord, map <int,string> mapStation, map <string,string> mapSchedule, map <string,string> mapActor, map <string,string> mapGenre, map <string,string> mapRole) {

	R_GetDiffs r_GetDiffs(string lineup, string blacklist, map <string,string> mapProgramRecord, map <int,string> mapStation, map <string,string> mapSchedule, map <string,string> mapActor, map <string,string> mapGenre, map <string,string> mapRole );

	ra_Processor->AddRequest( &r_GetDiffs );
							
	while( ra_Processor->SendRequests(Connection, m_pSocket) );

	if( r_GetDiffs.m_cProcessOutcome!=SUCCESSFULLY_PROCESSED )
	{
		cerr << "Cannot get list of records from server (clarifying local deletions destiny):" << (int) r_GetDiffs.m_cProcessOutcome << endl;
		return r_GetDiffs.m_clientfile;
	}

	return r_GetDiffs.m_clientfile;
}

bool ClientFunctions::ModifyClientDatabase(string path){
	
	ifstream mysqlfile(path);
        string line;
	
	if (!mysqlfile) {
		cerr<<"Error opening file "<<path<<endl;
		exit(1);
	}

	if (mysqlfile.is_open()) {
		
		while(! mysqlfile.eof())
		{
			getline (mysqlfile,line);

			g_GlobalConfig.m_pDatabase->mysql_query_result( line );
			
		}
	}	
}




