/*
 R_CommitChanges
 
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
 * @file R_CommitChanges.cpp
 * @brief source file for the R_CommitChanges class
 *
 */
 
#include "R_CommitChanges.h"
#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "sqlCVSprocessor.h"
#include "Database.h"
#include "Repository.h"
#include "CommonFunctions.h"

#include <iostream>
#include <sstream>

using namespace sqlCVS;

R_CommitChanges::R_CommitChanges( string sRepository, string sDefaultUser )
{
	m_sRepository=sRepository;
	m_sDefaultUser=sDefaultUser;
}

bool R_CommitChanges::ProcessRequest( class RA_Processor *pRA_Processor )
{
	cout << "Received Commit changes for repository: " << m_sRepository << endl;
	sqlCVSprocessor *psqlCVSprocessor = ( sqlCVSprocessor * ) pRA_Processor;
	Repository *pRepository = g_GlobalConfig.m_pDatabase->m_mapRepository_Find( m_sRepository );
	if( !pRepository )
	{
		cerr << "Cannot find repository2: " << m_sRepository;
		m_cProcessOutcome=INTERNAL_ERROR;
	}
	else
	{
		if( !g_GlobalConfig.ValidateUsers(psqlCVSprocessor->m_bSupervisor,true,&m_mapUsersPasswords) )
		{
			m_cProcessOutcome=LOGIN_FAILED;
			return true;
		}

		psqlCVSprocessor->m_pRepository = pRepository;
		m_psc_batch = psqlCVSprocessor->m_psc_bathdr_orig = psqlCVSprocessor->m_i_psc_batch = pRepository->CreateBatch( &g_GlobalConfig.m_mapValidatedUsers );

		if( !psqlCVSprocessor->m_i_psc_batch )
			m_cProcessOutcome=INTERNAL_ERROR;
		else
			m_cProcessOutcome=SUCCESSFULLY_PROCESSED; /** @todo -- process it */
	}
	return true;
}

