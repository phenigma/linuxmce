#include "PlutoVIPrequests.h"
#include "RA/RA_Processor.h"
#include "VR_ShowMenu.h"
#include "VR_RequestSecureTransaction.h"
#include "VR_RequestPayment.h"
#include "VR_PhoneRespondedToRequest.h"
#include "VR_PhoneInstalledBinary.h"
#include "VR_ManuallyMatchPhone.h"
#include "VR_IdentifyPhone.h"

#include "VA_UpdateTransaction.h"
#include "VA_SendFileToPhone.h"
#include "VA_SendMenuToPhone.h"

RA_Request *RA_Processor::BuildRequestFromData(long dwSize, const char *pcData, unsigned long dwRequestID)
{
	RA_Request *pRequest=NULL;
	switch( dwRequestID )
	{
	case VRP_REQUEST_SHOW_MENU:
		pRequest=new VR_ShowMenu();
		pRequest->CreateRequest(dwSize,pcData);
		return pRequest;
	case VRP_REQUEST_SEC_TRANS:
		pRequest=new VR_RequestSecureTransaction();
		pRequest->CreateRequest(dwSize,pcData);
		return pRequest;
	case VRS_REQUEST_PAYMENT:
		pRequest=new VR_RequestPayment();
		pRequest->CreateRequest(dwSize,pcData);
		return pRequest;
	case VRS_PHONE_RESPONDED:
		pRequest=new VR_PhoneRespondedToRequest();
		pRequest->CreateRequest(dwSize,pcData);
		return pRequest;
	case VRS_PHONE_INSTALLED_BIN:
		pRequest=new VR_PhoneInstalledBinary();
		pRequest->CreateRequest(dwSize,pcData);
		return pRequest;
	case VRS_MANUALLY_MATCH_PHONE:
		pRequest=new VR_ManuallyMatchPhone();
		pRequest->CreateRequest(dwSize,pcData);
		return pRequest;
	case VRS_IDENFITY_PHONE:
		pRequest=new VR_IdentifyPhone();
		pRequest->CreateRequest(dwSize,pcData);
		return pRequest;
	};

	return pRequest;
}

RA_Action *RA_Processor::BuildActionFromData( long dwSize, const char *pcData, unsigned long dwActionID)
{
	RA_Action *pRA_Action=NULL;
	switch( dwActionID )
	{
	case ACTION_UPDATETRANSACTION:
		pRA_Action=new VA_UpdateTransaction();
		pRA_Action->CreateAction(dwSize,pcData);
		return pRA_Action;
	case ACTION_SENDFILE_TOPHONE:
		pRA_Action=new VA_SendFileToPhone();
		pRA_Action->CreateAction(dwSize,pcData);
		return pRA_Action;
	case ACTION_SENDMENU_TOPHONE:
		pRA_Action=new VA_SendMenuToPhone();
		pRA_Action->CreateAction(dwSize,pcData);
		return pRA_Action;
	}
	return pRA_Action;
}

RA_Processor *RA_Processor::CreateRA_Processor(class RA_Config *pRA_Config/* = NULL*/)
{
	return new RA_Processor(0, 1, pRA_Config);
}

