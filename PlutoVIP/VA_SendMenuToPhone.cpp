#include "VIPShared/VIPIncludes.h"
#include "VA_SendMenuToPhone.h"
#include "VIPShared/VIPMenu.h"
#include "RA/RA_Request.h"
#include "VIPShared/PlutoConfig.h"
#include "VR_PhoneRespondedToRequest.h"
#include "BD/PhoneDevice.h"
#include "VR_IdentifyPhone.h"
#include "PlutoVIP/Customer.h"
#include "VR_ShowMenu.h"

#ifdef VIPESTABLISHMENT
#include "VIPEstablishment/CellPhoneEmulator.h"
#include "RA/RA_Processor.h"
#ifndef BT_SOCKET
#include "BD/BDCommandProcessor.h"
#endif
//#include "PlutoVIP/VIPSocket/VIPUIRequest.h"
#endif

// todo - is there a cross platform (linux/symbian/windows) way to dynamically allocate a growable memory block?
VA_SendMenuToPhone::VA_SendMenuToPhone(VR_ShowMenu *pVR_ShowMenu, u_int64_t MacAddress) 
	: RA_Action()
{ 
	m_iMacAddress=MacAddress;

	pVR_ShowMenu->ConvertRequestToBinary();
	m_pdbMenu.m_dwSize = pVR_ShowMenu->RequestSize();
	m_pdbMenu.m_pBlock = new char[m_pdbMenu.m_dwSize];
	memcpy(m_pdbMenu.m_pBlock, pVR_ShowMenu->Request(), m_pdbMenu.m_dwSize);

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

void VA_SendMenuToPhone::ProcessAction(class RA_Request *pRequest,class RA_Processor *pRA_Processor)
{
	if( !pRequest )
		throw "VA_SendMenuToPhone needs a pointer to the request that originated the action";

	// We need to get the customer that the request originated from
	if( pRequest->ID()!=/*VRS_IDENTIFY_PHONE*/ VRS_IDENFITY_PHONE )
		throw "We can't forward a request to a phone unless the original request was an identify phone";

	VR_IdentifyPhone *pRA_Request_Original = (VR_IdentifyPhone *) pRequest;

	VR_ShowMenu *pVR_ShowMenu = new VR_ShowMenu();
	pVR_ShowMenu->CreateRequest(m_pdbMenu.m_dwSize, m_pdbMenu.m_pBlock);

	pVR_ShowMenu->m_pMenuCollection->ConvertToBinary();
	BD_CP_ShowVMC *pVMC = new BD_CP_ShowVMC(
		0, 
		pVR_ShowMenu->m_pMenuCollection->m_iBinarySize,
		pVR_ShowMenu->m_pMenuCollection->m_pBinary
	);

	BDCommandProcessor *pProcessor = pRA_Request_Original->m_pCustomer->GetCommandProcessor(); // This will create it if it doesn't exist
	if( !pProcessor )
		return; // This could be normal, if the phone went out of range or doesn't support RFCOM

	pProcessor->AddCommand(pVMC);
	pthread_create(&pProcessor->m_BDSockThreadID, NULL, HandleBDCommandProcessorThread, (void*)pProcessor);
}
#else
void VA_SendMenuToPhone::ProcessAction(class RA_Request *pRequest,class RA_Processor *pRA_Processor)
{
}
#endif

