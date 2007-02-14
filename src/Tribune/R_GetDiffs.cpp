#include "R_GetDiffs.h"
#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "Tribuneprocessor.h"
#include "Database.h"
#include "R_GetChannels.h"
//#include "Repository.h"
#include "CommonFunctions.h"

#include <iostream>
#include <sstream>
#include <fstream>

using namespace Tribune;
using namespace std;

R_GetDiffs::R_GetDiffs(string lineup, string blacklist, map <string,string> mapProgramRecord, map <int,string> mapStation, map <string,string> mapSchedule, map <string,string> mapActor, map <string,string> mapGenre, map <string,string> mapRole)
{
	m_lineup = lineup;
	m_blacklist = blacklist;
	m_mapProgramRecord = mapProgramRecord;
        m_mapStation = mapStation;
	m_mapSchedule = mapSchedule;
	m_mapActor = mapActor;
	m_mapGenre = mapGenre;
	m_mapRole = mapRole;
}

bool R_GetDiffs::UpdateProgramRecord(string pk_programrecord){

	std::ostringstream sSQL;
	sSQL << "SELECT * FROM ProgramRecord PK_ProgramRecord=" << pk_programrecord;
		
	PlutoSqlResult res;
	MYSQL_ROW row=NULL;
	res.r = g_GlobalConfig.m_pDatabase->mysql_query_result( sSQL.str( ) );
	if( !res.r )
	{
		cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
		m_cProcessOutcome=INTERNAL_ERROR;
		return false;
	}
	else
	{
		row = mysql_fetch_row( res.r );

		if( !row[0] )
		{
			cerr << "Found NULL in query: " << sSQL.str() << endl;
			m_cProcessOutcome=INTERNAL_ERROR;
			return false; /**<< Request successfully processed */
		}
		clientfile<<"UPDATE ProgramRecord SET Title=\""<<row[1]<<"\", ReducedTitle1=\""<<row[2]<<"\", ReducedTitle2=\""<<row[3]<<"\", ReducedTitle3=\""<<row[4]<<"\", ReducedTitle4=\""<<row[5]<<"\", AltTitle=\""<<row[6]<<"\", ReducedDesc1=\""<<row[7]<<"\", ReducedDesc2=\""<<row[8]<<"\", ReducedDesc3=\""<<row[9]<<"\", AdvisoryAdultContent=\""<<row[10]<<"\", AdvisoryExplicitLanguage=\""<<row[11]<<"\", AdvisoryNudity=\""<<row[12]<<"\", AdvisoryViolence=\""<<row[13]<<"\", AdvisorySexualContent=\""<<row[14]<<"\", AdvisoryRapeContent=\""<<row[15]<<"\", Description=\""<<row[16]<<"\", RealYear=\""<<row[17]<<"\", MpaaRating=\""<<row[18]<<"\", StarRating=\""<<row[19]<<"\", RunTime=\""<<row[20]<<"\", ColorCode=\""<<row[21]<<"\", ProgramLanguage=\""<<row[22]<<"\", OrgCountry=\""<<row[23]<<"\", MadeForTV=\""<<row[24]<<"\", SourceType=\""<<row[25]<<"\", ShowType=\""<<row[26]<<"\", Holiday=\""<<row[27]<<"\", SynEpiNum=\""<<row[28]<<"\", AltSynEpiNum=\""<<row[29]<<"\", EpiTitle=\""<<row[30]<<"\", NetSynSource=\""<<row[31]<<"\", NetSynType=\""<<row[32]<<"\", DescWithActors=\""<<row[33]<<"\", ReducedDescWithActors=\""<<row[34]<<"\", OrgStudio=\""<<row[35]<<"\", Gamedate=\""<<row[36]<<"\", GameTime=\""<<row[37]<<"\", GameTimeZone=\""<<row[38]<<"\", OrgAirDate=\""<<row[39]<<"\", UniqueID=\""<<row[40]<<"\", Checksum=\""<<row[41]<<"\" WHERE PK_ProgramRecord=\""<<pk_programrecord<<"\"\n";

		//m_cProcessOutcome=SUCCESSFULLY_PROCESSED;
	}
	return true;
}

bool R_GetDiffs::UpdateProgramRecord_Genre(string pk_programrecord){
	
	std::ostringstream sSQL;
	sSQL << "SELECT FK_Genre FROM ProgramRecord_Genre FK_ProgramRecord=" << pk_programrecord;
		
	PlutoSqlResult res;
	MYSQL_ROW row=NULL;
	res.r = g_GlobalConfig.m_pDatabase->mysql_query_result( sSQL.str( ) );

	if( !res.r )
	{
		cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
		m_cProcessOutcome=INTERNAL_ERROR;
		return false;
	}
	else
	{
		while( ( row = mysql_fetch_row( res.r ) ) )
		{
			if( !row[0] )
			{
				cerr << "Found NULL in query: " << sSQL.str() << endl;
				m_cProcessOutcome=INTERNAL_ERROR;
				return false; /**<< Request successfully processed */
			}
			clientfile<<"UPDATE ProgramRecord_Genre SET FK_Genre=\""<<row[0]<<"\" WHERE FK_ProgramRecord=\""<<pk_programrecord<<"\"\n";

			map<string,string>::iterator itGen = m_mapGenre.find(row[0]);

			if( itGen == m_mapGenre.end() ){
				
				sSQL << "SELECT Description, Level FROM Genre WHERE PK_Genre=" << row[0];
				
				PlutoSqlResult res1;
				MYSQL_ROW row1=NULL;
				res1.r = g_GlobalConfig.m_pDatabase->mysql_query_result( sSQL.str( ) );

				if( !res1.r )
				{
					cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
					m_cProcessOutcome=INTERNAL_ERROR;
				}
				else
				{
					row1 = mysql_fetch_row( res1.r );
					clientfile<<"INSERT INTO Genre(PK_Genre, Description, Level) VALUES (\""<<row[0]<<"\",\""<<row1[0]<<"\",\""<<row1[1]<<"\")\n";
				}
				

			}
			
		}
	}
	return true;
}

bool R_GetDiffs::UpdateActor_ProgramRecord(string pk_programrecord){

	std::ostringstream sSQL;
	sSQL << "SELECT FK_Actor FROM Actor_ProgramRecord FK_ProgramRecord=" << pk_programrecord;
	PlutoSqlResult res;
	MYSQL_ROW row=NULL;
	res.r = g_GlobalConfig.m_pDatabase->mysql_query_result( sSQL.str( ) );

	if( !res.r )
	{
		cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
		m_cProcessOutcome=INTERNAL_ERROR;
		return false;
	}
	else
	{
		while( ( row = mysql_fetch_row( res.r ) ) )
		{
			if( !row[0] )
			{
				cerr << "Found NULL in query: " << sSQL.str() << endl;
				m_cProcessOutcome=INTERNAL_ERROR;
				return false; /**<< Request successfully processed */
			}
			clientfile<<"UPDATE Actor_ProgramRecord SET FK_Actor=\""<<row[0]<<"\" WHERE FK_ProgramRecord=\""<<pk_programrecord<<"\"\n";

			map<string,string>::iterator itActor = m_mapActor.find(row[0]);

			if( itActor == m_mapActor.end() ){
				
				sSQL << "SELECT FirstName, LastName FROM Actor WHERE PK_Actor=" << row[0];
				
				PlutoSqlResult res1;
				MYSQL_ROW row1=NULL;
				res1.r = g_GlobalConfig.m_pDatabase->mysql_query_result( sSQL.str( ) );

				if( !res1.r )
				{
					cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
					m_cProcessOutcome=INTERNAL_ERROR;
				}
				else
				{
					row1 = mysql_fetch_row( res1.r );
					clientfile<<"INSERT INTO Actor(PK_Actor, FirstName, LastName) VALUES (\""<<row[0]<<"\",\""<<row1[0]<<"\",\""<<row1[1]<<"\")\n";
				}
				

			}

			sSQL << "SELECT FK_Role FROM Actor_Role WHERE FK_Actor=" << row[0];

			PlutoSqlResult res2;
			MYSQL_ROW row2=NULL;
			res2.r = g_GlobalConfig.m_pDatabase->mysql_query_result( sSQL.str( ) );

			if( !res2.r )
			{
				cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
				m_cProcessOutcome=INTERNAL_ERROR;
			}
			else
			{
				while ( ( row2 = mysql_fetch_row( res2.r ) ) ){
					if( !row2[0] )
					{
						cerr << "Found NULL in query: " << sSQL.str() << endl;
						m_cProcessOutcome=INTERNAL_ERROR;
						return false; /**<< Request successfully processed */
					}
					clientfile<<"UPDATE Actor_Role SET FK_Role=\""<<row2[0]<<"\" WHERE FK_Actor=\""<<row[0]<<"\"\n";

					map<string,string>::iterator itRole = m_mapRole.find(row2[0]);

					if( itRole == m_mapRole.end() ){
						
						sSQL << "SELECT Description FROM Role WHERE PK_Role=" << row2[0];
						
						PlutoSqlResult res3;
						MYSQL_ROW row3=NULL;
						res3.r = g_GlobalConfig.m_pDatabase->mysql_query_result( sSQL.str( ) );
		
						if( !res3.r )
						{
							cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
							m_cProcessOutcome=INTERNAL_ERROR;
						}
						else
						{
							row3 = mysql_fetch_row( res3.r );
							clientfile<<"INSERT INTO Role(PK_Role, Description) VALUES (\""<<row2[0]<<"\",\""<<row3[0]<<"\") \n";
						}	
		
					}
				}
			}
			
		}
	}
	return true;
}

bool R_GetDiffs::InsertSchedule(string pk_station){
	
	std::ostringstream sSQL;
	sSQL << "SELECT * from Schedule where FK_Station="<<pk_station;
	PlutoSqlResult res1;
	MYSQL_ROW row1=NULL;
	res1.r = g_GlobalConfig.m_pDatabase->mysql_query_result( sSQL.str( ) );
	if( !res1.r )
	{
		cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
		m_cProcessOutcome=INTERNAL_ERROR;
	}
	else
	{
		while ( ( row1 = mysql_fetch_row( res1.r ) ) ){

			if( !row1[0] )
			{
				cerr << "Found NULL in query: " << sSQL.str() << endl;
				m_cProcessOutcome=INTERNAL_ERROR;
				return false; /**<< Request successfully processed */
			}
			
			map<string,string>::iterator it=m_mapSchedule.find(row1[0]);

			if( it == m_mapSchedule.end() ){
				clientfile<<"INSERT INTO Schedule(PK_Schedule, FK_Station, FK_ProgramRecord, AirDate, AirTime, Duration, PartNum, NumOfParts, CCaptioning, Stereo, Repeating, LiveTapeDelay, Subtitled, PremiereFinale, JoinedInProgress, CableInTheClassroom, TVRating, Sap, Blackout, SexRating, ViolenceRating, LanguageRating, DialogRating, FvRating, Enhanced, ThreeD, LetterBox, Hdtv, Dolby, Dvs, Checksum, TimestampF) VALUES (\""<<row1[0]<<"\",\""<<row1[2]<<"\",\""<<row1[3]<<"\",\""<<row1[4]<<"\",\""<<row1[5]<<"\",\""<<row1[6]<<"\",\""<<row1[7]<<"\",\""<<row1[8]<<"\",\""<<row1[9]<<"\",\""<<row1[10]<<"\",\""<<row1[11]<<"\",\""<<row1[12]<<"\",\""<<row1[13]<<"\",\""<<row1[14]<<"\",\""<<row1[15]<<"\",\""<<row1[16]<<"\",\""<<row1[17]<<"\",\""<<row1[18]<<"\",\""<<row1[19]<<"\",\""<<row1[20]<<"\",\""<<row1[21]<<"\",\""<<row1[22]<<"\",\""<<row1[23]<<"\",\""<<row1[24]<<"\",\""<<row1[25]<<"\",\""<<row1[26]<<"\",\""<<row1[27]<<"\",\""<<row1[28]<<"\",\""<<row1[29]<<"\",\""<<row1[30]<<"\", \""<<row1[31]<<"\", \""<<row1[32]<<"\") \n";
			}
			
		}
	}
	return true;
}

bool R_GetDiffs::InsertProgramRecord_Genre(string pk_programrecord){
	
	std::ostringstream sSQL;
	sSQL << "SELECT FK_Genre FROM ProgramRecord_Genre FK_ProgramRecord=" << pk_programrecord;

	PlutoSqlResult res3;
	MYSQL_ROW row3=NULL;
	res3.r = g_GlobalConfig.m_pDatabase->mysql_query_result( sSQL.str( ) );

	if( !res3.r )
	{
		cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
		m_cProcessOutcome=INTERNAL_ERROR;
	}
	else
	{
		while( ( row3 = mysql_fetch_row( res3.r ) ) )
		{
			if( !row3[0] )
			{
				cerr << "Found NULL in query: " << sSQL.str() << endl;
				m_cProcessOutcome=INTERNAL_ERROR;
				return false; /**<< Request successfully processed */
			}
			clientfile<<"INSERT INTO ProgramRecord_Genre(FK_ProgramRecord, FK_Genre) VALUES(\""<<pk_programrecord<<"\",\""<<row3[0]<<"\") \n";

			map<string,string>::iterator itGen = m_mapGenre.find(row3[0]);

			if( itGen == m_mapGenre.end() ){
				
				sSQL << "SELECT Description, Level FROM Genre WHERE PK_Genre=" << row3[0];
				
				PlutoSqlResult res4;
				MYSQL_ROW row4=NULL;
				res4.r = g_GlobalConfig.m_pDatabase->mysql_query_result( sSQL.str( ) );

				if( !res4.r )
				{
					cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
					m_cProcessOutcome=INTERNAL_ERROR;
				}
				else
				{
					row4 = mysql_fetch_row( res4.r );
					clientfile<<"INSERT INTO Genre(PK_Genre, Description, Level) VALUES (\""<<row3[0]<<"\",\""<<row4[0]<<"\",\""<<row4[1]<<"\")\n";
				}
				
			}
			
		}
	}
	return true;
}

bool R_GetDiffs::InsertActor_ProgramRecord(string pk_programrecord){

	std::ostringstream sSQL;
	sSQL << "SELECT FK_Actor FROM Actor_ProgramRecord FK_ProgramRecord=" << pk_programrecord;
	PlutoSqlResult res3;
	MYSQL_ROW row3=NULL;
	res3.r = g_GlobalConfig.m_pDatabase->mysql_query_result( sSQL.str( ) );

	if( !res3.r )
	{
		cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
		m_cProcessOutcome=INTERNAL_ERROR;
	}
	else
	{
		while( ( row3 = mysql_fetch_row( res3.r ) ) )
		{
			if( !row3[0] )
			{
				cerr << "Found NULL in query: " << sSQL.str() << endl;
				m_cProcessOutcome=INTERNAL_ERROR;
				return false; /**<< Request successfully processed */
			}

			clientfile<<"INSERT INTO Actor_ProgramRecord(FK_Actor, FK_ProgramRecord) VALUES(\""<<row3[0]<<"\",\""<<pk_programrecord<<"\") \n";

			map<string,string>::iterator itActor = m_mapActor.find(row3[0]);

			if( itActor == m_mapActor.end() ){
				
				sSQL << "SELECT FirstName, LastName FROM Actor WHERE PK_Actor=" << row3[0];
				
				PlutoSqlResult res4;
				MYSQL_ROW row4=NULL;
				res4.r = g_GlobalConfig.m_pDatabase->mysql_query_result( sSQL.str( ) );

				if( !res4.r )
				{
					cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
					m_cProcessOutcome=INTERNAL_ERROR;
				}
				else
				{
					row4 = mysql_fetch_row( res4.r );
					clientfile<<"INSERT INTO Actor(PK_Actor, FirstName, LastName) VALUES (\""<<row3[0]<<"\",\""<<row4[0]<<"\",\""<<row4[1]<<"\")\n";
				}
				

			}

			sSQL << "SELECT FK_Role FROM Actor_Role WHERE FK_Actor=" << row3[0];

			PlutoSqlResult res4;
			MYSQL_ROW row4=NULL;
			res4.r = g_GlobalConfig.m_pDatabase->mysql_query_result( sSQL.str( ) );

			if( !res4.r )
			{
				cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
				m_cProcessOutcome=INTERNAL_ERROR;
			}
			else
			{
				while ( ( row4 = mysql_fetch_row( res4.r ) ) ){
					if( !row4[0] )
					{
						cerr << "Found NULL in query: " << sSQL.str() << endl;
						m_cProcessOutcome=INTERNAL_ERROR;
						return false; /**<< Request successfully processed */
					}
					clientfile<<"UPDATE Actor_Role SET FK_Role=\""<<row4[0]<<"\" WHERE FK_Actor=\""<<row3[0]<<"\"\n";

					clientfile<<"INSERT INTO Actor_Role(FK_Actor, FK_Role) VALUES (\""<<row3[0]<<"\",\""<<row4[0]<<"\")\n";

					map<string,string>::iterator itRole = m_mapRole.find(row4[0]);

					if( itRole == m_mapRole.end() ){
						
						sSQL << "SELECT Description FROM Role WHERE PK_Role=" << row4[0];
						
						PlutoSqlResult res5;
						MYSQL_ROW row5=NULL;
						res5.r = g_GlobalConfig.m_pDatabase->mysql_query_result( sSQL.str( ) );
		
						if( !res5.r )
						{
							cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
							m_cProcessOutcome=INTERNAL_ERROR;
						}
						else
						{
							row5 = mysql_fetch_row( res5.r );
							clientfile<<"INSERT INTO Role(PK_Role, Description) VALUES (\""<<row4[0]<<"\",\""<<row5[0]<<"\") \n";
						}	
		
					}
				}
			}
			
		}
	}
	return true;
}

bool R_GetDiffs::InsertProgramRecord(string pk_station){

	std::ostringstream sSQL;
	sSQL<<"Ineterogarea ="<<pk_station;
	PlutoSqlResult res1;
	MYSQL_ROW row1=NULL;
	res1.r = g_GlobalConfig.m_pDatabase->mysql_query_result( sSQL.str( ) );
	if( !res1.r )
	{
		cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
		m_cProcessOutcome=INTERNAL_ERROR;
	}
	else
	{
		while ( ( row1 = mysql_fetch_row( res1.r ) ) ){

			if( !row1[0] )
			{
				cerr << "Found NULL in query: " << sSQL.str() << endl;
				m_cProcessOutcome=INTERNAL_ERROR;
				return false; /**<< Request successfully processed */
			}
			
			map<string,string>::iterator it=m_mapProgramRecord.find(row1[0]);

			if( it == m_mapProgramRecord.end() ){
				clientfile<<"INSERT INTO ProgramRecord(PK_ProgramRecord, Title, ReducedTitle1, ReducedTitle2, ReducedTitle3, ReducedTitle4, AltTitle, ReducedDesc1, ReducedDesc2, ReducedDesc3, AdvisoryAdultContent, AdvisoryExplicitLanguage, AdvisoryNudity, AdvisoryViolence, AdvisorySexualContent, AdvisoryRapeContent, Description, RealYear, MpaaRating, StarRating, RunTime, ColorCode, ProgramLanguage, OrgCountry, MadeForTV, SourceType, ShowType, Holiday, SynEpiNum, AltSynEpiNum, EpiTitle, NetSynSource, NetSynType, DescWithActors, ReducedDescWithActors, OrgStudio, Gamedate, GameTime, GameTimeZone, OrgAirDate, UniqueID, Checksum, TimestampF) VALUES (\""<<row1[0]<<"\",\""<<row1[2]<<"\",\""<<row1[3]<<"\",\""<<row1[4]<<"\",\""<<row1[5]<<"\",\""<<row1[6]<<"\",\""<<row1[7]<<"\",\""<<row1[8]<<"\",\""<<row1[9]<<"\",\""<<row1[10]<<"\",\""<<row1[11]<<"\",\""<<row1[12]<<"\",\""<<row1[13]<<"\",\""<<row1[14]<<"\",\""<<row1[15]<<"\",\""<<row1[16]<<"\",\""<<row1[17]<<"\",\""<<row1[18]<<"\",\""<<row1[19]<<"\",\""<<row1[20]<<"\",\""<<row1[21]<<"\",\""<<row1[22]<<"\",\""<<row1[23]<<"\",\""<<row1[24]<<"\",\""<<row1[25]<<"\",\""<<row1[26]<<"\",\""<<row1[27]<<"\",\""<<row1[28]<<"\",\""<<row1[29]<<"\",\""<<row1[30]<<"\",\""<<row1[31]<<"\",\""<<row1[32]<<"\",\""<<row1[33]<<"\",\""<<row1[34]<<"\",\""<<row1[35]<<"\",\""<<row1[36]<<"\",\""<<row1[37]<<"\",\""<<row1[38]<<"\",\""<<row1[39]<<"\",\""<<row1[40]<<"\",\""<<row1[41]<<"\",\""<<row1[42]<<"\",\""<<row1[43]<<"\") \n";

				if (! InsertProgramRecord_Genre((string)row1[0])){
					return false;
				}
				
				if (! InsertActor_ProgramRecord((string)row1[0])){
					return false;
				}

			}
			
		}
	}
	return true;
}

bool R_GetDiffs::ProcessRequest( class RA_Processor *pRA_Processor )
{
	(( Tribuneprocessor * ) pRA_Processor)->LogActivityTime();

	cout << "r_GetDiffs"<< endl;
	std::ostringstream sSQL;

	string randfile = "client.txt";
	clientfile.open("client.txt");
	
	if (clientfile.is_open()) {

		map<string,string>::iterator iter;
		for( iter = m_mapProgramRecord.begin(); iter != m_mapProgramRecord.end(); iter++ ) {
	
			map<string,string>::iterator it=(g_GlobalConfig.mapPrimaryKey_Timestam_ProgramRecord).find(iter->first);

			if( it == (g_GlobalConfig.mapPrimaryKey_Timestam_ProgramRecord).end() ){

				clientfile << "DELETE FROM ProgramRecord WHERE PK_ProgramRecord='"<<iter->first<<"'\n";	
				clientfile << "DELETE FROM ProgramRecord_Genre WHERE FK_ProgramRecord='"<<iter->first<<"'\n";	
				clientfile << "DELETE FROM Actor_ProgramRecord WHERE FK_ProgramRecord='"<<iter->first<<"'\n";	

			} else if ( ( (iter->second).compare(it->second) ) != 0 ) { 

				if (! UpdateProgramRecord(it->first)){
					return true;
				}
			
				if (! UpdateProgramRecord_Genre(it->first)){
					return true;
				}

				if (! UpdateActor_ProgramRecord(it->first)){
					return true;
				}
				
				
			}
		}

		for( iter = m_mapSchedule.begin(); iter != m_mapSchedule.end(); iter++ ) {
	
			map<string,string>::iterator it=(g_GlobalConfig.mapPrimaryKey_Timestam_Schedule).find(iter->first);

			if( it == (g_GlobalConfig.mapPrimaryKey_Timestam_Schedule).end() ){

				clientfile << "DELETE FROM Schedule WHERE PK_Schedule='"<<iter->first<<"'\n";	

			} else if ( ( (iter->second).compare(it->second) ) != 0 ) {
				
				sSQL << "SELECT * FROM Schedule PK_Schedule=" << it->first;
		
				PlutoSqlResult res;
				MYSQL_ROW row=NULL;
				res.r = g_GlobalConfig.m_pDatabase->mysql_query_result( sSQL.str( ) );
				if( !res.r )
				{
					cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
					m_cProcessOutcome=INTERNAL_ERROR;
				}
				else
				{
					row = mysql_fetch_row( res.r );

					if( !row[0] )
					{
						cerr << "Found NULL in query: " << sSQL.str() << endl;
						m_cProcessOutcome=INTERNAL_ERROR;
						return true; /**<< Request successfully processed */
					}
					clientfile<<"UPDATE Schedule SET FK_Station=\""<<row[1]<<"\", FK_ProgramRecord=\""<<row[2]<<"\", AirDate=\""<<row[3]<<"\", AirTime=\""<<row[4]<<"\", Duration=\""<<row[5]<<"\", PartNum=\""<<row[6]<<"\", NumOfParts=\""<<row[7]<<"\", CCaptioning=\""<<row[8]<<"\", Stereo=\""<<row[9]<<"\", Repeating=\""<<row[10]<<"\", LiveTapeDelay=\""<<row[11]<<"\", Subtitled=\""<<row[12]<<"\", PremiereFinale=\""<<row[13]<<"\", JoinedInProgress=\""<<row[14]<<"\", CableInTheClassroom=\""<<row[15]<<"\", TVRating=\""<<row[16]<<"\", Sap=\""<<row[17]<<"\", Blackout=\""<<row[18]<<"\", SexRating=\""<<row[19]<<"\", ViolenceRating=\""<<row[20]<<"\", LanguageRating=\""<<row[21]<<"\", DialogRating=\""<<row[22]<<"\", FvRating=\""<<row[23]<<"\", Enhanced=\""<<row[24]<<"\", ThreeD=\""<<row[25]<<"\", LetterBox=\""<<row[26]<<"\", Hdtv=\""<<row[27]<<"\", Dolby=\""<<row[28]<<"\", Dvs=\""<<row[29]<<"\", Checksum=\""<<row[30]<<"\" WHERE PK_Schedule=\""<<it->first<<"\"\n";

				}
 
			}
		}

		map<int,string>::iterator iter1;

		for( iter1 = m_mapStation.begin(); iter1 != m_mapStation.end(); iter1++ ) {
	
			map<int,string>::iterator it=(g_GlobalConfig.mapPrimaryKey_Timestam_Station).find(iter1->first);

			if( it == (g_GlobalConfig.mapPrimaryKey_Timestam_Station).end() ){

				clientfile << "DELETE FROM Station WHERE PK_Station='"<<iter1->first<<"'\n";	

			} else if ( ( (iter1->second).compare(it->second) ) != 0 ) {
				
				sSQL << "SELECT * FROM Station PK_Station=" << it->first;
		
				PlutoSqlResult res;
				MYSQL_ROW row=NULL;
				res.r = g_GlobalConfig.m_pDatabase->mysql_query_result( sSQL.str( ) );
				if( !res.r )
				{
					cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
					m_cProcessOutcome=INTERNAL_ERROR;
				}
				else
				{
					row = mysql_fetch_row( res.r );

					if( !row[0] )
					{
						cerr << "Found NULL in query: " << sSQL.str() << endl;
						m_cProcessOutcome=INTERNAL_ERROR;
						return true; /**<< Request successfully processed */
					}
					clientfile<<"UPDATE Station SET TimeZone=\""<<row[1]<<"\", Name=\""<<row[2]<<"\", CallSign=\""<<row[3]<<"\", Affil=\""<<row[4]<<"\", City=\""<<row[5]<<"\", State=\""<<row[6]<<"\", ZipCode=\""<<row[7]<<"\", Country=\""<<row[8]<<"\", DmaName=\""<<row[9]<<"\", DmaNum=\""<<row[10]<<"\", FccChannelNum=\""<<row[11]<<"\", Checksum=\""<<row[12]<<"\" WHERE PK_Station=\""<<it->first<<"\"\n";

				}
 
			}
		}

		sSQL << "SELECT s.* FROM Lineup l LEFT JOIN Headend h ON h.HeadendID=l.HeadendID LEFT JOIN Station s ON s.PK_Station=l.FK_Station WHERE h.PK_Headend=" << m_lineup << "AND s.Name NOT IN (" << m_blacklist << ")";

		PlutoSqlResult res;
		MYSQL_ROW row=NULL;
		res.r = g_GlobalConfig.m_pDatabase->mysql_query_result( sSQL.str( ) );
		if( !res.r )
		{
			cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
			m_cProcessOutcome=INTERNAL_ERROR;
		}
		else
		{
			while ( ( row = mysql_fetch_row( res.r ) ) ){

				if( !row[0] )
				{
					cerr << "Found NULL in query: " << sSQL.str() << endl;
					m_cProcessOutcome=INTERNAL_ERROR;
					return true; /**<< Request successfully processed */
				}

				//insert for Station
				map<int,string>::iterator it=m_mapStation.find(atoi(row[0]));

				if( it == m_mapStation.end() ){
					clientfile<<"INSERT INTO Station(PK_Station, TimeZone, Name, CallSign, Affil, City, State, ZipCode, Country, DmaName, DmaNum, FccChannelNum, Checksum, TimestampF) VALUES (\""<<row[0]<<"\",\""<<row[1]<<"\",\""<<row[2]<<"\",\""<<row[3]<<"\",\""<<row[4]<<"\",\""<<row[5]<<"\",\""<<row[6]<<"\",\""<<row[7]<<"\",\""<<row[8]<<"\",\""<<row[9]<<"\",\""<<row[10]<<"\",\""<<row[11]<<"\",\""<<row[12]<<"\",\""<<row[13]<<"\") \n";
				}

				//insert for Schedule
				if (!InsertSchedule((string)row[0])){
					return true;
				}
				
				//insert for ProgramRecord
				if (!InsertProgramRecord((string)row[0])){
					return true;
				}
// 				"SELECT p.PK_ProgramRecord FROM Station s LEFT JOIN Schedule c ON c.FK_Station=s.PK_Station LEFT JOIN ProgramRecord p ON p.PK_ProgramRecord=c.FK_ProgramRecord WHERE s.PK_Station="<<row[0]
// 
// 				"SELECT p.PK_ProgramRecord FROM ProgramRecord p LEFT JOIN Schedule s ON s.FK_ProgramRecord=p.ProgramRecord LEFT JOIN Station t ON t.PK_Station=s.FK_Station WHERE t.PK_Station=row[0]"

//SELECT PK_ProgramRecord FROM Schedule s LEFT JOIN Station t ON t.PK_Station=s.FK_Station LEFT JOIN ProgramRecord p ON p.PK_ProgramRecord=s.FK_ProgramRecord WHERE t.PK_Station=
				
			}
		}
		m_cProcessOutcome=SUCCESSFULLY_PROCESSED;
	}
	clientfile.close();
	
	m_clientfile = randfile;

	return true; /**<< Request successfully processed */
}
