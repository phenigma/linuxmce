#ifndef  XineStream_h
#define  XineStream_h

#include <xine.h>

class XineStream
{	
public:

	xine_stream_t *m_Stream;
	
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
	
	XineStream(xine_stream_t* pStream)
	{
		m_Stream = pStream;
	}
	
	void ReadInfo();
	
	operator xine_stream_t*()
	{
		return m_Stream;
	}
};


#endif

