#ifndef __NotifyIncomingCall_H
#define __NotifyIncomingCall_H

#include <etel.h>
#include <e32base.h>
#include <logcli.h>
#include <logview.h>
#include <f32file.h>
#include <logwrap.h>
#include <e32svr.h>
#include <aknnotewrappers.h>

class CPlutoMOAppUi;

class CIncomingCallWatcher : public CActive
{
public:
	~CIncomingCallWatcher();
	CIncomingCallWatcher(RLine& aLine, CPlutoMOAppUi *aAppView);
	void ConstructL();

protected:	// from CActive
	void RunL();
	void DoCancel();

private:	// implementation
	void IssueRequestL();

private:
	RLine&					iLine;
	CPlutoMOAppUi			*iAppView;
	TName					iName;
};

#endif