#include "VIPIncludes.h"
#include "BtObex.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBtObex::CBtObex( CBtObexCallback * cbck )
{
	m_Callback = cbck;
}

CBtObex::~CBtObex()
{
//	TRACE("\nDecon CBtObex");
}

COppClient::OPP_RETURN_CODE CBtObex::Push(BD_ADDR bda,WCHAR *psz_path_name,CSdpDiscoveryRec & sdp_rec)
{
	return COppClient::Push(bda, psz_path_name, sdp_rec);
}

void CBtObex::OnProgress(COppClient::OPP_RESULT_CODE result_code, BD_ADDR bda, WCHAR * string, long current, long total)
{
	m_Callback->OnObexEventReceived( CBtObexCallback::OBEX__SEND_PROGRESS, current, total );
}

void CBtObex::OnPushResponse(COppClient::OPP_RESULT_CODE result_code,  BD_ADDR bda, WCHAR * string)
{
//	string s;

	if (result_code != COppClient::COMPLETED)
	{
//		s.Format("File transfer failed.\nError Code %d", result_code);  //Filename: %s, m_p_loc_name,
		m_Callback->OnObexEventReceived( CBtObexCallback::OBEX__SEND_FAILED );
	}
	else
	{
//		s = _T("Transfer complete");
		m_Callback->OnObexEventReceived( CBtObexCallback::OBEX__SEND_SUCCESS );
	
	}
//	TRACE( s );
}

void CBtObex::OnAbortResponse (COppClient::OPP_RESULT_CODE result_code)
{
	m_Callback->OnObexEventReceived( CBtObexCallback::OBEX__SEND_ABORTED );
}
