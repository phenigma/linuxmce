
#ifndef XRECORDEXTENSIONHANDLER_H
#define XRECORDEXTENSIONHANDLER_H

#include <X11/Xlib.h>
#include <X11/extensions/record.h>

#include <pthread.h>

class XRecordExtensionHandler
{
	Display *m_pDisplay;
	bool 				m_bCanUseRecord;
	bool 				m_bShouldRecord;
	bool 				m_bIsRecording;

	pthread_t			m_processingThread;
	pthread_mutex_t		m_mutexCondition;
	pthread_cond_t		m_condition;


	// XRecordFlags m_RecordFlags;
	XRecordRange 		*m_RecordRange;
	XRecordContext 		m_RecordingContext;
    XRecordClientSpec 	m_RecordClient;

private:
	static void *recordingThreadMainFunction(void *arguments);

public:
	XRecordExtensionHandler(Display *display);

	virtual bool enableRecording(bool bEnable = true);

	virtual ~XRecordExtensionHandler();

	static void XRecordingDataCallback(XPointer pData, XRecordInterceptData *pRecordedData);

	void processXRecordEntry(XRecordInterceptData *pRecordedData);

	pthread_t	getRecordingThread();
};

#endif // XRECORDEXTENSIONHANDLER_H
