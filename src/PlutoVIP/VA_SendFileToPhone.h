#ifndef VA_SendFileToPhone_H
#define VA_SendFileToPhone_H

#include "RA/RA_Action.h"
#include "PlutoVIPRequests.h"

#if defined(VIPESTABLISHMENT) && !defined(BT_SOCKET)
#include "VIPShared/BtSendFile.h"
#endif

class VA_SendFileToPhone : public RA_Action
#if defined(VIPESTABLISHMENT) && !defined(BT_SOCKET)
	,CBtSendFileCallback
#endif
{
public:
	PlutoDataBlock m_pdbFile;
	string m_sFileName;
	u_int64_t m_iMacAddress;

	// For an incoming menu
	VA_SendFileToPhone(string sPathName, string sFileName,u_int64_t MacAddress);
	VA_SendFileToPhone() {}

	unsigned long ID() { return ACTION_SENDFILE_TOPHONE; }
	virtual void SetupSerialization(int iSC_Version)
	{
		RA_Action::SetupSerialization(iSC_Version);
		StartSerializeList() + m_pdbFile + m_sFileName
			+ m_iMacAddress;
	}

    virtual void ProcessAction(class RA_Request *pRequest,class RA_Processor *pRA_Processor);

#if defined(VIPESTABLISHMENT) && !defined(BT_SOCKET)
	void OnSendFileEventReceived(  eSendFileCBEvents e_event_code, long param1, long param2);
#endif
};


#endif

