/*
 R_UpdatedTable
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */


/**
 *
 * @file R_UpdateTable.cpp
 * @brief source file for the R_UpdateTable class
 *
 */
 
  
#include "R_UpdateTable.h"
#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "CommonFunctions.h"
#include "sqlCVSprocessor.h"
#include "Database.h"

#include <iostream>
#include <sstream>

using namespace sqlCVS;

R_UpdateTable::R_UpdateTable( string sTableName, int psc_batch_last_sync, int psc_id_last_sync, vector<string> *pvectFields )
{
	m_sTableName=sTableName;
	m_pvectFields=pvectFields;
	m_bDeleteVectFields=false;
	m_psc_batch_last_sync=psc_batch_last_sync;
	m_psc_id_last_sync=psc_id_last_sync;
}
/** The server will call this constructor, then ProcessRequest */
R_UpdateTable::R_UpdateTable( )
{
	m_pvectFields = new vector<string>;
	m_bDeleteVectFields=true;
}

R_UpdateTable::~R_UpdateTable( )
{
	if( m_bDeleteVectFields )
		delete m_pvectFields;
}

bool R_UpdateTable::ProcessRequest( class RA_Processor *pRA_Processor )
{
	sqlCVSprocessor *psqlCVSprocessor = ( sqlCVSprocessor * ) pRA_Processor;

	psqlCVSprocessor->m_pTable = g_GlobalConfig.m_pDatabase->m_mapTable_Find( m_sTableName );

	if( !psqlCVSprocessor->m_pTable->Repository_get( ) )
	{
		cerr << "Trying to check in table: " << psqlCVSprocessor->m_pTable->Name_get( ) << " but it's not part of a repository" << endl;
		m_cProcessOutcome=INTERNAL_ERROR;
		return true;
	}

	delete psqlCVSprocessor->m_pvectFields; /** In case there already was one */
	psqlCVSprocessor->m_pvectFields = m_pvectFields;
	m_bDeleteVectFields=false;

	try
	{
		psqlCVSprocessor->m_pTable->GetChanges( this );
	}
	catch( const char *pException )
	{
		cerr << "Cannot send updates to client" << pException << endl;
		m_cProcessOutcome=INTERNAL_ERROR;
		return true;
	}

	m_cProcessOutcome=SUCCESSFULLY_PROCESSED; /** @todo -- process it */

	return true;   /** Request processed successfully */
}
