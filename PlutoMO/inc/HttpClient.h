#ifndef __HTTP_CLIENT_H__
#define __HTTP_CLIENT_H__

#include <http\rhttpsession.h>
#include <http\mhttptransactioncallback.h>

#include <uri8.h>
#include <e32base.h>
#include <http.h>
#include <chttpformencoder.h>

/** Handles all events for the active transaction. Just displays and logs events that occur.
*/
class CHttpEventHandler : public CBase, public MHTTPTransactionCallback
{
public:
	//
	// methods from MHTTPTransactionCallback
	//
	virtual void MHFRunL(RHTTPTransaction aTransaction, const THTTPEvent& aEvent) 
	{
	switch (aEvent.iStatus)
		{
		case THTTPEvent::EGotResponseHeaders:
			{
			// HTTP response headers have been received. We can determine now if there is
			// going to be a response body to save.
			RHTTPResponse resp = aTransaction.Response();
			TInt status = resp.StatusCode();
			RStringF statusStr = resp.StatusText();
			TBuf<32> statusStr16;
			statusStr16.Copy(statusStr.DesC());

			} break;
		case THTTPEvent::EGotResponseBodyData:
			{
			// Get the body data supplier
			MHTTPDataSupplier* iRespBody = aTransaction.Response().Body();

			iRespBody->ReleaseData();
			} break;
		case THTTPEvent::EResponseComplete:
			{
				int a = 0 ;
			// The transaction's response is complete
			} break;
		case THTTPEvent::ESucceeded:
			{
			aTransaction.Close();
			CActiveScheduler::Stop();
			} break;
		case THTTPEvent::EFailed:
			{
				int a = 0 ;
			//aTransaction.Close();
			//CActiveScheduler::Stop();
			} break;
		case THTTPEvent::ERedirectedPermanently:
			{
			int a = 0 ;
			} break;
		case THTTPEvent::ERedirectedTemporarily:
			{
				int a = 0 ;
			} break;
		default:
			{
				int a = 0 ;
			} break;
		}

	}
	virtual TInt MHFRunError(TInt aError, RHTTPTransaction aTransaction, const THTTPEvent& aEvent) 
	{
		return 0;
	}
};

class PlutoHttpClient : public CBase
{
	RHTTPSession *iSess;

public:
	PlutoHttpClient()
	{
		iSess = new RHTTPSession();

		// create an active scheduler to use
//		CActiveScheduler* scheduler = new(ELeave) CActiveScheduler();
//		CleanupStack::PushL(scheduler);
//		CActiveScheduler::Install(scheduler);
	}

	~PlutoHttpClient()
	{

		delete iSess;
		iSess = NULL;
	}

	void SendRequest(TUriParser8 uri)
	{
		iSess->OpenL();

		RStringPool strP = iSess->StringPool();
		RStringF aMethod;
		aMethod = strP.StringF(HTTP::EGET,RHTTPSession::GetTable());

		CHttpEventHandler* iTransObs = new CHttpEventHandler();
		RHTTPTransaction iTrans = iSess->OpenTransactionL(uri, *iTransObs, aMethod);
		RHTTPHeaders hdr = iTrans.Request().GetHeaderCollection();

		// submit the transaction
		iTrans.SubmitL(); 

		// Start the scheduler, once the transaction completes or is cancelled on an error the scheduler will be
		// stopped in the event handler
		CActiveScheduler::Start();

	}
};

#endif