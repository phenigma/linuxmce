#include "R_UpdateRepository.h"
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

R_UpdateRepository::R_UpdateRepository(string sRepository)
{
	m_sRepository=sRepository;
}

bool R_UpdateRepository::ProcessRequest(class RA_Processor *pRA_Processor)
{
	sqlCVSprocessor *psqlCVSprocessor = (sqlCVSprocessor *) pRA_Processor;
	Repository *pRepository = g_GlobalConfig.m_pDatabase->m_mapRepository_Find(m_sRepository);
	if( !pRepository )
	{
		cerr << "Cannot find repository: " << m_sRepository;
		m_cProcessOutcome=INTERNAL_ERROR;
	}
	else
	{
		// TODO - validate users, and store in request processors
		psqlCVSprocessor->m_pRepository = pRepository;
		psqlCVSprocessor->m_i_psc_batch = pRepository->CreateBatch();
		m_cProcessOutcome=SUCCESSFULLY_PROCESSED; // Todo -- process it
	}
	return true;
}
