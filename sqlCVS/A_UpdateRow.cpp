#include "A_UpdateRow.h"
#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "R_UpdateTable.h"
#include "sqlCVSprocessor.h"
#include "Table.h"
#include "Database.h"
#include "CommonFunctions.h"

#include <iostream>
#include <sstream>

using namespace sqlCVS;

void A_UpdateRow::ProcessAction(class RA_Request *pRequest,class RA_Processor *pRA_Processor)
{
	if( pRequest->ID()!=R_UPDATE_TABLE )
	{
		cerr << "Got an update row action, but it wasn't in response to an update table request" << endl;
		throw "Internal error";
	}
	R_UpdateTable *pR_UpdateTable = (R_UpdateTable *) pRequest;
	sqlCVSprocessor *psqlCVSprocessor = (sqlCVSprocessor *) pRA_Processor;

	Table *pTable = g_GlobalConfig.m_pDatabase->m_mapTable_Find(pR_UpdateTable->m_sTableName);

	pTable->UpdateRow(this,pR_UpdateTable,psqlCVSprocessor);
}
