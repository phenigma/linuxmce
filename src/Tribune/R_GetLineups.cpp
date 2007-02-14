#include "R_GetLineups.h"
#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "Tribuneprocessor.h"
#include "Database.h"
//#include "Repository.h"
#include "CommonFunctions.h"

#include <iostream>
#include <sstream>

using namespace Tribune;

R_GetLineups::R_GetLineups( string sTable, int zipcode)
{
	m_sTable=sTable;
	m_zipcode=zipcode;
}

bool R_GetLineups::ProcessRequest( class RA_Processor *pRA_Processor )
{
	(( Tribuneprocessor * ) pRA_Processor)->LogActivityTime();

	cout << "R_GetLineups" << endl;
	std::ostringstream sSQL;
	// See notes in Table::DetermineDeletions

	sSQL << "SELECT PK_Headend, HeadendName FROM " << m_sTable << " WHERE ZipCode=" << m_zipcode;

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
			m_mapPrimaryKey_LineupName[ row[0] ] = row[1];
		}
		m_cProcessOutcome=SUCCESSFULLY_PROCESSED;
	}

	return true; /**<< Request successfully processed */
}
