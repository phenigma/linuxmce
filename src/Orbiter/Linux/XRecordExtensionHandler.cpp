
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


	if ( m_bIsRecording )
	{
		g_pPlutoLogger->Write(LV_STATUS, "Disabling recording");
		enableRecording(NULL, false); // stop any recording taking place. This will not put the thread of the cond_wait if it is there.
	}

	g_pPlutoLogger->Write(LV_STATUS, "Signaling the condition");
	pthread_cond_signal(&m_condition); // if it is there it will be put out.

	g_pPlutoLogger->Write(LV_STATUS, "cancelling the thread");
	pthread_cancel(m_processingThread);
	g_pPlutoLogger->Write(LV_STATUS, "Joining the theread");
	pthread_join(m_processingThread, NULL);
	g_pPlutoLogger->Write(LV_STATUS, "Done");
}

void *XRecordExtensionHandler::recordingThreadMainFunction(void *arguments)
{
	int 		iMinorVersion, iMajorVersion;
	XRecordExtensionHandler 	*pXRecordObject;
	XRecordRange 				*recordRange;
    XRecordClientSpec 			recordClient;
	Display 					*pControlConnection, *pDataConnection;

	pXRecordObject = (XRecordExtensionHandler*)arguments;

	pControlConnection = XOpenDisplay(getenv(pXRecordObject->m_strDisplayName.c_str()));
	pDataConnection = XOpenDisplay(getenv(pXRecordObject->m_strDisplayName.c_str()));

	if ( ! XRecordQueryVersion(pControlConnection, &iMinorVersion, &iMajorVersion) )
	{
		g_pPlutoLogger->Write(LV_STATUS, "XRecord extension not available.");
		return NULL;
	}

	g_pPlutoLogger->Write(LV_STATUS, "Available XRecord extension with version %d.%d.", iMajorVersion, iMinorVersion);

	//XSync(pControlConnection, True);
	//XSync(pDataConnection, True);
	recordRange = XRecordAllocRange();

	recordClient = XRecordAllClients;
	recordRange->device_events.first = KeyPress;
	recordRange->device_events.last = MotionNotify;

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

	// pXRecordObject->m_recordingContext = XRecordCreateContext(pDataConnection, XRecordFromServerTime | XRecordFromClientTime | XRecordFromClientSequence, &recordClient, 1, &recordRange, 1);
	pXRecordObject->m_recordingContext = XRecordCreateContext(pDataConnection, 0, &recordClient, 1, &recordRange, 1);

	pXRecordObject->m_bIsRecording = false;
	pXRecordObject->m_pDisplay = pControlConnection;

	while ( true )
	{
		if ( pXRecordObject->m_bShouldRecord && ! pXRecordObject->m_bIsRecording )
		{
			g_pPlutoLogger->Write(LV_CRITICAL, "Enabling recording!!!");
			pXRecordObject->m_bIsRecording = true;
			if ( XRecordEnableContext(pDataConnection, pXRecordObject->m_recordingContext, (XRecordInterceptProc)&XRecordExtensionHandler::XRecordingDataCallback, (char*)pXRecordObject) == False)
				g_pPlutoLogger->Write(LV_WARNING, "Could not enable recording context!");

			g_pPlutoLogger->Write(LV_CRITICAL, "Recording completed!!!");
			pXRecordObject->m_bIsRecording = false;
		}

		if ( pXRecordObject->m_bShouldQuit )
			break;

		pthread_mutex_lock(&pXRecordObject->m_mutexCondition);
		pthread_cond_wait(&pXRecordObject->m_condition, &pXRecordObject->m_mutexCondition );
		pthread_mutex_unlock(&pXRecordObject->m_mutexCondition);

		if ( pXRecordObject->m_bShouldQuit )
			break;
	}

	XRecordFreeContext( pControlConnection, pXRecordObject->m_recordingContext);
	XFree(recordRange);

	XCloseDisplay(pControlConnection);
	XCloseDisplay(pDataConnection);

	return NULL;
}

bool XRecordExtensionHandler::enableRecording(Orbiter *pRecordingOrbiter, bool bEnable)
{
	m_pOrbiter = pRecordingOrbiter;

	m_bShouldRecord = bEnable;

	g_pPlutoLogger->Write(LV_STATUS, "Signalling record thread: %d, current state: %d ", m_bShouldRecord, m_bIsRecording);
	if ( m_bIsRecording  && ! m_bShouldRecord )
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Calling XRecord Disable: %d, current state: %d ", m_bShouldRecord, m_bIsRecording);
		XRecordProcessReplies(m_pDisplay);
		XSync(m_pDisplay, True);
		XRecordDisableContext(m_pDisplay, m_recordingContext);
		g_pPlutoLogger->Write(LV_CRITICAL, "Calling should be Disables: %d, current state: %d ", m_bShouldRecord, m_bIsRecording);
	}

	pthread_cond_signal(&m_condition);

	return true;
}


void XRecordExtensionHandler::XRecordingDataCallback(XPointer pData, XRecordInterceptData *pRecordedData)
{
	XRecordExtensionHandler *pRecordingHandler = (XRecordExtensionHandler*)pData;

	switch ( pRecordedData->category )
	{
		case XRecordStartOfData:
			g_pPlutoLogger->Write(LV_WARNING, "Recording context enabled.", pthread_self());
			pRecordingHandler->m_iMouseX = pRecordingHandler->m_iMouseY = -1;
			break;

		case XRecordEndOfData:
			g_pPlutoLogger->Write(LV_WARNING, "Recording context got end of data.", pthread_self());
			break;

		default:
			if ( pRecordingHandler->m_bShouldRecord )
			{
				pRecordingHandler->processXRecordToOrbiterEvent(pRecordedData, &pRecordingHandler->m_OrbiterEvent, pRecordingHandler->m_pDisplay);
				if ( pRecordingHandler->m_pOrbiter )
				{
					Orbiter::Event *pEvent = new Orbiter::Event;
					*pEvent = pRecordingHandler->m_OrbiterEvent;
					pRecordingHandler->m_pOrbiter->CallMaintenanceInMiliseconds(0, &Orbiter::QueueEventForProcessing, pEvent, false );
				}
			}
	}

	XRecordFreeData(pRecordedData);

	if ( ! pRecordingHandler->m_bShouldRecord )
	{
		XSync(pRecordingHandler->m_pDisplay, True);
		XRecordDisableContext(pRecordingHandler->m_pDisplay, pRecordingHandler->m_recordingContext);
	}
}

void XRecordExtensionHandler::processXRecordToOrbiterEvent(XRecordInterceptData *pRecordedData, Orbiter::Event *orbiterEvent, Display *pDisplay)
{
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
					// g_pPlutoLogger->Write(LV_WARNING, "Key %s with keycode %d", pxEvent->u.u.type == KeyPress ? "down" : "up", pxEvent->u.u.detail);
					orbiterEvent->data.button.m_iPK_Button = pxEvent->u.u.detail;
					// orbiterEvent->data.button.m_iPK_Button = pxEvent->u.u.detail;
					break;

				case MotionNotify:
					orbiterEvent->type = Orbiter::Event::NOT_PROCESSED;
					m_iMouseX = pxEvent->u.keyButtonPointer.rootX;
					m_iMouseY = pxEvent->u.keyButtonPointer.rootY;
					// g_pPlutoLogger->Write(LV_WARNING, "Mouse move at position: %d, %d", m_iMouseX, m_iMouseY);
					break;

				case ButtonPress:  case ButtonRelease: // mouse button related event types
					// g_pPlutoLogger->Write(LV_WARNING, "Button with id: %d is %s", pxEvent->u.u.detail, pxEvent->u.u.type == ButtonPress ? "down" : "up");
					orbiterEvent->type = pxEvent->u.u.type == ButtonPress ? Orbiter::Event::REGION_DOWN : Orbiter::Event::REGION_UP;
					orbiterEvent->data.region.m_iButton = pxEvent->u.u.detail;
					orbiterEvent->data.region.m_iX = m_iMouseX;
					orbiterEvent->data.region.m_iY = m_iMouseY;

					if ( m_iMouseX == -1 && m_iMouseY == -1 )
					 	orbiterEvent->type = Orbiter::Event::NOT_PROCESSED;

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
