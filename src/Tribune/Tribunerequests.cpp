#include "Tribunerequests.h"
#include "R_GetLineups.h"

#include "Tribuneprocessor.h"
#include "RA/RAServerSocket.h"

RA_Request *RA_Processor::BuildRequestFromData( long dwSize, const char *pcData, unsigned long dwRequestID )
{
	RA_Request *pRequest=NULL;
	switch( dwRequestID )
	{
	case R_GET_LINEUPS:
		{
			pRequest = new R_GetLineups( );
			pRequest->CreateRequest( dwSize, pcData );
		}
	};

	return pRequest;
}

RA_Action *RA_Processor::BuildActionFromData( long dwSize, const char *pcData, unsigned long dwActionID )
{
	RA_Action *pRA_Action=NULL;
// 	switch( dwActionID )
// 	{
// 	case A_UPDATE_ROW:
// 		pRA_Action=new A_UpdateRow( );
// 		pRA_Action->CreateAction( dwSize, pcData );
// 		return pRA_Action;
// 	case A_UPDATE_SCHEMA:
// 		pRA_Action=new A_UpdateSchema( );
// 		pRA_Action->CreateAction( dwSize, pcData );
// 		return pRA_Action;
// 	}
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

