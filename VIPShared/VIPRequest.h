#ifndef VIP_Request_h
#define VIP_Request_h

/*
	Implement the mandatory BuildRequestFromData for use with PlutoVIP
*/

// Request Types handled by the server
#define	VRS_IDENFITY_PHONE			1
#define	VRS_REQUEST_PAYMENT			2
#define	VRS_PHONE_RESPONDED			3
#define VRS_MANUALLY_MATCH_PHONE	4
#define VRS_PHONE_INSTALLED_BIN		5

// Request Types handled by the phone
#define	VRP_REQUEST_SHOW_MENU		1001
#define	VRP_REQUEST_SEC_TRANS		1002

// Process outcomes
#define USER_CANNOT_PAY_ONLINE	-100

RA_Request *RA_Processor::BuildRequestFromData(long dwSize, const char *pcData, unsigned long dwRequestID)
{
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

#ifndef VIPPHONE

	case VRS_REQUEST_PAYMENT:
		pRequest = new VR_RequestPayment(iRemainingSize, pcPosition);
		break;
	case VRS_IDENFITY_PHONE:
		pRequest = new VR_IdentifyPhone(iRemainingSize, pcPosition);
		break;
	case VRS_PHONE_RESPONDED:
		pRequest = new VR_PhoneRespondedToRequest(iRemainingSize, pcPosition);
		break;
	case VRP_REQUEST_SEC_TRANS:  // Will still need to build this to get the phone's info
		pRequest = new VR_RequestSecureTransaction(iRemainingSize, pcPosition);
		break;
	case VRP_REQUEST_SHOW_MENU:  // Will still need to build this to get the phone's info
		pRequest = new VR_ShowMenu(iRemainingSize, pcPosition);
		break;

#else

	case VRP_REQUEST_SEC_TRANS:
		pRequest = new VR_RequestSecureTransaction(dwSize-4,pcPosition);
		break;
	case VRP_REQUEST_SHOW_MENU:
		pRequest = new VR_ShowMenu(dwSize-4,pcPosition);
		break;
#endif
	}

	return pRequest;
}

#endif
