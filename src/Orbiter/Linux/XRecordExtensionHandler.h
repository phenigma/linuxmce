
#ifndef XRECORDEXTENSIONHANDLER_H
#define XRECORDEXTENSIONHANDLER_H

#include "../Orbiter.h"

#include <X11/Xlib.h>
#include <X11/extensions/record.h>

#include <pthread.h>

#include <string>

class XRecordExtensionHandler
{
	Display *m_pDisplay;
	bool 				m_bCanUseRecord;
	bool 				m_bShouldRecord;
	bool				m_bShouldQuit;
	bool 				m_bIsRecording;

	pthread_t			m_processingThread;
	pthread_mutex_t		m_mutexCondition;
	pthread_cond_t		m_condition;


	// XRecordFlags m_RecordFlags;
	XRecordRange 		*m_RecordRange;
	XRecordContext 		m_RecordingContext;
    XRecordClientSpec 	m_RecordClient;

	Orbiter 			*m_pOrbiter;
	Orbiter::Event 		m_OrbiterEvent;

private:
	static void *recordingThreadMainFunction(void *arguments);

	static void XRecordingDataCallback(XPointer pData, XRecordInterceptData *pRecordedData);

	void processXRecordToOrbiterEvent(XRecordInterceptData *pRecordedData, Orbiter::Event *orbiterEvent);

public:
	XRecordExtensionHandler(std::string strDisplayName);

	virtual bool enableRecording(Orbiter *processingOrbiter, bool bEnable = true);

	virtual ~XRecordExtensionHandler();

	pthread_t	getRecordingThread();
};

#endif // XRECORDEXTENSIONHANDLER_H
