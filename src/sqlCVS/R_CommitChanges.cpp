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

R_CommitChanges::R_CommitChanges( string sRepository, string sDefaultUser, string sComments )
{
	m_sRepository=sRepository;
	m_sDefaultUser=sDefaultUser;
	m_sComments=sComments;
}

bool R_CommitChanges::ProcessRequest( class RA_Processor *pRA_Processor )
{
	cout << "Received Commit changes for repository: " << m_sRepository << endl;
	sqlCVSprocessor *psqlCVSprocessor = ( sqlCVSprocessor * ) pRA_Processor;
	psqlCVSprocessor->m_sComments = m_sComments;
	Repository *pRepository = g_GlobalConfig.m_pDatabase->m_mapRepository_Find( m_sRepository );
	if( !pRepository )
	{
		cerr << "Cannot find repository2: " << m_sRepository;
		m_cProcessOutcome=INTERNAL_ERROR;
	}
	else
	{
		cout << "Ready to validate users.  m_map: " << &m_mapUsersPasswords << endl;
		if( !g_GlobalConfig.ValidateUsers(psqlCVSprocessor->m_bSupervisor,true,&m_mapUsersPasswords) )
		{
			m_cProcessOutcome=LOGIN_FAILED;
			return true;
		}

		// See if any user supplied a password, or if this is a totally anonymous checkin
		psqlCVSprocessor->m_bAllAnonymous = true;
		for(map<int,ValidatedUser *>::iterator it=g_GlobalConfig.m_mapValidatedUsers.begin();it!=g_GlobalConfig.m_mapValidatedUsers.end();++it)
		{
			ValidatedUser *pValidatedUser = (*it).second;
			if( !pValidatedUser->m_bWithoutPassword )
			{
				psqlCVSprocessor->m_bAllAnonymous = false;
				break;
			}
		}

		// Figure out who is the default user that will receive ownership of any new records that
		// don't have an owner specified
		bool bNoPassword,bIsSupervisor; // We're not going to use them.  Only needed to pass them in
		psqlCVSprocessor->m_ipsc_user_default = ValidateUser(m_sDefaultUser,"nopass",bNoPassword,bIsSupervisor);
		if( !psqlCVSprocessor->m_ipsc_user_default && g_GlobalConfig.m_mapValidatedUsers.size() )
			psqlCVSprocessor->m_ipsc_user_default = (*g_GlobalConfig.m_mapValidatedUsers.begin()).first;

		psqlCVSprocessor->m_pRepository = pRepository;
		m_psc_batch = psqlCVSprocessor->m_psc_bathdr_orig = psqlCVSprocessor->m_i_psc_batch = pRepository->CreateBatch( psqlCVSprocessor, &g_GlobalConfig.m_mapValidatedUsers );

		if( !psqlCVSprocessor->m_i_psc_batch )
			m_cProcessOutcome=INTERNAL_ERROR;
		else
			m_cProcessOutcome=SUCCESSFULLY_PROCESSED; /** @todo -- process it */
	}
	return true;
}

