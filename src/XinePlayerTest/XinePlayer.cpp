//
// C++ Implementation: XinePlayer
//
// Description: 
//
//
// Author: Radu Leca <radu@radul>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "XinePlayer.h"

	XinePlayer::XinePlayer()
	{
		char configFile[1024];
	
		m_VideoDriverName = "auto";
		m_AudioDriverName = "auto";
		
		m_Xine = xine_new();
		sprintf( configFile, "%s%s", xine_get_homedir(), "/.xine/config");
		xine_config_load( m_Xine, configFile);
		xine_init( m_Xine );
		
		m_nXPos = 0;
		m_nYPos = 0;
		m_nWidth = 640;
		m_nHeight = 480;
		m_dPixelAspect = 1.0;
		
		m_nRunning = 0;
		m_nLengthTime = 0;
		
		m_Display = NULL;
		m_nScreen = 0;
		
		printf( "XinePlayer::XinePlayer\n" );
	}
	
	XinePlayer::~XinePlayer()
	{
		xine_close( (m_Stream) );
		xine_event_dispose_queue( m_EventQueue );
		xine_dispose( m_Stream);
		if( m_AudioPort )
			xine_close_audio_driver( m_Xine, m_AudioPort );  
		xine_close_video_driver(m_Xine, m_VideoPort);  
		xine_exit( m_Xine );
	}
	
	bool XinePlayer::SetOutput(Display *display,Window *window,int nScreen)
	{
		m_Display = display;
		m_Window = *window;
		m_nScreen = nScreen;
		
		m_Visual.display              = display;
		m_Visual.screen               = m_nScreen;
		m_Visual.d                    = m_Window;
		m_Visual.dest_size_cb         = OnResize2;
		m_Visual.frame_output_cb      = FrameOutputCb2;
		m_Visual.user_data            = this;
	
		if( (m_VideoPort = xine_open_video_driver( m_Xine, 
			m_VideoDriverName.c_str(), XINE_VISUAL_TYPE_X11, (void *) &m_Visual)) == NULL )
		{
			printf("I'm unable to initialize '%s' video driver. Giving up.\n", m_VideoDriverName.c_str() );
			return false;
		}
		
		m_AudioPort     = xine_open_audio_driver( m_Xine , m_AudioDriverName.c_str() , NULL);
		m_Stream        = xine_stream_new( m_Xine, m_AudioPort, m_VideoPort);
		m_EventQueue    = xine_event_new_queue( m_Stream );
		xine_event_create_listener_thread( m_EventQueue, XineEvent2, this);
		
		xine_post_t *test;
		const char *const *Info;
		xine_post_in_t *pInInfo;
		int nPos = 0;
		
		test = xine_post_init( m_Xine, "tvtime",0,
			    &m_AudioPort, &m_VideoPort );
		if( test )
		{
			printf( "Succed post_init\n" );
			Info = xine_post_list_inputs( test );
			
			while( Info[nPos] ) { printf("%s\n" , Info[nPos++]); }
			printf( "\n\n" );
			
			Info = xine_post_list_outputs( test );
			
			nPos = 0;
			while( Info[nPos] ) { printf("%s\n" , Info[nPos++]); }
			printf( "\n\n" );
			
			pInInfo  =  xine_post_input( test, "video" );
			pInInfo  =  xine_post_input( test, "parameters" );
			pInInfo  =  xine_post_input( test, "method" );
			pInInfo  =  xine_post_input( test, "enable" );

		}
		else
			printf( "Failed post_init\n");

		xine_port_send_gui_data( m_VideoPort, XINE_GUI_SEND_DRAWABLE_CHANGED, (void *) m_Window);
		xine_port_send_gui_data( m_VideoPort, XINE_GUI_SEND_VIDEOWIN_VISIBLE, (void *) 1);
		
		printf( "Pixel aspect:%f\n", m_dPixelAspect );
		printf( "Screen:%d\n" , m_nScreen );
		printf( "XinePlayer::SetOutput\n" );
		return true;
	}
	
	void XinePlayer::GetInfo()
	{
		string pluginDir, fontDir, homeDir;
		const char* const* pluginList;
		const char* descPlugin;
		
		homeDir = xine_get_homedir();
		printf( "homeDir: %s \n", homeDir.c_str() );
	
		
		pluginList = pluginList = xine_list_post_plugins( m_Xine );
		int nPos = 0;
		printf( "List post plugin:\n" );
		while( pluginList[nPos] )
			printf( "%d: %s \n",nPos, pluginList[nPos++] );
		printf( "\n\n" );
			
		
		pluginList = xine_list_video_output_plugins( m_Xine );
		printf( "List video output plugin:\n" );
		nPos = 0;
		while( pluginList[nPos] )
			printf( "%d: %s \n", nPos, pluginList[nPos++] );
		printf( "\n\n" );
		
		
		pluginList = xine_list_demuxer_plugins( m_Xine );
		printf( "List demuxer plugin:\n" );
		nPos = 0;
		while( pluginList[nPos] )
			printf( "%d: %s \n", nPos, pluginList[nPos++] );
		printf( "\n\n" );
		
		
		descPlugin = xine_get_post_plugin_description( m_Xine, "tvtime" );
		printf( "Description: %s\n" , descPlugin );
			
		char* demux;	
		demux = xine_get_demux_for_mime_type( m_Xine, "tvtime" );
		printf( "Demux: %s\n" , demux );
	
		/*pluginDir = xine_get_plugindir();
		fontDir = xine_get_fontdir();
	
		printf( "Plugin dir:\n", pluginDir );
		printf( "fontDir:\n", fontDir ); */
	}

	bool XinePlayer::Open(char mrl[])
	{
		m_ActualMrl = mrl;
		if( !xine_open( m_Stream, mrl) )
		{
			printf("Unable to open mrl '%s'\n", mrl);
			return false;
		}
		return true;
	
	}
	
	bool XinePlayer::Play()
	{
		if( !xine_play( m_Stream, 0, 0) )
		{
			printf( "Can not play stream\n" );
			return false;
		}
		
		xine_get_pos_length( m_Stream, NULL, NULL, & m_nLengthTime);
	
		m_nRunning = 1;	
		struct timeval firstTime,actualTime;
		//struct timezone timeZone;
		//int nRes , length_time = 0;
		long nLastPos =0 , nActualPos, nDiff;
		int64_t nVirtualFrame = 0, nVirtualFrame2 = 0; 
		gettimeofday(&firstTime, NULL);
		
		while( m_nRunning) 
		{
			XEvent xevent;
			int    got_event;

			XLockDisplay( m_Display );
 			got_event = XPending( m_Display );
			if( got_event )
				XNextEvent( m_Display, &xevent );
			XUnlockDisplay( m_Display);
    
 			gettimeofday(&actualTime, NULL);
 			//printf( "%d %d %d %d " , actualTime.tv_sec, firstTime.tv_sec, 
 			//	actualTime.tv_usec, firstTime.tv_usec);
 			nActualPos = actualTime.tv_sec - firstTime.tv_sec;
 			nActualPos *= 1000;
 			nActualPos += (actualTime.tv_usec - firstTime.tv_usec)/1000;
 			nDiff = nActualPos - nLastPos;
    
 			nVirtualFrame = xine_get_current_vpts( m_Stream );
 			nVirtualFrame2 = nVirtualFrame / 90;
    
			printf( "Actual pos:%ld Last pos:%ld Diff:%ld   " , nActualPos, nLastPos, nDiff );
  			printf( "Virtual frame:%ld  %ld\n" , nVirtualFrame, nVirtualFrame2 );
			
			nLastPos = nActualPos;
	
    
 			if( !got_event ) 
			{
				xine_usec_sleep(20000);
				continue;
			}
			
			switch(xevent.type)
			{
			case Expose:
			if(xevent.xexpose.count != 0)
				break;
			xine_port_send_gui_data( m_VideoPort, XINE_GUI_SEND_EXPOSE_EVENT, &xevent);
			printf( "Expose\n" );
 			break;
		
			case ClientMessage:
				printf( "X Client message \n" );
			break;
		
			case ButtonPress:
			{
				printf( "X Mouse button\n" );
			}
      
			case ConfigureNotify:
			{
			XConfigureEvent *cev = (XConfigureEvent *) &xevent;
			Window           tmp_win;
			m_nWidth  = cev->width;
			m_nHeight = cev->height;
		
			if( (cev->x == 0) && (cev->y == 0) )
			{
				XLockDisplay( m_Display );
				XTranslateCoordinates( m_Display, cev->window,
				DefaultRootWindow(cev->display),
				0, 0, &m_nXPos, &m_nYPos, &tmp_win);
				XUnlockDisplay( m_Display );
			}
			else 
			{
				m_nXPos = cev->x;
				m_nYPos = cev->y;
			}
 			}
      
			printf( "Configure notify %d*%d\n", m_nWidth, m_nHeight );
			break;
 		}
	}
	return true;
}
	
	void XinePlayer::Pause()
	{
		xine_set_param( m_Stream, XINE_PARAM_SPEED, XINE_SPEED_PAUSE  );
	}
	
	void XinePlayer::Resume()
	{
		xine_set_param( m_Stream, XINE_PARAM_SPEED, XINE_SPEED_NORMAL  );
	}
	
	void XinePlayer::JumpAtPosition( int nSecond )
	{
		
	}
	
	void XinePlayer::ChangePlaybackSpeed(int nSpeed )
	{
		
	}
	
	//audio function
	void XinePlayer::SetAudioVolume(int nVolume)	// 0-100
	{
		if( (nVolume) < 0 || (nVolume >100) )
		{
			//log this
			return;
		}
		
		xine_set_param( m_Stream, XINE_PARAM_AUDIO_VOLUME, nVolume );
	}
	
	void XinePlayer::SetAudioMute()
	{
		SetAudioVolume( 0 );
	}
	
	int XinePlayer::GetAudioVolume()
	{
		return xine_get_param( m_Stream, XINE_PARAM_AUDIO_VOLUME );
	}
	
	
	void XinePlayer::XineEvent(void *user_data, const xine_event_t *event)
	{
		printf( "Xine Event:\n" );
	
		switch( event->type )
		{ 
			case XINE_EVENT_UI_PLAYBACK_FINISHED:
			printf( "PLAYBACK_FINISHED \n"  );
			m_nRunning = 0;
			break;
		
			case XINE_EVENT_QUIT:
			printf( "QUIT \n" );
			break;
		
			case XINE_EVENT_PROGRESS:
			printf( "Progress \n" );
			break;
		}
	}
	
	void XinePlayer::OnResize(void *data, int video_width, int video_height, double video_pixel_aspect,
			 int *dest_width, int *dest_height, double *dest_pixel_aspect)
	{
		printf( "OnResize:\n" );	
	
		*dest_width        = m_nWidth;
		*dest_height       = m_nHeight;
 		*dest_pixel_aspect = m_dPixelAspect;
	}
	
	void XinePlayer::FrameOutputCb(void *data, int video_width, int video_height,
			    double video_pixel_aspect, int *dest_x, int *dest_y,
			    int *dest_width, int *dest_height, 
			    double *dest_pixel_aspect, int *win_x, int *win_y)
	{
		printf( "FrameOutputCb\n" );
	
		*dest_x            = 0;
  		*dest_y            = 0;
		*win_x             = m_nXPos;
		*win_y             = m_nYPos;
		*dest_width        = m_nWidth;
		*dest_height       = m_nHeight;
		*dest_pixel_aspect = m_dPixelAspect;
	}
	

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

void XineEvent2(void *user_data, const xine_event_t *event)
{
	XinePlayer *pPlayer = (XinePlayer*) user_data;

	printf( "XineEvent2\n" );
	pPlayer->XineEvent(user_data,event);
}
	
void OnResize2(void *data, int video_width, int video_height, double video_pixel_aspect,
			 int *dest_width, int *dest_height, double *dest_pixel_aspect)
{
	XinePlayer *pPlayer = (XinePlayer*) data;

	printf( "OnResize2 \n" );	
	pPlayer->OnResize(data, video_width, video_height, video_pixel_aspect,
			 dest_width, dest_height, dest_pixel_aspect);
}
	
void FrameOutputCb2(void *data, int video_width, int video_height,
			    double video_pixel_aspect, int *dest_x, int *dest_y,
			    int *dest_width, int *dest_height, 
			    double *dest_pixel_aspect, int *win_x, int *win_y)
{
	XinePlayer *pPlayer = (XinePlayer*) data;

	printf( "FrameOutputCb2 \n" );
	pPlayer->FrameOutputCb(data,video_width,video_height,
			    video_pixel_aspect, dest_x, dest_y,
			    dest_width, dest_height, 
			    dest_pixel_aspect, win_x, win_y);
}	
	