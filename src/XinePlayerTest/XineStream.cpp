#include "XineStream.h"

void  XineStream::ReadInfo()
{
	BitRate         =  xine_get_stream_info( m_Stream, XINE_STREAM_INFO_BITRATE           );
	SeekAble        =  xine_get_stream_info( m_Stream, XINE_STREAM_INFO_SEEKABLE          );
	VideoWidth      =  xine_get_stream_info( m_Stream, XINE_STREAM_INFO_VIDEO_WIDTH       );
	VideoHeight     =  xine_get_stream_info( m_Stream, XINE_STREAM_INFO_VIDEO_HEIGHT      );
	VideoRatio      =  xine_get_stream_info( m_Stream, XINE_STREAM_INFO_VIDEO_RATIO       );
	VideoChannels   =  xine_get_stream_info( m_Stream, XINE_STREAM_INFO_VIDEO_CHANNELS    );
	VideoStreams    =  xine_get_stream_info( m_Stream, XINE_STREAM_INFO_VIDEO_STREAMS     );
	VideoBitRate    =  xine_get_stream_info( m_Stream, XINE_STREAM_INFO_VIDEO_BITRATE     );
	VideoFOURCC     =  xine_get_stream_info( m_Stream, XINE_STREAM_INFO_VIDEO_FOURCC      );
	VideoHandled    =  xine_get_stream_info( m_Stream, XINE_STREAM_INFO_VIDEO_HANDLED     );
	FrameDuration   =  xine_get_stream_info( m_Stream, XINE_STREAM_INFO_FRAME_DURATION    );
	AudioChanenels  =  xine_get_stream_info( m_Stream, XINE_STREAM_INFO_AUDIO_CHANNELS    );
	AudioBits       =  xine_get_stream_info( m_Stream, XINE_STREAM_INFO_AUDIO_BITS        );
	AudioSampleRate =  xine_get_stream_info( m_Stream, XINE_STREAM_INFO_AUDIO_SAMPLERATE  );
	AudioBitRate    =  xine_get_stream_info( m_Stream, XINE_STREAM_INFO_AUDIO_BITRATE     );
}


void XineStream::ScheduleOsdObject(XineOsdObject *pOsd, int timeOffsetShow, int timeOffsetHide)
{
	if (pOsd==NULL)
		return;
		
	xine_osd_show( *pOsd, timeOffsetShow);
	if (timeOffsetHide!=-1)
		xine_osd_hide(	*pOsd, timeOffsetHide);
}
