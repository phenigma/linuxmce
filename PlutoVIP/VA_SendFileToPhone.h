#ifndef VA_SendFileToPhone_H
#define VA_SendFileToPhone_H

#include "RA/RA_Action.h"

#if defined(VIPESTABLISHMENT) && !defined(BT_SOCKET)
#include "../VIPShared/BtSendFile.h"
#endif

class VA_SendFileToPhone : public RA_Action
#if defined(VIPESTABLISHMENT) && !defined(BT_SOCKET)
	,CBtSendFileCallback
#endif
{
public:
	void *m_pFile;
	string m_sFileName;
	unsigned long m_iFileSize;
	u_int64_t m_iMacAddress;

	// For an incoming menu
	VA_SendFileToPhone(unsigned long size,const char *data);
	VA_SendFileToPhone(string sFileName,u_int64_t MacAddress);
	~VA_SendFileToPhone();

	unsigned long ID() { return ACTION_SENDFILE_TOPHONE; }
	void ConvertActionToBinary();
	virtual void ProcessAction();

#if defined(VIPESTABLISHMENT) && !defined(BT_SOCKET)
	void OnSendFileEventReceived(  eSendFileCBEvents e_event_code, long param1, long param2);
#endif
};


#endif

