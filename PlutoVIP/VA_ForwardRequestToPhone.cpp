#include "VIPIncludes.h"
#include "VIPShared/VA_ForwardRequestToPhone.h"
#include "VIPShared/VIPMenu.h"
#include "RA/RA_Request.h"
#include "VIPShared/PlutoConfig.h"
#include "VIPShared/VR_PhoneRespondedToRequest.h"
#include "VIPShared/PhoneDevice.h"
#include "VIPShared/VR_IdentifyPhone.h"
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
	m_iOriginalRequestSize = pRequest->RequestSize();
	m_pOriginalRequest = new char[m_iOriginalRequestSize];
	memcpy(m_pOriginalRequest,pRequest->Request(),m_iOriginalRequestSize);

}

VA_ForwardRequestToPhone::VA_ForwardRequestToPhone(unsigned long size,const char *data)
	: RA_Action(size,data)
{
	m_iRequestID = Read_unsigned_long();
	m_iMacAddress=Read_int64();
	m_iReturnRequest = Read_unsigned_char();
	m_iOriginalRequestSize = Read_unsigned_long();
	m_pOriginalRequest = Read_block(m_iOriginalRequestSize);
	m_dwActionSize = (unsigned long) (m_pcCurrentPosition-m_pcDataBlock);
}

void VA_ForwardRequestToPhone::ConvertActionToBinary()
{
	RA_Action::ConvertActionToBinary();
	Write_unsigned_long(m_iRequestID);
	Write_int64(m_iMacAddress);
	Write_unsigned_char(m_iReturnRequest);
	Write_unsigned_long(m_iOriginalRequestSize);
	Write_block(m_pOriginalRequest,m_iOriginalRequestSize);
	m_dwActionSize = (unsigned long) (m_pcCurrentPosition-m_pcDataBlock);
	m_pcAction = m_pcDataBlock;
}

#ifdef VIPESTABLISHMENT

#include "BD/BDCommandProcessor.h"
#include "BD/BDCommandProcessor.h"
#include "BD_CP_SendMeKeystrokes.h"
#include "BD_CP_ShowVMC.h"
#include "VIPShared/VR_ShowMenu.h"
#include "VIPEstablishment/EstablishmentSocket.h"

void *HandleBDCommandProcessorThread(void *p)
{
	BDCommandProcessor *pt = (BDCommandProcessor *)p;
	BDCommand *pCommand = new BD_CP_SendMeKeystrokes(0);
	pt->AddCommand(pCommand);
	while( pt->ReceiveCommand(0,0,NULL) );
	return NULL;
}

void VA_ForwardRequestToPhone::ProcessAction()
{
	if( !m_pcRequest )
		throw "VA_ForwardRequestToPhone needs a pointer to the request that originated the action";

	RA_Request *pRequest = RA_Request::BuildRequestFromData(m_iOriginalRequestSize,m_pOriginalRequest);
	if( pRequest->ID()==VRP_REQUEST_SHOW_MENU )
	{
		// We need to get the customer that the request originated from
		if( m_pcRequest->ID()!=/*VRS_IDENTIFY_PHONE*/ VRS_IDENFITY_PHONE )
			throw "We can't forward a request to a phone unless the original request was an identify phone";

		VR_IdentifyPhone *pRA_Request_Original = (VR_IdentifyPhone *) m_pcRequest;

		// This is the only request I know how to send now
		VR_ShowMenu *pMenu = (VR_ShowMenu *) pRequest;
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
void VA_ForwardRequestToPhone::ProcessAction()
{
}
#endif

