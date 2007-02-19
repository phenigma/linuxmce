#include "R_GetChannels.h"
#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "Tribuneprocessor.h"
#include "Database.h"
#include "CommonFunctions.h"

#include <iostream>
#include <sstream>

using namespace Tribune;

R_GetChannels::R_GetChannels(int pk_headend, string extra_condition)
{
	m_pk_headend=pk_headend;
	m_extra_condition=extra_condition;
}

bool R_GetChannels::ProcessRequest( class RA_Processor *pRA_Processor )
{
	(( Tribuneprocessor * ) pRA_Processor)->LogActivityTime();

	cout << "r_GetChannels"<< endl;
	std::ostringstream sSQL;

	/** test if there are blacklist channels*/
	if (m_extra_condition.length( )==0){
		sSQL << "SELECT s.PK_Station, s.Name name FROM Lineup l LEFT JOIN Headend h ON h.HeadendID=l.HeadendID LEFT JOIN Station s ON s.PK_Station=l.FK_Station WHERE h.PK_Headend=" << m_pk_headend;
	} else {
		sSQL << "SELECT s.PK_Station, s.Name name FROM Lineup l LEFT JOIN Headend h ON h.HeadendID=l.HeadendID LEFT JOIN Station s ON s.PK_Station=l.FK_Station WHERE h.PK_Headend=" << m_pk_headend << " AND s.Name NOT IN (" << m_extra_condition << ")";

	}

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
		while( ( row = mysql_fetch_row( res.r ) ) )
		{
			if( !row[0] )
			{
				cerr << "Found NULL in query: " << sSQL.str() << endl;
				m_cProcessOutcome=INTERNAL_ERROR;
				return true; /**<< Request successfully processed */
			}
			
			map<string,string>::iterator iter = m_mapPrimaryKey_ChannelName.find(row[0]);
			if (iter == m_mapPrimaryKey_ChannelName.end()){
				m_mapPrimaryKey_ChannelName[ row[0] ] = row[1];
			}
		}
		m_cProcessOutcome=SUCCESSFULLY_PROCESSED;
	}

	return true; /**<< Request successfully processed */
}
