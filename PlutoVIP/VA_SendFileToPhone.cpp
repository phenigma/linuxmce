#include "VIPIncludes.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"
#include "../VIPShared/VA_SendFileToPhone.h"
#include "../VIPShared/VIPMenu.h"
#include "../VIPShared/PhoneDevice.h"
#include "RA/RA_Request.h"
#include "../VIPShared/PlutoConfig.h"
#include "../VIPShared/VR_PhoneRespondedToRequest.h"

// todo - is there a cross platform (linux/symbian/windows) way to dynamically allocate a growable memory block?
VA_SendFileToPhone::VA_SendFileToPhone(string sFileName,u_int64_t MacAddress) 
	: RA_Action()
{ 
	m_sFileName=sFileName;
	m_iMacAddress=MacAddress;
	size_t s;
	char *p = FileUtils::ReadFileIntoBuffer(g_pPlutoConfig->m_sMenuPath + m_sFileName.c_str(), s);
	m_iFileSize = (unsigned long) s;
	m_pFile=(void *) p;
}

VA_SendFileToPhone::VA_SendFileToPhone(unsigned long size,const char *data)
	: RA_Action(size,data)
{
	m_iMacAddress=Read_int64();
	Read_string(m_sFileName);
	m_iFileSize = Read_unsigned_long();
	m_pFile = Read_block(m_iFileSize);
}

VA_SendFileToPhone::~VA_SendFileToPhone()
{
	delete m_pFile;
}

void VA_SendFileToPhone::ConvertActionToBinary()
{
	RA_Action::ConvertActionToBinary();
	Write_int64(m_iMacAddress);
	Write_string(m_sFileName);
	Write_unsigned_long(m_iFileSize);
	Write_block((char *)m_pFile,m_iFileSize);
	m_dwActionSize = (unsigned long) (m_pcCurrentPosition-m_pcDataBlock);
	m_pcAction = m_pcDataBlock;
}

#if defined(VIPESTABLISHMENT) && !defined(BT_SOCKET)

pluto_pthread_mutex_t m_FileSendMutex("FileSend");

void VA_SendFileToPhone::ProcessAction()
{
	// Be sure 2 threads don't both try to do this at the same time
	PLUTO_SAFETY_LOCK(fs,m_FileSendMutex);

	// This class will delete itself when it's finished
	CBtSendFile *ptrBtSendFile = new CBtSendFile( this );

	PhoneDevice p("",m_iMacAddress,0);  // just to get a string
	FILE *f = fopen((g_pPlutoConfig->m_sPicturePath + "\\" + m_sFileName).c_str(),"wb");
	if( f )
	{
		fwrite(m_pFile,m_iFileSize,1,f);
		fclose(f);
		ptrBtSendFile->SendFile( g_pPlutoConfig->m_sPicturePath + "\\" + m_sFileName, p.m_sMacAddress, CBtSendFile::BY_OBEX_THEN_NONE );
	}
}


void VA_SendFileToPhone::OnSendFileEventReceived(  eSendFileCBEvents e_event_code, long param1, long param2)
{
	/*
	if ( e_event_code == FILE_SEND_PROGRESS )
	{
		m_lCurrent = param1;
		m_lTotal   = param2;
		PostMessage( WM_SEND_PROGRESS );
	}
	if ( e_event_code == FILE_SEND_COMPLETED )
		PostMessage( WM_SEND_COMPLETED );
	else if ( e_event_code == FILE_SEND_ABORTED )
		PostMessage( WM_SEND_ABORTED );
	else if ( e_event_code == FILE_SEND_FAILED )
		PostMessage( WM_SEND_FAILED );
*/
}

#else
void VA_SendFileToPhone::ProcessAction()
{
}
#endif

