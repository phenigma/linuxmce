#include "NotifyIncomingCall.h"
#include "PlutoMOAppUi.h"

CIncomingCallWatcher::~CIncomingCallWatcher()
{
	Cancel();
}

CIncomingCallWatcher::CIncomingCallWatcher(RLine& aLine, CPlutoMOAppUi *aAppView) 
	: CActive(EPriorityStandard), iLine(aLine), iAppView(aAppView)
{
	CActiveScheduler::Add(this);
}

void CIncomingCallWatcher::ConstructL()
{
	IssueRequestL();
}

void CIncomingCallWatcher::IssueRequestL()
{
	iLine.NotifyIncomingCall(iStatus, iName);
	SetActive();
}

void CIncomingCallWatcher::RunL()
{
	// if successful, notify observer and re-issue request
	if (iStatus.Int() == KErrNone)
		iAppView->NotifyIncomingCall(iName);
	IssueRequestL();
}

void CIncomingCallWatcher::DoCancel()
{
	iLine.NotifyIncomingCallCancel();
}

