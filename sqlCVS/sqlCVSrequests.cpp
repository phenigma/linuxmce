#include "sqlCVSrequests.h"
#include "RA/RA_Processor.h"
#include "R_CommitChanges.h"
#include "R_CommitTable.h"
#include "sqlCVSProcessor.h"

RA_Request *RA_Processor::BuildRequestFromData(long dwSize, const char *pcData, unsigned long dwRequestID)
{
	// Todo - put this in a common class
	int iRemainingSize = 0;
	const char *pcPosition = NULL;
	if( dwSize > 0 )
	{
		pcPosition = pcData;
		dwRequestID = *((unsigned long *)pcPosition);
		pcPosition += sizeof(unsigned long);
		iRemainingSize = dwSize-4;
	}

	RA_Request *pRequest=NULL;
	switch( dwRequestID )
	{
	case R_COMMIT_CHANGES:
		return new R_CommitChanges(iRemainingSize,pcPosition);
	case R_COMMIT_TABLE:
		return new R_CommitTable(iRemainingSize,pcPosition);
	};

	return pRequest;
}

RA_Action *RA_Processor::BuildActionFromData( long dwSize, const char *pcData, unsigned long dwActionID)
{
	return NULL;
}

RA_Processor *RA_Processor::CreateRA_Processor()
{
	return new sqlCVSprocessor();
}

