
#include "X11/Xproto.h"
#include "DCE/Logger.h"
#include "XRecordExtensionHandler.h"

using namespace DCE;
using namespace std;

XRecordExtensionHandler::XRecordExtensionHandler(string displayName)
{
	int iMinorVersion, iMajorVersion;

	m_pDisplay = XOpenDisplay(getenv(displayName.c_str()));
	m_bCanUseRecord = true;

	XLockDisplay(m_pDisplay);
	XSynchronize(m_pDisplay, True);
	if ( ! XRecordQueryVersion(m_pDisplay, &iMinorVersion, &iMajorVersion) )
	{
		g_pPlutoLogger->Write(LV_STATUS, "XRecord extension not available.");
		m_bCanUseRecord = false;
		return;
	}

	pthread_mutex_init(&m_mutexCondition, NULL);
	pthread_cond_init(&m_condition, NULL);

	g_pPlutoLogger->Write(LV_STATUS, "Available XRecord extension with version %d.%d.", iMajorVersion, iMinorVersion);

	m_RecordRange = XRecordAllocRange();

	m_RecordClient = XRecordAllClients;

	m_RecordRange->device_events.first = KeyPress;
    m_RecordRange->device_events.last = ButtonRelease;

//  	m_RecordRange->device_events.first = KeyPress;
//     m_RecordRange->device_events.last = LASTEvent;

		// see X11/XProto.h for defines (we want nothing)
    m_RecordRange->core_requests.first = X_NoOperation;
    m_RecordRange->core_requests.last = X_NoOperation;

    m_RecordRange->core_replies.first = X_NoOperation;
    m_RecordRange->core_replies.last = X_NoOperation;

		// see X11/X.h for defines (we want nothing)
	m_RecordRange->errors.first = Success;
    m_RecordRange->errors.last = Success;

		// no extensions
    m_RecordRange->ext_requests.ext_major.first = 0;
	m_RecordRange->ext_requests.ext_major.last = 0;
	m_RecordRange->ext_requests.ext_minor.first = 0;
	m_RecordRange->ext_requests.ext_minor.last = 0;

	m_RecordRange->ext_replies.ext_major.first = 0;
	m_RecordRange->ext_replies.ext_major.last = 0;
    m_RecordRange->ext_replies.ext_minor.first = 0;
    m_RecordRange->ext_replies.ext_minor.last = 0;

	m_RecordingContext = XRecordCreateContext(m_pDisplay, 0, &m_RecordClient, 1, &m_RecordRange, 1);
	XSynchronize(m_pDisplay, False);
	XUnlockDisplay(m_pDisplay);

	m_bShouldRecord = false;
	m_bShouldQuit = false;
	m_bIsRecording = false;
	pthread_create(&m_processingThread, NULL, recordingThreadMainFunction, this);
}

XRecordExtensionHandler::~XRecordExtensionHandler()
{
	m_bShouldQuit = true;
	enableRecording(NULL, false);
	pthread_join(m_processingThread, NULL);
}

void *XRecordExtensionHandler::recordingThreadMainFunction(void *arguments)
{
	XRecordExtensionHandler *pXRecordObject = (XRecordExtensionHandler*)arguments;

	while ( true )
	{
		if ( pXRecordObject->m_bShouldQuit )
		{
			g_pPlutoLogger->Write(LV_STATUS, "XRecord handler thread quitting\n");
			return NULL;
		}

		if ( pXRecordObject->m_bShouldRecord )
		{
			g_pPlutoLogger->Write(LV_STATUS, "Enabling recording context");
			if ( XRecordEnableContext(
						pXRecordObject->m_pDisplay,
						pXRecordObject->m_RecordingContext,
						(XRecordInterceptProc)&XRecordExtensionHandler::XRecordingDataCallback,
						(char*)pXRecordObject) == false)
				g_pPlutoLogger->Write(LV_STATUS, "Could not enable recording context!");
		}

		if ( pXRecordObject->m_bShouldQuit )
		{
			g_pPlutoLogger->Write(LV_STATUS, "XRecord handler thread quitting\n");
			return NULL;
		}

		pthread_mutex_lock(&pXRecordObject->m_mutexCondition);
		pthread_cond_wait(&pXRecordObject->m_condition, &pXRecordObject->m_mutexCondition);
		pthread_mutex_unlock(&pXRecordObject->m_mutexCondition);
	}

	return NULL;
}

bool XRecordExtensionHandler::enableRecording(Orbiter *pRecordingOrbiter, bool bEnable)
{
	if ( ! m_bCanUseRecord )
		return m_bCanUseRecord;


	if ( m_bIsRecording && !bEnable )
	{
		m_bShouldRecord = false;
		m_bIsRecording = false;
		g_pPlutoLogger->Write(LV_STATUS, "Disabling recording context");
		XLockDisplay(m_pDisplay);
		XRecordDisableContext(m_pDisplay, m_RecordingContext);
		XUnlockDisplay(m_pDisplay);
		g_pPlutoLogger->Write(LV_STATUS, "Done disabling recording context");
		m_pOrbiter = pRecordingOrbiter;
	}
	else if ( ! m_bIsRecording && bEnable )
	{
		m_bShouldRecord = true;
		m_pOrbiter = pRecordingOrbiter;
		pthread_cond_signal(&m_condition);
	}

	return true;
}


void XRecordExtensionHandler::XRecordingDataCallback(XPointer pData, XRecordInterceptData *pRecordedData)
{
	XRecordExtensionHandler *pRecordingHandler = (XRecordExtensionHandler*)pData;

	if ( pRecordedData->category == XRecordStartOfData )
	{
		g_pPlutoLogger->Write(LV_STATUS, "Recording context enabled.", pthread_self());
		XRecordFreeData(pRecordedData);
		return;
	}

	g_pPlutoLogger->Write(LV_STATUS, "Processing new event.");
	pRecordingHandler->processXRecordToOrbiterEvent(pRecordedData, &pRecordingHandler->m_OrbiterEvent);
	XRecordFreeData(pRecordedData);

	if ( pRecordingHandler->m_pOrbiter )
	{
		Orbiter::Event *pEvent = new Orbiter::Event;
		*pEvent = pRecordingHandler->m_OrbiterEvent;
		pRecordingHandler->m_pOrbiter->CallMaintenanceInMiliseconds(0, &Orbiter::QueueEventForProcessing, pEvent, false );
	}
}

void XRecordExtensionHandler::processXRecordToOrbiterEvent(XRecordInterceptData *pRecordedData, Orbiter::Event *orbiterEvent)
{
	g_pPlutoLogger->Write(LV_STATUS, "Got event.");

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
