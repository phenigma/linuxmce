#include "VIPShared/VIPIncludes.h"
#include "VA_ForwardRequestToPhone.h"
#include "VIPShared/VIPMenu.h"
#include "RA/RA_Request.h"
#include "VIPShared/PlutoConfig.h"
#include "VR_PhoneRespondedToRequest.h"
#include "BD/PhoneDevice.h"
#include "VR_IdentifyPhone.h"
#include "PlutoVIP/Customer.h"

#ifdef VIPESTABLISHMENT
#include "VIPEstablishment/CellPhoneEmulator.h"
#include "RA/RA_Processor.h"
#ifndef BT_SOCKET
#include "BD/BDCommandProcessor.h"
#endif
//#include "PlutoVIP/VIPSocket/VIPUIRequest.h"
#endif

// todo - is there a cross platform (linux/symbian/windows) way to dynamically allocate a growable memory block?
VA_ForwardRequestToPhone::VA_ForwardRequestToPhone(unsigned char iReturnRequest, 
												   class RA_Request *pRequest,u_int64_t MacAddress) 
	: RA_Action()
{ 
	m_iRequestID = pRequest->ID();
	m_iReturnRequest = iReturnRequest;
	m_iMacAddress=MacAddress;
	pRequest->ConvertRequestToBinary();
	m_pdbOriginalRequest.m_dwSize = pRequest->RequestSize();
	m_pdbOriginalRequest.m_pBlock = new char[m_pdbOriginalRequest.m_dwSize];
	memcpy(m_pdbOriginalRequest.m_pBlock,pRequest->Request(),m_pdbOriginalRequest.m_dwSize);

}

#ifdef VIPESTABLISHMENT

#include "BD/BDCommandProcessor.h"
#include "BD/BDCommandProcessor.h"
#include "VIPShared/BD_CP_SendMeKeystrokes.h"
#include "VIPShared/BD_CP_ShowVMC.h"
#include "VR_ShowMenu.h"
#include "VIPEstablishment/EstablishmentSocket.h"

void *HandleBDCommandProcessorThread(void *p)
{
	BDCommandProcessor *pt = (BDCommandProcessor *)p;
	BDCommand *pCommand = new BD_CP_SendMeKeystrokes(0);
	pt->AddCommand(pCommand);
	while( pt->ReceiveCommand(0,0,NULL) );
	return NULL;
}

void VA_ForwardRequestToPhone::ProcessAction(class RA_Request *pRequest,class RA_Processor *pRA_Processor)
{
	if( !pRequest )
		throw "VA_ForwardRequestToPhone needs a pointer to the request that originated the action";

    unsigned long *RequestID = (unsigned long *)m_pdbOriginalRequest.m_pBlock;
	RA_Request *pRequest_Original = pRA_Processor->BuildRequestFromData(m_pdbOriginalRequest.m_dwSize,m_pdbOriginalRequest.m_pBlock + sizeof(unsigned long),*RequestID);
	if( pRequest_Original->ID()==VRP_REQUEST_SHOW_MENU )
	{
		// We need to get the customer that the request originated from
		if( pRequest->ID()!=/*VRS_IDENTIFY_PHONE*/ VRS_IDENFITY_PHONE )
			throw "We can't forward a request to a phone unless the original request was an identify phone";

		VR_IdentifyPhone *pRA_Request_Original = (VR_IdentifyPhone *) pRequest;

		// This is the only request I know how to send now
		VR_ShowMenu *pMenu = (VR_ShowMenu *) pRequest_Original;
		pMenu->m_pMenuCollection->ConvertToBinary();
		BD_CP_ShowVMC *pVMC = new BD_CP_ShowVMC(0,pMenu->m_pMenuCollection->m_iBinarySize,pMenu->m_pMenuCollection->m_pBinary);

		BDCommandProcessor *pProcessor = pRA_Request_Original->m_pCustomer->GetCommandProcessor(); // This will create it if it doesn't exist
		if( !pProcessor )
			return; // This could be normal, if the phone went out of range or doesn't support RFCOM

		pProcessor->AddCommand(pVMC);
		pthread_create(&pProcessor->m_BDSockThreadID, NULL, HandleBDCommandProcessorThread, (void*)pProcessor);
	}
}
#else
void VA_ForwardRequestToPhone::ProcessAction(class RA_Request *pRequest,class RA_Processor *pRA_Processor)
{
}
#endif

