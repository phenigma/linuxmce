
#ifndef XRECORDEXTENSIONHANDLER_H
#define XRECORDEXTENSIONHANDLER_H

#include "../Orbiter.h"

#include <X11/Xlib.h>
#include <X11/extensions/record.h>

#include <pthread.h>

#include <string>

class XRecordExtensionHandler
{
	std::string 		m_strDisplayName;

	bool 				m_isRecordingEnabled;
	bool 				m_shouldRecord, m_shouldQuit;

	pthread_t			recordingThread;

	pthread_mutex_t 	mutexEnableRecordCondition;
	pthread_mutex_t 	mutexStoppedRecordingCondition;

    pthread_cond_t  	enableRecordCondition;
	pthread_cond_t  	recordingStateChangedCondition;

	XRecordContext		m_recordingContext;
	// XRecordFlags m_RecordFlags;

	Orbiter 			*m_pOrbiter;
	Orbiter::Event 		m_OrbiterEvent;

	Display 			*m_pDisplay;

	int 				m_iMouseX, m_iMouseY;

private:
	static void *recordingThreadMainFunction(void *arguments);

	static void XRecordingDataCallback(XPointer pData, XRecordInterceptData *pRecordedData);

	void processXRecordToOrbiterEvent(XRecordInterceptData *pRecordedData, Orbiter::Event *orbiterEvent, Display *pDisplay);

public:
	XRecordExtensionHandler(std::string strDisplayName);

	virtual bool enableRecording(Orbiter *processingOrbiter, bool bEnable = true);

	virtual ~XRecordExtensionHandler();

	pthread_t	getRecordingThread();
};

#endif // XRECORDEXTENSIONHANDLER_H
