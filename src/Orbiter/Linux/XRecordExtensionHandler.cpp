
#include "X11/Xproto.h"
#include "DCE/Logger.h"
#include "XRecordExtensionHandler.h"

#define XLockDisplay(pDisplay) \
	g_pPlutoLogger->Write(LV_STATUS, "[0x%x] Locking display here %s: %d", pthread_self(), __FILE__, __LINE__); \
	::XLockDisplay(pDisplay); \
	g_pPlutoLogger->Write(LV_STATUS, "[0x%x] Done locking display here %s: %d", pthread_self(), __FILE__, __LINE__);

#define XUnlockDisplay(pDisplay) \
	g_pPlutoLogger->Write(LV_STATUS, "[0x%x] Unlocking display here %s: %d", pthread_self(), __FILE__, __LINE__); \
	::XUnlockDisplay(pDisplay); \
	g_pPlutoLogger->Write(LV_STATUS, "[0x%x] Done unlocking display here %s: %d", pthread_self(), __FILE__, __LINE__);


using namespace DCE;

XRecordExtensionHandler::XRecordExtensionHandler(Display *pDisplay)
{
	int iMinorVersion, iMajorVersion;

	m_pDisplay = pDisplay;
	m_bCanUseRecord = true;

	XLockDisplay(pDisplay);
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
	XUnlockDisplay(pDisplay);

	m_bShouldRecord = false;
	m_bIsRecording = false;
	pthread_create(&m_processingThread, NULL, recordingThreadMainFunction, this);
}

XRecordExtensionHandler::~XRecordExtensionHandler()
{

}

void *XRecordExtensionHandler::recordingThreadMainFunction(void *arguments)
{
	XRecordExtensionHandler *pXRecordObject = (XRecordExtensionHandler*)arguments;

	while ( true )
	{
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

		g_pPlutoLogger->Write(LV_STATUS, "Sleeping until next time");
		pthread_mutex_lock(&pXRecordObject->m_mutexCondition);
		pthread_cond_wait(&pXRecordObject->m_condition, &pXRecordObject->m_mutexCondition);
		pthread_mutex_unlock(&pXRecordObject->m_mutexCondition);
		g_pPlutoLogger->Write(LV_STATUS, "I'm awake");
	}
}

bool XRecordExtensionHandler::enableRecording(bool bEnable)
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
		g_pPlutoLogger->Write(LV_STATUS, "done disblinmg recording context");
	}
	else if ( ! m_bIsRecording && bEnable )
	{
		m_bShouldRecord = true;
		pthread_cond_signal(&m_condition);
	}

	return true;
}


void XRecordExtensionHandler::XRecordingDataCallback(XPointer pData, XRecordInterceptData *pRecordedData)
{
	XRecordExtensionHandler *pRecordingHandler = (XRecordExtensionHandler*)pData;

	if ( pRecordedData->category == XRecordStartOfData )
	{
		g_pPlutoLogger->Write(LV_STATUS, "Thread 0x%x: Recording started", pthread_self());
		XRecordFreeData(pRecordedData);
		return;
	}

	// XLockDisplay(pRecordingHandler->m_pDisplay);
	g_pPlutoLogger->Write(LV_STATUS, "Processing new event.");
	pRecordingHandler->processXRecordEntry(pRecordedData);
	g_pPlutoLogger->Write(LV_STATUS, "Freeing event data.");
	XRecordFreeData(pRecordedData);
	g_pPlutoLogger->Write(LV_STATUS, "after free.");
	// XUnlockDisplay(pRecordingHandler->m_pDisplay);
}

void XRecordExtensionHandler::processXRecordEntry(XRecordInterceptData *pRecordedData)
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
				case KeyPress:
				case KeyRelease:
					g_pPlutoLogger->Write(LV_STATUS, "Thread 0x%x: Processing key: %d", pthread_self(), pxEvent->u.u.detail);
					break;

				case ButtonPress:
				case ButtonRelease:
					g_pPlutoLogger->Write(LV_STATUS, "Thread 0x%x: Processing button: %d", pthread_self(), pxEvent->u.u.detail );
					break;

				default:
					g_pPlutoLogger->Write(LV_STATUS, "Thread 0x%x: Don't know how to processs event: %d", pthread_self(), pxEvent->u.u.type );
			}
		}
	}
}

pthread_t XRecordExtensionHandler::getRecordingThread()
{
	return m_processingThread;
}
