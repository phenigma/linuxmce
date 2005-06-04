
#include "DCE/Logger.h"
#include "XRecordExtensionHandler.h"

#include "pluto_main/Define_Button.h"

#include <X11/Xutil.h>
#include <X11/Xproto.h>
#include <X11/keysym.h>


using namespace DCE;
using namespace std;

XRecordExtensionHandler::XRecordExtensionHandler(string displayName)
	: m_strDisplayName(displayName), m_isRecordingEnabled(false), m_shouldRecord(false), m_shouldQuit(false)
{
 	pthread_mutex_init(&mutexEnableRecordCondition, NULL);
	pthread_cond_init(&enableRecordCondition, NULL);

	pthread_mutex_init(&mutexStoppedRecordingCondition, NULL);
	pthread_cond_init(&recordingStateChangedCondition, NULL);

    pthread_create(&recordingThread, NULL, recordingThreadMainFunction, this);
}

XRecordExtensionHandler::~XRecordExtensionHandler()
{
	g_pPlutoLogger->Write(LV_STATUS, "Marking as quit");
	m_shouldQuit = true;


	if ( m_isRecordingEnabled )
	{
		g_pPlutoLogger->Write(LV_STATUS, "XRecordExtensionHandler::~XRecordExtensionHandler(): Disabling recording");
		enableRecording(NULL, false); // stop any recording taking place. This will not put the thread of the cond_wait if it is there.
	}

	g_pPlutoLogger->Write(LV_STATUS, "XRecordExtensionHandler::~XRecordExtensionHandler(): Signaling the condition");
	pthread_cond_signal(&enableRecordCondition); // if it is there it will be put out.

	g_pPlutoLogger->Write(LV_STATUS, "XRecordExtensionHandler::~XRecordExtensionHandler(): cancelling the thread");
	pthread_cancel(recordingThread);
	g_pPlutoLogger->Write(LV_STATUS, "XRecordExtensionHandler::~XRecordExtensionHandler(): Joining the thread");
	pthread_join(recordingThread, NULL);
	g_pPlutoLogger->Write(LV_STATUS, "XRecordExtensionHandler::~XRecordExtensionHandler(): Done");
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
		g_pPlutoLogger->Write(LV_WARNING, "XRecordExtensionHandler::recordingThreadMainFunction(): XRecord extension not available.");
		return NULL;
	}

	g_pPlutoLogger->Write(LV_STATUS, "XRecordExtensionHandler::recordingThreadMainFunction(): Available XRecord extension with version %d.%d.", iMajorVersion, iMinorVersion);

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

	pXRecordObject->m_isRecordingEnabled = false;
	pXRecordObject->m_pDisplay = pControlConnection;

	XSync(pXRecordObject->m_pDisplay, True);
	while ( true )
	{
		if ( pXRecordObject->m_shouldRecord && ! pXRecordObject->m_isRecordingEnabled )
		{
			g_pPlutoLogger->Write(LV_STATUS, "XRecordExtensionHandler::recordingThreadMainFunction(): Enabling recording!!!");
			if ( ! XRecordEnableContext(
					pDataConnection,
					pXRecordObject->m_recordingContext,
					(XRecordInterceptProc)&XRecordExtensionHandler::XRecordingDataCallback, (char*)pXRecordObject))
				g_pPlutoLogger->Write(LV_WARNING, "XRecordExtensionHandler::recordingThreadMainFunction(): Could not enable recording context!");

			g_pPlutoLogger->Write(LV_STATUS, "XRecordExtensionHandler::recordingThreadMainFunction(): Recording completed!!!");
		}

        if ( pXRecordObject->m_shouldQuit )
            break;

		g_pPlutoLogger->Write(LV_STATUS, "XRecordExtensionHandler::recordingThreadMainFunction(): Recording thread going to sleep!!!");
        pthread_mutex_lock(&pXRecordObject->mutexEnableRecordCondition);
        while ( ! pXRecordObject->m_shouldQuit && ! pXRecordObject->m_shouldRecord )
			pthread_cond_wait(&pXRecordObject->enableRecordCondition, &pXRecordObject->mutexEnableRecordCondition);
        pthread_mutex_unlock(&pXRecordObject->mutexEnableRecordCondition);
		g_pPlutoLogger->Write(LV_STATUS, "XRecordExtensionHandler::recordingThreadMainFunction(): Recording thread is awake!!!");

        if ( pXRecordObject->m_shouldQuit )
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

	m_shouldRecord = bEnable;

	if (m_shouldRecord == m_isRecordingEnabled )
		return m_isRecordingEnabled;

	g_pPlutoLogger->Write(LV_STATUS, "XRecordExtensionHandler::enableRecording(): Trying to change recording state: CurrentState[:ShouldRecord: %d, :IsRecording: %d]", m_shouldRecord, m_isRecordingEnabled);
	if ( m_isRecordingEnabled  && ! m_shouldRecord )
    {
		g_pPlutoLogger->Write(LV_STATUS, "XRecordExtensionHandler::enableRecording(): Calling XRecord Disable: ShouldRecord: %d, IsRecording: %d", m_shouldRecord, m_isRecordingEnabled);
		XRecordProcessReplies(m_pDisplay);
		XRecordDisableContext(m_pDisplay, m_recordingContext);
		XFlush(m_pDisplay);
		g_pPlutoLogger->Write(LV_STATUS, "XRecordExtensionHandler::enableRecording():  XRecordDisableContext called: ShouldRecord: %d, IsRecording: %d", m_shouldRecord, m_isRecordingEnabled);
	}
	else
	{
		g_pPlutoLogger->Write(LV_STATUS, "XRecordExtensionHandler::enableRecording(): Waiting for state change. for condition: %p", &recordingStateChangedCondition);

		pthread_cond_signal(&enableRecordCondition);

		pthread_mutex_lock(&mutexStoppedRecordingCondition);
		while ( ! m_isRecordingEnabled )
			pthread_cond_wait(&recordingStateChangedCondition, &mutexStoppedRecordingCondition);
    	pthread_mutex_unlock(&mutexStoppedRecordingCondition);

		g_pPlutoLogger->Write(LV_STATUS, "XRecordExtensionHandler::enableRecording(): State changed: CurrentState[:ShouldRecord: %d, :IsRecording: %d].", m_shouldRecord, m_isRecordingEnabled);
	}

	return m_isRecordingEnabled;
}

void XRecordExtensionHandler::XRecordingDataCallback(XPointer pData, XRecordInterceptData *pRecordedData)
{
	XRecordExtensionHandler *pRecordingHandler = (XRecordExtensionHandler*)pData;

	switch ( pRecordedData->category )
	{
		case XRecordStartOfData:
			g_pPlutoLogger->Write(LV_STATUS, "XRecordExtensionHandler::XRecordingDataCallback(): Recording context enabled.");
			pRecordingHandler->m_iMouseX = pRecordingHandler->m_iMouseY = -1;

			pRecordingHandler->m_isRecordingEnabled = true;
			g_pPlutoLogger->Write(LV_STATUS, "XRecordExtensionHandler::XRecordingDataCallback(): Signalling.");
			pthread_cond_signal(&pRecordingHandler->recordingStateChangedCondition);
			break;

		case XRecordEndOfData:
			pRecordingHandler->m_isRecordingEnabled = false;
			g_pPlutoLogger->Write(LV_STATUS, "XRecordExtensionHandler::XRecordingDataCallback(): Recording context got end of data.");
			pthread_cond_signal(&pRecordingHandler->recordingStateChangedCondition);
			break;

		default:
			pRecordingHandler->processXRecordToOrbiterEvent(pRecordedData, &pRecordingHandler->m_OrbiterEvent, pRecordingHandler->m_pDisplay);
			if ( pRecordingHandler->m_pOrbiter )
			{
				Orbiter::Event *pEvent = new Orbiter::Event;
				*pEvent = pRecordingHandler->m_OrbiterEvent;
				pRecordingHandler->m_pOrbiter->CallMaintenanceInMiliseconds(0, &Orbiter::QueueEventForProcessing, pEvent, pe_NO );
			}
	}

	XRecordFreeData(pRecordedData);
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
					g_pPlutoLogger->Write(LV_STATUS, "XRecordExtensionHandler::processXRecordToOrbiterEvent(): Don't know how to processs recorded event: %d", pxEvent->u.u.type );
			}
		}
	}
}

pthread_t XRecordExtensionHandler::getRecordingThread()
{
	return recordingThread;
}
