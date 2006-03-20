#ifndef  XineStream_h
#define  XineStream_h

#include <xine.h>

#include "XineOsdObject.h"

class XineStream
{	
private:
	xine_stream_t *m_pStream;
public:
	int BitRate;           
	int SeekAble;          
	int VideoWidth;       
	int VideoHeight;      
	int VideoRatio;     
	int VideoChannels;    
	int VideoStreams;     
	int VideoBitRate;     
	int VideoFOURCC;      
	int VideoHandled;    
	int FrameDuration;    
	int AudioChanenels;    
	int AudioBits;        
	int AudioSampleRate;  
	int AudioBitRate;     
	
	
	
	XineStream();
	XineStream(xine_stream_t* pStream);
	~XineStream();
	
	void ReadInfo();
	
	operator xine_stream_t*()
	{
		return m_pStream;
	}
	
	void operator =(xine_stream_t *t);
	
	void ScheduleOsdObject(XineOsdObject *pOsd, int timeOffsetShow, int timeOffsetHide=-1);
};


#endif

