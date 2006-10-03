#ifndef __INCOMING_CALL_NOTIFIER_H__
#define __INCOMING_CALL_NOTIFIER_H__
//---------------------------------------------------------------------------------------------------------
#include <Tapi.h>
//---------------------------------------------------------------------------------------------------------
class IncomingCallNotifier
{
public:
	IncomingCallNotifier();
	~IncomingCallNotifier();

private:

	HLINE		m_hLine;
	HLINEAPP	m_hLineApp;
	DWORD		m_dwNumDevs;

	//helper functions
	DWORD InitializeTAPI ();
	DWORD SelectLine(const HLINEAPP hLineApp, const DWORD dwNumberDevices, const DWORD dwAPIVersionLow, 
		const DWORD dwAPIVersionHigh, const TCHAR* const psTSPLineName);
};
//---------------------------------------------------------------------------------------------------------
#endif