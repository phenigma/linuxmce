
#include "DCE/Logger.h"
#include "XRecordExtensionHandler.h"

#include "pluto_main/Define_Button.h"

#include <X11/Xutil.h>
#include <X11/Xproto.h>
#include <X11/keysym.h>


using namespace DCE;
using namespace std;

XRecordExtensionHandler::XRecordExtensionHandler(string displayName)
	: m_strDisplayName(displayName), m_bShouldRecord(false), m_bShouldQuit(false), m_bIsRecording(false)
{
	pthread_mutex_init(&m_mutexCondition, NULL);
	pthread_cond_init(&m_condition, NULL);

	pthread_create(&m_processingThread, NULL, recordingThreadMainFunction, this);
}

XRecordExtensionHandler::~XRecordExtensionHandler()
{
	g_pPlutoLogger->Write(LV_STATUS, "Marking as quit");
	m_bShouldQuit = true;

	g_pPlutoLogger->Write(LV_STATUS, "Disabling recording");
	enableRecording(NULL, false); // stop any recording taking place. This will not put the thread of the cond_wait if it is there.
	g_pPlutoLogger->Write(LV_STATUS, "Signaling the condition");
	pthread_cond_signal(&m_condition); // if it is there it will be put out.

	g_pPlutoLogger->Write(LV_STATUS, "Joining the theread");
	pthread_join(m_processingThread, NULL);
	g_pPlutoLogger->Write(LV_STATUS, "Done");
}

void *XRecordExtensionHandler::recordingThreadMainFunction(void *arguments)
{
	int 		iMinorVersion, iMajorVersion;
	XRecordExtensionHandler 	*pXRecordObject;
	XRecordRange 				*recordRange;
	XRecordContext 				recordingContext;
    XRecordClientSpec 			recordClient;
	Display 					*pDisplay;

	pXRecordObject = (XRecordExtensionHandler*)arguments;

	pDisplay = XOpenDisplay(getenv(pXRecordObject->m_strDisplayName.c_str()));
	if ( ! XRecordQueryVersion(pDisplay, &iMinorVersion, &iMajorVersion) )
	{
		g_pPlutoLogger->Write(LV_STATUS, "XRecord extension not available.");
		return NULL;
	}

	g_pPlutoLogger->Write(LV_STATUS, "Available XRecord extension with version %d.%d.", iMajorVersion, iMinorVersion);

	recordRange = XRecordAllocRange();

	recordClient = XRecordAllClients;

	recordRange->device_events.first = KeyPress;
    recordRange->device_events.last = ButtonRelease;

    recordRange->core_requests.first = X_NoOperation;
    recordRange->core_requests.last = X_NoOperation;

    recordRange->core_replies.first = X_NoOperation;
    recordRange->core_replies.last = X_NoOperation;

	recordRange->errors.first = Success;
    recordRange->errors.last = Success;

    recordRange->ext_requests.ext_major.first = 0;
	recordRange->ext_requests.ext_major.last = 0;
	recordRange->ext_requests.ext_minor.first = 0;
	recordRange->ext_requests.ext_minor.last = 0;

	recordRange->ext_replies.ext_major.first = 0;
	recordRange->ext_replies.ext_major.last = 0;
    recordRange->ext_replies.ext_minor.first = 0;
    recordRange->ext_replies.ext_minor.last = 0;

	recordingContext = XRecordCreateContext(pDisplay, 0, &recordClient, 1, &recordRange, 1);

	pXRecordObject->m_pDisplay = pDisplay;

	XLockDisplay(pDisplay);
	pXRecordObject->m_bIsRecording = false;
	while ( true )
	{
		if ( pXRecordObject->m_bShouldRecord && ! pXRecordObject->m_bIsRecording )
		{
			pXRecordObject->m_bIsRecording = true;

			g_pPlutoLogger->Write(LV_WARNING, " enabling 1 ");
			if ( XRecordEnableContextAsync( pDisplay, recordingContext, (XRecordInterceptProc)&XRecordExtensionHandler::XRecordingDataCallback, (char*)pXRecordObject) == false)
			{
				g_pPlutoLogger->Write(LV_STATUS, "Could not enable recording context!");
				pXRecordObject->m_bIsRecording = false;
			}
			g_pPlutoLogger->Write(LV_WARNING, " enabled 1 ");
		}

		if ( pXRecordObject->m_bIsRecording )
		{
			g_pPlutoLogger->Write(LV_CRITICAL, "Processing replies");
			XRecordProcessReplies(pDisplay);
			usleep(50000);
			g_pPlutoLogger->Write(LV_CRITICAL, "After processing replies");
		}
		else
		{
			g_pPlutoLogger->Write(LV_CRITICAL, "Sleeping ... ");
			pthread_mutex_lock(&pXRecordObject->m_mutexCondition);
			pthread_cond_wait(&pXRecordObject->m_condition, &pXRecordObject->m_mutexCondition );
			pthread_mutex_unlock(&pXRecordObject->m_mutexCondition);
			g_pPlutoLogger->Write(LV_CRITICAL, "Awake... ");

		}


		if ( pXRecordObject->m_bShouldQuit )
		{
			if ( pXRecordObject->m_bIsRecording )
			{
				g_pPlutoLogger->Write(LV_WARNING, " disabling 1 ");
				XRecordDisableContext( pDisplay, recordingContext );
				pXRecordObject->m_bIsRecording = false;
			}

			return NULL;
		}

		if ( ! pXRecordObject->m_bShouldRecord && pXRecordObject->m_bIsRecording )
		{
			g_pPlutoLogger->Write(LV_WARNING, " disabling 2 ");
			XRecordDisableContext( pDisplay, recordingContext );
			pXRecordObject->m_bIsRecording = false;
		}
	}

	XUnlockDisplay(pDisplay);
	return NULL;
}

bool XRecordExtensionHandler::enableRecording(Orbiter *pRecordingOrbiter, bool bEnable)
{
	m_pOrbiter = pRecordingOrbiter;

	m_bShouldRecord = bEnable;

	g_pPlutoLogger->Write(LV_STATUS, "Signalling record thread: %d", m_bShouldRecord);
	pthread_cond_signal(&m_condition);

	return true;
}


void XRecordExtensionHandler::XRecordingDataCallback(XPointer pData, XRecordInterceptData *pRecordedData)
{
	XRecordExtensionHandler *pRecordingHandler = (XRecordExtensionHandler*)pData;

	g_pPlutoLogger->Write(LV_WARNING, "Got new event 1.");

	if ( pRecordedData->category == XRecordStartOfData )
	{
		g_pPlutoLogger->Write(LV_STATUS, "Recording context enabled.", pthread_self());
		XRecordFreeData(pRecordedData);
		return;
	}

	pRecordingHandler->processXRecordToOrbiterEvent(pRecordedData, &pRecordingHandler->m_OrbiterEvent, pRecordingHandler->m_pDisplay);
	XRecordFreeData(pRecordedData);

	if ( pRecordingHandler->m_pOrbiter )
	{
		Orbiter::Event *pEvent = new Orbiter::Event;
		*pEvent = pRecordingHandler->m_OrbiterEvent;
		pRecordingHandler->m_pOrbiter->CallMaintenanceInMiliseconds(0, &Orbiter::QueueEventForProcessing, pEvent, false );
	}
}

void XRecordExtensionHandler::processXRecordToOrbiterEvent(XRecordInterceptData *pRecordedData, Orbiter::Event *orbiterEvent, Display *pDisplay)
{
	g_pPlutoLogger->Write(LV_WARNING, "Got new event 2.");

	switch (pRecordedData->category )
	{
		case XRecordFromServer:
		case XRecordFromClient:
		{
			xEvent *pxEvent = (xEvent*)pRecordedData->data;

			switch ( pxEvent->u.u.type )
			{
				case KeyPress: case KeyRelease: // key related events types
					orbiterEvent->type = pxEvent->u.u.type == KeyPress ? Orbiter::Event::BUTTON_DOWN : Orbiter::Event::BUTTON_UP;
					orbiterEvent->data.button.m_iPK_Button = pxEvent->u.u.detail;
					// orbiterEvent->data.button.m_iPK_Button = pxEvent->u.u.detail;
					break;

				case ButtonPress:  case ButtonRelease: // mouse button related event types
					orbiterEvent->type = pxEvent->u.u.type == ButtonPress ? Orbiter::Event::REGION_DOWN : Orbiter::Event::REGION_UP;
					orbiterEvent->data.region.m_iX = pxEvent->u.keyButtonPointer.eventX;
					orbiterEvent->data.region.m_iY = pxEvent->u.keyButtonPointer.eventY;
					break;

				default:
					g_pPlutoLogger->Write(LV_STATUS, "Thread 0x%x: Don't know how to processs recorded event: %d", pthread_self(), pxEvent->u.u.type );
			}
		}
	}
}

pthread_t XRecordExtensionHandler::getRecordingThread()
{
	return m_processingThread;
}
