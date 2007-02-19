#include "Tribunerequests.h"
#include "R_GetLineups.h"
#include "R_GetChannels.h"
#include "R_GetDiffs.h"

#include "Tribuneprocessor.h"

#include <RA/RAServerSocket.h>
#include <RA/RA_Processor.h>

RA_Request *RA_Processor::BuildRequestFromData( long dwSize, const char *pcData, unsigned long dwRequestID )
{
	RA_Request *pRequest=NULL;
	switch( dwRequestID )
	{
	case R_GET_LINEUPS:
		{
			pRequest = new R_GetLineups( );
			pRequest->CreateRequest( dwSize, pcData );
			break;
		}
	case R_GET_CHANNELS:
		{
			pRequest = new R_GetChannels( );
			pRequest->CreateRequest( dwSize, pcData );
			break;
		}
	case R_GET_DIFFS:
		{
			pRequest = new R_GetDiffs( );
			pRequest->CreateRequest( dwSize, pcData );
			break;
		}
	default:
		cout<<"Wrong ID: "<<dwRequestID<<endl;
	};

	return pRequest;
}

RA_Action *RA_Processor::BuildActionFromData( long dwSize, const char *pcData, unsigned long dwActionID )
{
	RA_Action *pRA_Action=NULL;
	return pRA_Action;
}

RA_Processor *RA_Processor::CreateRA_Processor(class RAServerSocket *pRAServerSocket,class RA_Config *pRA_Config/* = NULL*/)
{
	if( g_pTribuneprocessor )
	{
		cout << "There is still another processor open for " << (int) (time(NULL) - g_pTribuneprocessor->tTime_Creation) <<
			" seconds for IP: " << g_pTribuneprocessor->m_pRAServerSocket->m_sIPAddress << endl;
		
		int iInactivityTime = (int) (time(NULL) - g_pTribuneprocessor->tTime_LastActivity);
		
		cout << "It is inactive " << iInactivityTime << " seconds. ";
		if (iInactivityTime<g_GlobalConfig.m_iServerProcessorTimeout)
		{
			cout << "Remote client has to wait, telling him we are busy" << endl;
			pRAServerSocket->SendString("BUSY_RETRY");
			return NULL;
		}
		else
		{
			cout << "Closing inactive processor, creating new one for the new client" << endl;
			delete g_pTribuneprocessor;
			g_pTribuneprocessor = NULL;
		}
	}
	else
	{
			cout << "Creating new processor for the new client" << endl;
	}

	g_pTribuneprocessor = new Tribuneprocessor( pRAServerSocket );
	printf("Setting g_pTribuneprocessor %p\n",g_pTribuneprocessor);
	return g_pTribuneprocessor;
}

