/**
 *
 * @file sqlCVSrequests.cpp
 * @brief source file    @todo ask
 *
 */
 
 /**
  *
  * Copyright information goes here
  *
  */
  
#include "sqlCVSrequests.h"
#include "RA/RA_Processor.h"
#include "R_CommitChanges.h"
#include "R_CommitTable.h"
#include "R_CommitRow.h"
#include "R_GetAll_psc_id.h"
#include "R_UpdateRepository.h"
#include "R_UpdateTable.h"

#include "A_UpdateRow.h"

#include "sqlCVSProcessor.h"


RA_Request *RA_Processor::BuildRequestFromData( long dwSize, const char *pcData, unsigned long dwRequestID )
{
	RA_Request *pRequest=NULL;
	switch( dwRequestID )
	{
	case R_COMMIT_CHANGES:
		pRequest=new R_CommitChanges( );
		pRequest->CreateRequest( dwSize, pcData );
		return pRequest;
	case R_COMMIT_TABLE:
		pRequest=new R_CommitTable( );
		pRequest->CreateRequest( dwSize, pcData );
		return pRequest;
	case R_COMMIT_ROW:
		pRequest=new R_CommitRow( );
		pRequest->CreateRequest( dwSize, pcData );
		return pRequest;
	case R_GET_ALL_PSC_ID:
		pRequest=new R_GetAll_psc_id( );
		pRequest->CreateRequest( dwSize, pcData );
		return pRequest;
	case R_UPDATE_REPOSITORY:
		pRequest=new R_UpdateRepository( );
		pRequest->CreateRequest( dwSize, pcData );
		return pRequest;
	case R_UPDATE_TABLE:
		pRequest=new R_UpdateTable( );
		pRequest->CreateRequest( dwSize, pcData );
		return pRequest;
	};

	return pRequest;
}

RA_Action *RA_Processor::BuildActionFromData( long dwSize, const char *pcData, unsigned long dwActionID )
{
	RA_Action *pRA_Action=NULL;
	switch( dwActionID )
	{
	case A_UPDATE_ROW:
		pRA_Action=new A_UpdateRow( );
		pRA_Action->CreateAction( dwSize, pcData );
		return pRA_Action;
	}
	return pRA_Action;
}

RA_Processor *RA_Processor::CreateRA_Processor( )
{
	return new sqlCVSprocessor( );
}

