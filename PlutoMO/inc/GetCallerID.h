#ifndef __GetCallerID_H
#define __GetCallerID_H

#include <e32base.h>
#include <logcli.h>
#include <logview.h>
#include <f32file.h>
#include <logwrap.h>
#include <e32svr.h>
#include <aknnotewrappers.h>

class CPlutoMOAppUi;

class CGetCallerId : public CActive
{
public:
	CLogViewRecent* iRecentLogView;
	RFs iFs;
	CLogClient* iLogClient;
	CPlutoMOAppUi* iAppView;

	CGetCallerId(CPlutoMOAppUi* aAppView);
	~CGetCallerId();
	virtual void RunL();
	virtual void DoCancel();
	void GetLatest(); 
};

#endif