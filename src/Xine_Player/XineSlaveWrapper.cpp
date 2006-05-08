/*
XineSlaveWrapper

Copyright (C) 2004 Pluto, Inc., a Florida Corporation

www.plutohome.com

Phone: +1 (877) 758-8648

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.
*/

/**
 *
 * @file XineSlaveWrapper.cpp
 * @brief source file for XineSlaveWrapper class
 *
 * C++ Implementation: Xine Player DCE Plugin (the Xine lib slave controller)
 *
 */
#include "DCE/Logger.h"
#include "XineSlaveWrapper.h"
#include "Xine_Player.h"

#include <math.h>

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "DCE/DCEConfig.h"

#include "pluto_main/Define_Button.h"

#include "JpegEncoderDecoder.h"

#define INPUT_MOTION (ExposureMask | ButtonPressMask | KeyPressMask | \
                      ButtonMotionMask | StructureNotifyMask | PropertyChangeMask | PointerMotionMask)

#define POINTER_HIDE_SECONDS 2
static XineSlaveWrapper::Dynamic_Pointer *g_pDynamic_Pointer = NULL;

/** Define this here to avoid some dependency. */
typedef struct
{
    uint32_t flags;
    uint32_t functions;
    uint32_t decorations;
    int32_t input_mode;
    uint32_t status;
}
MWMHints;

// for original xine library - comment this when using a patched one
#define xine_seek(A, B, C) xine_play(A, B, C)

// Since xine can't handle speeds other than 1x,2x,4x, we'll create our own
// thread to handle special seek speeds by doing lots of searches
// If g_iSpecialSeekSpeed==0, that means we don't need a special speed--thread will sleep
// It can be set to a value such as 32000 (for 32x) or -250 (for -1/4x)
// The event processing thread will handle it
int g_iSpecialSeekSpeed = 0;

XineSlaveWrapper::XineSlaveWrapper( int iTimeCodeReportFrequency )
        : m_sWindowTitle( "pluto-xine-playback-window" ),
        XServerDisplay( NULL ),
        m_pSameStream( NULL ),
        m_pDynamic_Pointer( NULL )
{
    m_iSpecialOneTimeSeek = 0;
    m_xine_osd_t = NULL;
    m_iPrebuffer = 0;
    m_iCurrentWindow = 0;
    m_iTimeCodeReportFrequency = iTimeCodeReportFrequency;

    m_sXineAudioDriverName = "alsa";
    m_sXineVideoDriverName = "xv";

    DCEConfig * pConfig = new DCEConfig();
    m_sConfigFile = pConfig->ReadString( "XinePlayerConfigurationFile", "/etc/pluto/xine.conf" );
    delete pConfig;

    m_pXineVisualizationPlugin = NULL;
    m_pDeinterlacePlugin = NULL;
    m_bExitThread = false;
}

XineSlaveWrapper::~XineSlaveWrapper()
{
    stopMedia( 1 ); // any number will do.
    
    // disconnecting from xine lib
    if (m_pXine)
    {
        xine_exit(m_pXine);
        m_pXine = NULL;
    }
    
    closeWindow();
}

static const char noCursorDataDescription[] =
    {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };


XineSlaveWrapper::XineStream::XineStream():m_xineStreamMutex("xine-stream-access-mutex")
{
	pthread_mutexattr_t      mutexAttr;
  pthread_mutexattr_init( &mutexAttr );
  pthread_mutexattr_settype( &mutexAttr,  PTHREAD_MUTEX_RECURSIVE_NP );
	m_xineStreamMutex.Init( &mutexAttr );
}


/**
 *   \fn bool XineSlaveWrapper::createWindow()
 */
bool XineSlaveWrapper::createWindow()
{
    XColor black; //, dummy;
    XSizeHints sizeHints;
    MWMHints wmHints;
    XClassHint classHint;

    long propertyValues[ 1 ];
    int completionEvent;
    int xpos, ypos, width, height;

    double res_h, res_v;

    if ( !XInitThreads() )
    {
        g_pPlutoLogger->Write( LV_WARNING, "XInitThreads() failed!" );
        return false;
    }

    if ( ( XServerDisplay = XOpenDisplay( getenv( "DISPLAY" ) ) ) == NULL )
    {
        g_pPlutoLogger->Write( LV_WARNING, "Could not open X DISPLAY from: %s", getenv( "DISPLAY" ) );
        return false;
    }

    XLockDisplay( XServerDisplay );

    m_iCurrentScreen = XDefaultScreen( XServerDisplay );
    xpos = 10;
    ypos = 20;
    width = 720;
    height = 540;

    windows[ 0 ] = XCreateSimpleWindow( XServerDisplay, XDefaultRootWindow( XServerDisplay ),
                                        xpos, ypos, width, height,
                                        1, 0, 0 );
    windows[ 1 ] = XCreateSimpleWindow( XServerDisplay, XDefaultRootWindow( XServerDisplay ),
                                        0, 0, ( DisplayWidth( XServerDisplay, m_iCurrentScreen ) ), DisplayHeight( XServerDisplay, m_iCurrentScreen ),
                                        0, 0, 0 );

    classHint.res_name = ( char* ) m_sWindowTitle.c_str();
    classHint.res_class = ( char* ) m_sWindowTitle.c_str();
    XSetClassHint ( XServerDisplay, windows[ 0 ], &classHint );

    classHint.res_class = ( char* ) m_sWindowTitle.c_str();
    XSetClassHint ( XServerDisplay, windows[ 0 ], &classHint );

    XSelectInput( XServerDisplay, windows[ 0 ], INPUT_MOTION );
    XSelectInput( XServerDisplay, windows[ 1 ], INPUT_MOTION );

    XSetStandardProperties( XServerDisplay, windows[ 0 ], m_sWindowTitle.c_str(), m_sWindowTitle.c_str(), None, NULL, 0, 0 );
    XSetStandardProperties( XServerDisplay, windows[ 1 ], m_sWindowTitle.c_str(), m_sWindowTitle.c_str(), None, NULL, 0, 0 );

    sizeHints.win_gravity = StaticGravity;
    sizeHints.flags = PPosition | PSize | PWinGravity;

    XSetWMNormalHints( XServerDisplay, windows[ 0 ], &sizeHints );
    XSetWMNormalHints( XServerDisplay, windows[ 1 ], &sizeHints );

    Atom XA_DELETE_WINDOW = XInternAtom( XServerDisplay, "WM_DELETE_WINDOW", False );
    XSetWMProtocols( XServerDisplay, windows[ 0 ], &XA_DELETE_WINDOW, 1 );
    XSetWMProtocols( XServerDisplay, windows[ 1 ], &XA_DELETE_WINDOW, 1 );

    Atom XA_NO_BORDER = XInternAtom( XServerDisplay, "_MOTIF_WM_HINTS", False );
    wmHints.flags = ( 1L << 1 ); // MWM_HINTS_DECORATIONS
    wmHints.decorations = 0;

    XChangeProperty( XServerDisplay, windows[ 1 ], XA_NO_BORDER, XA_NO_BORDER, 32,
                     PropModeReplace, ( unsigned char * ) & wmHints,
                     5 /* PROP_MWM_HINTS_ELEMENTS */ );

    Atom XA_WIN_LAYER = XInternAtom( XServerDisplay, "_NET_WM_STATE", False );
    propertyValues[ 0 ] = 10;
    XChangeProperty( XServerDisplay, windows[ 1 ], XA_WIN_LAYER, XA_CARDINAL, 32, PropModeReplace, ( unsigned char * ) propertyValues, 1 );


    XA_WIN_LAYER = XInternAtom( XServerDisplay, "_NET_WM_STATE", False );
    propertyValues[ 0 ] = 12;
    XChangeProperty( XServerDisplay, windows[ 1 ], XA_WIN_LAYER, XA_CARDINAL, 32, PropModeReplace, ( unsigned char * ) propertyValues, 1 );

    if ( XShmQueryExtension( XServerDisplay ) == True )
        completionEvent = XShmGetEventBase( XServerDisplay ) + ShmCompletion;
    else
        completionEvent = -1;

    noCursor = XCreateBitmapFromData( XServerDisplay, ( DefaultRootWindow( XServerDisplay ) ), noCursorDataDescription, 8, 8 );
    cursors[0] = XCreateFontCursor(XServerDisplay, XC_left_ptr);
    cursors[1] = XCreatePixmapCursor(XServerDisplay, noCursor, noCursor, &black, &black, 0, 0);
    if ( m_pDynamic_Pointer )
        delete m_pDynamic_Pointer;
    m_pDynamic_Pointer = new Dynamic_Pointer(this, &cursors[0], &cursors[1]);

    XFreePixmap( XServerDisplay, noCursor );

    XDefineCursor( XServerDisplay, windows[ m_iCurrentWindow ], cursors[ m_iCurrentWindow ] );
    XMapRaised( XServerDisplay, windows[ m_iCurrentWindow ] );

    if ( m_pDynamic_Pointer )
        m_pDynamic_Pointer->pointer_hide();

    res_h = ( DisplayWidth( XServerDisplay, m_iCurrentScreen ) * 1000 / DisplayWidthMM( XServerDisplay, m_iCurrentScreen ) );
    res_v = ( DisplayHeight( XServerDisplay, m_iCurrentScreen ) * 1000 / DisplayHeightMM( XServerDisplay, m_iCurrentScreen ) );

    m_dPixelAspect = res_v / res_h;

    g_pPlutoLogger->Write( LV_STATUS, "XServer aspect %f", m_dPixelAspect );

    if ( fabs( m_dPixelAspect - 1.0 ) < 0.01 )
        m_dPixelAspect = 1.0;

    XSync( XServerDisplay, True );
    XUnlockDisplay( XServerDisplay );

    return true;
}


/**
 *
 *  \fn XineSlaveWrapper::createStream(string fileName, int streamID, int mediaPosition, int iRequestingObject);
 */
bool XineSlaveWrapper::createStream( string fileName, int streamID, int iRequestingObject )
{
    if ( m_xine_osd_t )
    {
        // Freeing causes a seg fault.  I guess that means xinelib automatically frees it when the stream changes???
        // xine_osd_free(m_xine_osd_t);
        m_xine_osd_t = NULL;
    }


    g_pPlutoLogger->Write( LV_STATUS, "Got a create stream command command for %s", fileName.c_str() );

    bool isNewStream = false;

    XineStream *xineStream = getStreamForId( streamID, "" ); /** @warning HACK: This should be changed to behave properly */

    g_pPlutoLogger->Write( LV_STATUS, "Testing to see if we can reuse the stream." );

    if ( xineStream == NULL )
    {
        g_pPlutoLogger->Write( LV_STATUS, "Nope .. making a new one" );
        isNewStream = true;
        xineStream = new XineStream();
        
        PLUTO_SAFETY_LOCK( streamLock, xineStream->m_xineStreamMutex );
        xineStream->m_pOwner = this;
        xineStream->m_pStream = NULL;
        xineStream->m_iStreamID = streamID;
        setStreamForId( streamID, xineStream );
    }
    else
    {
    		PLUTO_SAFETY_LOCK( streamLock, xineStream->m_xineStreamMutex );
        g_pPlutoLogger->Write( LV_STATUS, "Closing old stream" );
        if ( m_pXineVisualizationPlugin )
        {
            xine_post_wire_audio_port( xine_get_audio_source( xineStream->m_pStream ), m_pXineAudio );
            xine_post_dispose( m_pXine, m_pXineVisualizationPlugin );
            m_pXineVisualizationPlugin = NULL;
        }
		if (m_pDeinterlacePlugin)
		{
			xine_post_wire_video_port( xine_get_video_source( xineStream->m_pStream ), m_pXineVideo );
				xine_post_dispose( m_pXine, m_pDeinterlacePlugin );
				m_pDeinterlacePlugin = NULL; 		
		}

        // this can be null if we weren't able to open any ports in a previous atempt
        if ( xineStream->m_pStream )
		{
	        if ( g_iSpecialSeekSpeed || xineStream->m_pOwner->m_iSpecialOneTimeSeek )
			{
				g_iSpecialSeekSpeed=0;
				xineStream->m_pOwner->m_iSpecialOneTimeSeek=0;
		        usleep( 300000 );  // Wait to be sure we're not in the middle of a special seek
			}
            xine_close( xineStream->m_pStream );
		}

        g_pPlutoLogger->Write( LV_STATUS, "Closed!" );

        xineStream->m_bIsRendering = false;
        xineStream->m_iStreamID = streamID;
    }

		PLUTO_SAFETY_LOCK( streamLock, xineStream->m_xineStreamMutex );
    xineStream->m_bIsVDR = fileName.substr( 0, 4 ) == "vdr:";

    m_x11Visual.display = XServerDisplay;
    m_x11Visual.screen = m_iCurrentScreen;
    m_x11Visual.d = windows[ m_iCurrentWindow ];
    m_x11Visual.dest_size_cb = &destinationSizeCallback;
    m_x11Visual.frame_output_cb = &frameOutputCallback;
    m_x11Visual.user_data = xineStream;

    g_pPlutoLogger->Write( LV_STATUS, "Opening Video Driver" );
    if ( isNewStream && ( m_pXineVideo = xine_open_video_driver( m_pXine, m_sXineVideoDriverName.c_str(), XINE_VISUAL_TYPE_X11, ( void * ) & m_x11Visual ) ) == NULL )
    {
        g_pPlutoLogger->Write( LV_WARNING, "I'm unable to initialize m_pXine's '%s' video driver. Falling to 'xshm'.", m_sXineVideoDriverName.c_str() );
        if ( ( m_pXineVideo = xine_open_video_driver( m_pXine, "xshm", XINE_VISUAL_TYPE_X11, ( void * ) & m_x11Visual ) ) == NULL )
        {
            g_pPlutoLogger->Write( LV_WARNING, "I'm unable to initialize m_pXine's 'xshm' video driver. Giving up." );
            // TODO: obvious memory leak here
            setStreamForId( streamID, NULL );
            return false;
        }
    }

    g_pPlutoLogger->Write( LV_STATUS, "Opening Audio Driver" );
    if ( isNewStream && ( m_pXineAudio = xine_open_audio_driver( m_pXine, m_sXineAudioDriverName.c_str(), NULL ) ) == NULL )
    {
        g_pPlutoLogger->Write( LV_WARNING, "I'm unable to initialize m_pXine's '%s' audio driver.", m_sXineAudioDriverName.c_str() );
        // TODO: obvious memory leak here
        setStreamForId( streamID, NULL );
        return false;
    }

    g_pPlutoLogger->Write( LV_STATUS, "Calling xine_stream_new" );
    if ( isNewStream && ( xineStream->m_pStream = xine_stream_new( m_pXine, m_pXineAudio, m_pXineVideo ) ) == NULL )
    {
        g_pPlutoLogger->Write( LV_WARNING, "Could not create stream (wanted to play: %s)", fileName.c_str() );
        setStreamForId( streamID, NULL );
        // TODO: obvious memory leak here
        delete xineStream;
        return false;
    }

    xineStream->m_iRequestingObject = iRequestingObject;

//	old code
//    if ( isNewStream )
//        m_mapStreams[ streamID ] = xineStream; /** @todo: check for leaks (if someone will send me multiple playStream with the same Stream ID it will leak memory) */

    if ( isNewStream )
    {
        g_pPlutoLogger->Write( LV_STATUS, "xine_event_new_queue" );
        xineStream->m_pStreamEventQueue = xine_event_new_queue( xineStream->m_pStream );
        xine_event_create_listener_thread( xineStream->m_pStreamEventQueue, xineEventListener, xineStream );
    }

    xine_port_send_gui_data( m_pXineVideo, XINE_GUI_SEND_VIDEOWIN_VISIBLE, ( void * ) 0 );
    xine_port_send_gui_data( m_pXineVideo, XINE_GUI_SEND_DRAWABLE_CHANGED, ( void * ) windows[ m_iCurrentWindow ] );
    xine_port_send_gui_data( m_pXineVideo, XINE_GUI_SEND_VIDEOWIN_VISIBLE, ( void * ) 1 );

    setXineStreamDebugging( streamID, true );

    g_pPlutoLogger->Write( LV_STATUS, "Ready to call xine_open" );
    if ( xine_open( xineStream->m_pStream, fileName.c_str() ) )
    {
        g_pPlutoLogger->Write( LV_STATUS, "Opened..... " );

        setXineStreamDebugging( streamID, false );
        xineStream->m_bHasVideo = xine_get_stream_info( xineStream->m_pStream, XINE_STREAM_INFO_HAS_VIDEO );

        if ( xineStream->m_iImgWidth == 0 )
            xineStream->m_iImgWidth++;

        if ( xineStream->m_iImgHeight == 0 )
            xineStream->m_iImgHeight++;

        if ( xineStream->m_bHasVideo )
        {
		if (!m_pDeinterlacePlugin)
		{
			xine_video_port_t **volist;
			volist = (xine_video_port_t **)malloc(2 * sizeof(xine_video_port_t*));
			volist[0] = m_pXineVideo;
			volist[1] = NULL;

			m_pDeinterlacePlugin = xine_post_init(m_pXine, "tvtime", 0, NULL, volist);
		}		
		if (m_pDeinterlacePlugin)
		{		
			xine_post_in_t *input_api;
			xine_post_api_t *api;
			xine_post_api_descr_t *desc;
			xine_post_api_parameter_t *parm;
			char *data;
			
			input_api = (xine_post_in_t *)xine_post_input(m_pDeinterlacePlugin, "parameters");
			if (!input_api) {
			perror("Failed to setup post plugin: ");
			return -1;
			}
			
			api = (xine_post_api_t *)input_api->data;
			desc = api->get_param_descr();
			parm = desc->parameter;
			data = (char *)malloc(desc->struct_size);
			api->get_parameters(m_pDeinterlacePlugin, (void *)data);
			
			while (parm->type != POST_PARAM_TYPE_LAST)
			{
			g_pPlutoLogger->Write(LV_STATUS,"parm: %s, %s\n", parm->name, parm->description);
			int i = 0;
			if(parm->enum_values != NULL)
			    while(parm->enum_values[i] != NULL)
			    {
				g_pPlutoLogger->Write(LV_STATUS,"parm[%d]: %s\n", i, parm->enum_values[i]);
				i++;
			    }
			if(!strncasecmp(parm->name, "method", 6) &&
			    parm->type == POST_PARAM_TYPE_INT)
			{
			    *(int *)(data + parm->offset) = 4;
			}
			else if(!strncasecmp(parm->name, "enabled", 7) &&
				 parm->type == POST_PARAM_TYPE_BOOL)
			{
			    *(int *)(data + parm->offset) = 1;
			}
			else if(!strncasecmp(parm->name, "cheap_mode", 10) &&
				 parm->type == POST_PARAM_TYPE_BOOL)
			{
			    *(int *)(data + parm->offset) = 0;
			}
			else if(!strncasecmp(parm->name, "use_progressive_frame_flag", 26) &&
				parm->type == POST_PARAM_TYPE_BOOL)
			{
			    *(int *)(data + parm->offset) = 1;
			}
			else if(!strncasecmp(parm->name, "pulldown", 8) &&
				 parm->type == POST_PARAM_TYPE_INT)
			{
			    *(int *)(data + parm->offset) = 1;
			}
			parm++;
			}
			api->set_parameters(m_pDeinterlacePlugin, (void *)data);
			free(data);
			xine_post_wire_video_port(xine_get_video_source(xineStream->m_pStream), m_pDeinterlacePlugin->video_input[0]);
		}
		else
		{
			g_pPlutoLogger->Write( LV_WARNING, "tvtime failed to load");
		}
		
		xine_post_wire_audio_port( xine_get_audio_source( xineStream->m_pStream ), m_pXineAudio );
        }
        else
        {
            if ( ! m_pXineVisualizationPlugin )
                m_pXineVisualizationPlugin = xine_post_init( m_pXine, "goom", 1, &m_pXineAudio, &m_pXineVideo );

            if ( m_pXineVisualizationPlugin )
            {
                // const char *const *inputs;
                bool bPostWireResult;

                //                 bPostWireResult = xine_post_wire_audio_port(xine_get_audio_source(xineStream->m_pStream), m_pXineVisualizationPlugin->audio_input[1]);
                bPostWireResult = false;

                g_pPlutoLogger->Write( LV_STATUS, "Visualization plugin %s", bPostWireResult ? "enabled" : "disabled" );
                if ( bPostWireResult )
                {
                    xineStream->m_iImgWidth = 100;
                    xineStream->m_iImgHeight = 100;
                }
            }
        }

        g_pPlutoLogger->Write( LV_STATUS, "Got image dimensions: %dx%d", xineStream->m_iImgWidth, xineStream->m_iImgWidth );

        xine_port_send_gui_data( m_pXineVideo, XINE_GUI_SEND_DRAWABLE_CHANGED, ( void * ) windows[ m_iCurrentWindow ] );
        xine_port_send_gui_data( m_pXineVideo, XINE_GUI_SEND_VIDEOWIN_VISIBLE, ( void * ) 1 );

        xineStream->m_bIsRendering = true;

        if ( isNewStream )
        {
            g_pPlutoLogger->Write( LV_STATUS, "Creating event processor" );
            m_bExitThread = false;
            pthread_create( &xineStream->eventLoop, NULL, eventProcessingLoop, xineStream );
            g_pPlutoLogger->Write( LV_STATUS, "Event processor started" );
        }

        if ( ! xineStream->m_bHasVideo )
        {
            g_pPlutoLogger->Write( LV_STATUS, "Stream is seekable: %d", xine_get_stream_info( xineStream->m_pStream, XINE_STREAM_INFO_SEEKABLE ) );
            xine_set_param( xineStream->m_pStream, XINE_PARAM_SPEED, XINE_SPEED_PAUSE );
        }
    }
    else
    {
        g_pPlutoLogger->Write( LV_WARNING, "Failed to open %s", fileName.c_str() );
        return false;
    }

    g_pPlutoLogger->Write( LV_STATUS, "Done with CreateStream" );
    return true;
}

/**
 * @brief play the stream with the streamID identifier.
 */
bool XineSlaveWrapper::playStream( int streamID, string mediaPosition, bool playbackStopped )
{
    StopSpecialSeek();
    if ( m_xine_osd_t )
    {
        // Freeing causes a seg fault.  I guess that means xinelib automatically frees it when the stream changes???
        // xine_osd_free(m_xine_osd_t);
        m_xine_osd_t = NULL;
    }

    XineStream *xineStream = getStreamForId( streamID, "XineSlaveWrapper::playStream() could not play an empty stream!" );

    if ( xineStream == NULL )
        return false;

		PLUTO_SAFETY_LOCK( streamLock, xineStream->m_xineStreamMutex );

    xineStream->m_iPlaybackSpeed = PLAYBACK_NORMAL;
    time_t startTime = time( NULL );

    int Subtitle = -2, Angle = -2, AudioTrack = -2;
    int pos = m_pAggregatorObject->CalculatePosition( mediaPosition, NULL, &Subtitle, &Angle, &AudioTrack );

    if ( xine_play( xineStream->m_pStream, 0, 0 ) )
    {
        g_pPlutoLogger->Write( LV_STATUS, "Playing... The command took %d seconds to complete at pos %d", time( NULL ) - startTime, pos );
        if ( pos )
        {
            // This functionality in xine keeps bouncing back and forth between working and not working
            // With some Xine revisions you can pass the seek position in when you start playing, but
            // other times you must wait at least 500ms after starting the stream before trying to seek
            // or else you get
            // "Stream is not seekable".  In such cases we have a m_iSpecialOneTimeSeek that we will use
            // if we fail to do the seek correctly
            m_iSpecialOneTimeSeek = pos;
        }

        if ( Subtitle != -2 )
            setSubtitle( Subtitle );
        if ( AudioTrack != -2 )
            setAudio( AudioTrack );

        if ( playbackStopped )
            xine_set_param( xineStream->m_pStream, XINE_PARAM_SPEED, XINE_SPEED_PAUSE );
        else
            xine_set_param( xineStream->m_pStream, XINE_PARAM_SPEED, XINE_SPEED_NORMAL );

        m_iPrebuffer = xine_get_param( xineStream->m_pStream, XINE_PARAM_METRONOM_PREBUFFER );

        m_pAggregatorObject->ReportTimecode( xineStream->m_iStreamID, xineStream->m_iPlaybackSpeed );

        return true;
    }
    else
    {
        g_pPlutoLogger->Write( LV_WARNING, "Play failed with error %d", xine_get_error( xineStream->m_pStream ) );
        return false;
    }
}

bool XineSlaveWrapper::closeWindow()
{
    if ( XServerDisplay != NULL )
    {
        XLockDisplay( XServerDisplay );

        XUnmapWindow( XServerDisplay, windows[ m_iCurrentWindow ] );


        //         if ( ! m_pXineVisualizationPlugin )
        //                 m_pXineVisualizationPlugin = xine_post_init(m_pXine, "goom", 1, &m_pXineAudio, &m_pXineVideo);

        //             if  ( m_pXineVisualizationPlugin )
        //             {
        //                 const char *const *inputs;
        //                 bool bPostWireResult;
        //
        // //                 bPostWireResult = xine_post_wire_audio_port(xine_get_audio_source(xineStream->m_pStream), m_pXineVisualizationPlugin->audio_input[1]);
        //                 bPostWireResult = false;

        /*                g_pPlutoLogger->Write(LV_STATUS, "Visualization plugin %s", bPostWireResult ?  "enabled" : "disabled");
                        if ( bPostWireResult )
                        {
                            xineStream->m_iImgWidth = 100;
                            xineStream->m_iImgHeight = 100;
                        }*/
        //             }

        XFreeCursor( XServerDisplay, cursors[ 0 ] );
        XFreeCursor( XServerDisplay, cursors[ 1 ] );

        XDestroyWindow( XServerDisplay, windows[ 0 ] );
        XDestroyWindow( XServerDisplay, windows[ 1 ] );

        XUnlockDisplay( XServerDisplay );
        XCloseDisplay ( XServerDisplay );

        m_bExitThread = true;
        if ( m_pSameStream )
            pthread_join( m_pSameStream->eventLoop, NULL );

        XServerDisplay = NULL;

        delete m_pDynamic_Pointer;
        m_pDynamic_Pointer = NULL;
    }

    return true;
}

static char *experience_labels[] = {
                                       "Beginner",
                                       "Advanced",
                                       "Expert",
                                       "Master of the known universe",
                                       NULL
                                   };

bool XineSlaveWrapper::loadXineConfiguration()
{
    m_pXine = xine_new();
    g_pPlutoLogger->Write( LV_STATUS, "Loading config from %s", m_sConfigFile.c_str() );
    xine_config_load( m_pXine, m_sConfigFile.c_str() );
    xine_init( m_pXine );

    xine_config_register_enum( m_pXine,
                               "gui.experience_level", 0,
                               experience_labels,
                               "display of configuration settings",
                               "Controls whether more advanced configuration settings are shown.",
                               0, NULL, ( void* ) 1 );

    return true;
}

bool XineSlaveWrapper::createXineLibConnection()
{
    if ( !loadXineConfiguration() )
        return false;

    xine_cfg_entry_t xineConfigEntry;
    char **driver_ids;
    int i;
    const char *const *driver_names;

    driver_names = xine_list_video_output_plugins( m_pXine );

    i = 0;
    while ( driver_names[ i++ ] )
        ;
    driver_ids = ( char ** ) xine_xmalloc( sizeof( char * ) * ( i + 1 ) );
    i = 0;
    driver_ids[ i ] = strdup( "auto" );

    while ( driver_names[ i ] )
    {
        driver_ids[ i + 1 ] = strdup( driver_names[ i ] );
        i++;
    }

    driver_ids[ i + 1 ] = NULL;
    xine_config_register_enum( m_pXine, "video.driver", 0, driver_ids, "video driver to use", "Choose video driver. NOTE: you may restart xine to use the new driver", 0, NULL, NULL );
    if ( xine_config_lookup_entry ( m_pXine, "video.driver", &xineConfigEntry ) == 1 )
        m_sXineVideoDriverName = driver_ids[ xineConfigEntry.num_value ];
    else
        g_pPlutoLogger->Write( LV_STATUS, "Video driver key not defined in the config file using: %s", m_sXineVideoDriverName.c_str() );

    free( driver_ids );

    driver_names = xine_list_audio_output_plugins( m_pXine );
    i = 0;
    while ( driver_names[ i++ ] )
        ;
    driver_ids = ( char ** ) xine_xmalloc( sizeof( char * ) * ( i + 1 ) );
    i = 0;
    driver_ids[ i ] = strdup( "auto" );
    while ( driver_names[ i ] )
    {
        driver_ids[ i + 1 ] = strdup( driver_names[ i ] );
        i++;
    }

    driver_ids[ i + 1 ] = NULL;
    xine_config_register_enum( m_pXine, "audio.driver", 0, driver_ids, "audio driver to use", "Choose audio driver. NOTE: you may restart xine to use the new driver", 0, NULL, NULL );
    if ( xine_config_lookup_entry ( m_pXine, "audio.driver", &xineConfigEntry ) == 1 )
        m_sXineAudioDriverName = driver_ids[ xineConfigEntry.num_value ];
    else
        g_pPlutoLogger->Write( LV_STATUS, "Audio driver key not defined in the config file using: %s", m_sXineVideoDriverName.c_str() );
    free( driver_ids );

    g_pPlutoLogger->Write( LV_STATUS, "Using Video driver %s and Audio driver %s", m_sXineVideoDriverName.c_str(), m_sXineAudioDriverName.c_str() );

    
    return true;
}


/**
 * xine Callbacks.
 */

void XineSlaveWrapper::destinationSizeCallback( void *data, int video_width, int video_height, double video_pixel_aspect,
        int *dest_width, int *dest_height,
        double *dest_pixel_aspect )
{
    XineStream * pStream = ( XineStream* ) data;

    /**
    * @test
      if( ! m_pStream->m_bIsRendering )
             g_pPlutoLogger->Write(LV_STATUS, "Destination size callback called (not rendering)!");
    */

    *dest_width = pStream->m_iImgWidth + ( pStream->m_bIsRendering ? 0 : 1 );
    *dest_height = pStream->m_iImgHeight + ( pStream->m_bIsRendering ? 0 : 1 );
    *dest_pixel_aspect = pStream->m_pOwner->m_dPixelAspect;
}

void XineSlaveWrapper::frameOutputCallback( void *data, int video_width, int video_height, double video_pixel_aspect,
        int *dest_x, int *dest_y, int *dest_width, int *dest_height,
        double *dest_pixel_aspect,
        int *win_x, int *win_y )
{
    XineStream * pStream = ( XineStream* ) data;

    /**
    * @test
    *     if( ! pStream->m_bIsRendering)
    *         g_pPlutoLogger->Write(LV_STATUS, "Framer Output callback called (not rendering).");
    */

    *dest_x = 0;
    *dest_y = 0;
    *win_x = pStream->m_iImgXPos;
    *win_y = pStream->m_iImgYPos;
    *dest_width = pStream->m_iImgWidth + ( pStream->m_bIsRendering ? 0 : 1 );
    *dest_height = pStream->m_iImgHeight + ( pStream->m_bIsRendering ? 0 : 1 );
    *dest_pixel_aspect = pStream->m_pOwner->m_dPixelAspect;
}


/**
    \fn XineSlaveWrapper::xineEventListener(void *userData)
 */
void XineSlaveWrapper::xineEventListener( void *userData, const xine_event_t *event )
{
    XineStream * xineStream = ( XineStream * ) userData;
    
    PLUTO_SAFETY_LOCK( streamLock, xineStream->m_xineStreamMutex );

    switch ( event->type )
    {
            case XINE_EVENT_UI_PLAYBACK_FINISHED:
            /**
            * @test
            report_mrl_and_title(NULL);
             clear_tracks();

             menu_snapshots = end_menu = 0;
               if (repeat)
             {
                xine_play(m_pstream, 0, 0);
             }
             else if (mrl[mrl_n+1] != NULL)
                play_mrl(mrl[++mrl_n]);
             else
                send_event(XINE_SE_PLAYBACK_FINISHED, "");
            */
            g_pPlutoLogger->Write( LV_STATUS, "Got XINE_EVENT_UI_PLAYBACK_FINISHED" );
            xineStream->m_pOwner->StopSpecialSeek();
            xineStream->m_pOwner->m_pAggregatorObject->ReportTimecode( xineStream->m_iStreamID, xineStream->m_iPlaybackSpeed );
            xineStream->m_pOwner->playbackCompleted( xineStream->m_iStreamID, false );
            xineStream->m_bIsRendering = false;
            break;
            case XINE_EVENT_QUIT:
                g_pPlutoLogger->Write( LV_STATUS, "Stream was disposed" );
                // the playback completed is sent from another place. (see the stopMedia)
                break;

            case XINE_EVENT_PROGRESS:
            {
                xine_progress_data_t *pProgressEvent = ( xine_progress_data_t * ) event->data;
                g_pPlutoLogger->Write( LV_WARNING, "Playback (%s) is at %d%.", pProgressEvent->description, pProgressEvent->percent );
            }
            break;

            case XINE_EVENT_UI_NUM_BUTTONS:
            {
                xineStream->m_pOwner->StopSpecialSeek();
                xineStream->m_pOwner->m_pAggregatorObject->ReportTimecode( xineStream->m_iStreamID, xineStream->m_iPlaybackSpeed );
                int iButtons = ( ( xine_ui_data_t * ) event->data ) ->num_buttons;

                g_pPlutoLogger->Write( LV_STATUS, "Menu with %d buttons", iButtons );
                if ( xineStream->m_pOwner->m_pAggregatorObject )
                    xineStream->m_pOwner->m_pAggregatorObject->FireMenuOnScreen( xineStream->m_iRequestingObject, xineStream->m_iStreamID, iButtons != 0 );
            }
            break;

            case XINE_EVENT_UI_SET_TITLE:
            if ( g_iSpecialSeekSpeed )
                break; // Ignore this while we're doing all those seeks
            {
                xine_ui_data_t *data = ( xine_ui_data_t * ) event->data;
                g_pPlutoLogger->Write( LV_STATUS, "UI set title: %s %s", data->str, xineStream->m_pOwner->m_pAggregatorObject->GetPosition().c_str() );
                //UI set title: Title 58, Chapter 1,
                const char *p = strstr( data->str, "Title " );
                if ( p )
                    xineStream->m_pOwner->m_pAggregatorObject->m_iTitle = atoi( p + 6 );

                p = strstr( data->str, "Chapter " );
                if ( p )
                    xineStream->m_pOwner->m_pAggregatorObject->m_iChapter = atoi( p + 8 );
            }
            break;

            case XINE_EVENT_INPUT_MOUSE_MOVE:
            if ( g_pDynamic_Pointer )
                g_pDynamic_Pointer->pointer_show();
            break;
            case XINE_EVENT_SPU_BUTTON:
            case XINE_EVENT_INPUT_MOUSE_BUTTON:
            case XINE_EVENT_INPUT_MENU1:
            case XINE_EVENT_INPUT_MENU2:
            case XINE_EVENT_INPUT_MENU3:
            case XINE_EVENT_INPUT_MENU4:
            case XINE_EVENT_INPUT_MENU5:
            case XINE_EVENT_INPUT_MENU6:
            case XINE_EVENT_INPUT_MENU7:
            case XINE_EVENT_INPUT_UP:
            case XINE_EVENT_INPUT_DOWN:
            case XINE_EVENT_INPUT_LEFT:
            case XINE_EVENT_INPUT_RIGHT:
            case XINE_EVENT_INPUT_SELECT:
            case XINE_EVENT_INPUT_NEXT:
            case XINE_EVENT_INPUT_PREVIOUS:
            case XINE_EVENT_INPUT_ANGLE_NEXT:
            case XINE_EVENT_INPUT_ANGLE_PREVIOUS:
            case XINE_EVENT_INPUT_BUTTON_FORCE:
            case XINE_EVENT_INPUT_NUMBER_0:
            case XINE_EVENT_INPUT_NUMBER_1:
            case XINE_EVENT_INPUT_NUMBER_2:
            case XINE_EVENT_INPUT_NUMBER_3:
            case XINE_EVENT_INPUT_NUMBER_4:
            case XINE_EVENT_INPUT_NUMBER_5:
            case XINE_EVENT_INPUT_NUMBER_6:
            case XINE_EVENT_INPUT_NUMBER_7:
            case XINE_EVENT_INPUT_NUMBER_8:
            case XINE_EVENT_INPUT_NUMBER_9:
            case XINE_EVENT_INPUT_NUMBER_10_ADD:
            // ignored at the moment;
            break;

            case XINE_EVENT_UI_CHANNELS_CHANGED:
            if ( g_iSpecialSeekSpeed )
                break; // Ignore this while we're doing all those seeks
            {
                int numaudio = xine_get_stream_info( xineStream->m_pStream, XINE_STREAM_INFO_MAX_AUDIO_CHANNEL );
                if ( numaudio > 1 )
                {
                    string sAudioTracks = StringUtils::itos( xineStream->m_pOwner->getAudio() ) + "\n";
                    int iResult = 1, i;
                    for ( i = 0;i < 100;++i )
                    {
                        char lang[ XINE_LANG_MAX ];
                        strcpy( lang, "**error**" );
                        iResult = xine_get_audio_lang( xineStream->m_pStream, i, lang );
                        if ( iResult == 0 )
                            break;
                        sAudioTracks += string( xineStream->m_pOwner->TranslateLanguage( lang ) ) + "\n";
                    }
                    if ( i >= 100 )
                        g_pPlutoLogger->Write( LV_CRITICAL, "Something went wrong audio tracks>100" );
                    xineStream->m_pOwner->m_pAggregatorObject->DATA_Set_Audio_Tracks( sAudioTracks );
                }
                else
                    g_pPlutoLogger->Write( LV_STATUS, "Ignoring XINE_EVENT_UI_CHANNELS_CHANGED since there aren't multiple audio tracks" );

                int numsubtitle = xine_get_stream_info( xineStream->m_pStream, XINE_STREAM_INFO_MAX_SPU_CHANNEL );
                if ( numsubtitle > 1 )
                {
                    string sSubtitles = StringUtils::itos( xineStream->m_pOwner->getSubtitle() ) + "\n";
                    int iResult = 1, i;
                    for ( i = 0;i < 100;++i )
                    {
                        char lang[ XINE_LANG_MAX ];
                        strcpy( lang, "**error**" );
                        iResult = xine_get_spu_lang( xineStream->m_pStream, i, lang );
                        if ( iResult == 0 )
                            break;
                        sSubtitles += string( xineStream->m_pOwner->TranslateLanguage( lang ) ) + "\n";
                    }
                    if ( i >= 100 )
                        g_pPlutoLogger->Write( LV_CRITICAL, "Something went wrong subitltes>100" );
                    xineStream->m_pOwner->m_pAggregatorObject->DATA_Set_Subtitles( sSubtitles );
                }
                else
                    g_pPlutoLogger->Write( LV_STATUS, "Ignoring XINE_EVENT_UI_CHANNELS_CHANGED since there aren't multiple subtitles" );
            }
            break;

            default:
            g_pPlutoLogger->Write( LV_STATUS, "Got unprocessed Xine playback event: %d", event->type );
            break;
    }

    if ( g_pDynamic_Pointer )
        g_pDynamic_Pointer->pointer_check_time();
}

bool XineSlaveWrapper::setXineStreamDebugging( int m_istreamID, bool newValue )
{
    XineStream * xineStream = getStreamForId( m_istreamID, "Trying to set debugging level for and invalid stream: (%d)" );

    if ( xineStream == NULL )
        return false;
        
    PLUTO_SAFETY_LOCK( streamLock, xineStream->m_xineStreamMutex );

    if ( newValue )
    {
        xine_set_param( xineStream->m_pStream, XINE_PARAM_VERBOSITY, XINE_VERBOSITY_DEBUG );
    }
    else
    {
        xine_set_param( xineStream->m_pStream, XINE_PARAM_VERBOSITY, XINE_VERBOSITY_NONE );
    }

    return true;
}


void XineSlaveWrapper::resume()
{
    if ( m_pXineVideo != NULL )
    {
        int count = 50;

        while ( --count &&
                !xine_port_send_gui_data( m_pXineVideo, XINE_GUI_SEND_DRAWABLE_CHANGED, ( void * ) windows[ m_iCurrentWindow ] ) )
            xine_usec_sleep( 20000 );

        if ( !count )
        {
            g_pPlutoLogger->Write(LV_STATUS, "RESUME video driver failed. make sure we have a patched xine-lib with XV2 driver\n" );
            g_pPlutoLogger->Write(LV_STATUS, "  and no other application is using the XVideo port.\n" );
            return ;
        }

        xine_port_send_gui_data( m_pXineVideo, XINE_GUI_SEND_VIDEOWIN_VISIBLE, ( void * ) 1 );
    }
}

void *XineSlaveWrapper::eventProcessingLoop( void *arguments )
{
    XineStream * pStream = ( XineStream* ) arguments;

    Bool checkResult;

    Display *pDisplay = XOpenDisplay( getenv( "DISPLAY" ) );

    int iCounter_TimeCode = 0;
    int iCounter = 0;  // A counter for the special seek
    XEvent event;
    while ( ! pStream->m_pOwner->m_bExitThread )
    {
        if ( pStream->m_bIsRendering )
        {
            do
            {
                XLockDisplay( pStream->m_pOwner->XServerDisplay );
                checkResult = XCheckWindowEvent( pStream->m_pOwner->XServerDisplay, pStream->m_pOwner->windows[ pStream->m_pOwner->m_iCurrentWindow ], INPUT_MOTION, &event );
                XUnlockDisplay( pStream->m_pOwner->XServerDisplay );

                if ( checkResult == True )
                    pStream->m_pOwner->XServerEventProcessor( pStream, event );

            }
            while ( checkResult == True );
        }

        if ( iCounter++ > 10 )                   // Every second
        {
            g_pPlutoLogger->Write( LV_WARNING, "%s (seek %d) t.c. ctr %d freq %d,", pStream->m_pOwner->m_pAggregatorObject->GetPosition().c_str(), g_iSpecialSeekSpeed, iCounter_TimeCode, pStream->m_pOwner->m_iTimeCodeReportFrequency );
            iCounter = 0;
            if ( pStream->m_pOwner->m_iTimeCodeReportFrequency && ++iCounter_TimeCode >= pStream->m_pOwner->m_iTimeCodeReportFrequency )
            {
                pStream->m_pOwner->m_pAggregatorObject->ReportTimecode( pStream->m_iStreamID, pStream->m_iPlaybackSpeed );
                iCounter_TimeCode = 1;
            }
        }
        if ( pStream->m_pOwner->m_iSpecialOneTimeSeek && iCounter > 5 ) // We need to wait 500ms after the stream starts before doing the seek!
        {
            pStream->m_pOwner->Seek(pStream->m_pOwner->m_iSpecialOneTimeSeek,10000); // As long as we're within 10 seconds that's fine
            pStream->m_pOwner->m_iSpecialOneTimeSeek = 0;
        }
        if ( g_iSpecialSeekSpeed )
            pStream->m_pOwner->HandleSpecialSeekSpeed();

        usleep( 100000 );
    }

    XCloseDisplay( pDisplay );
    return NULL;
}

int XineSlaveWrapper::XServerEventProcessor( XineStream *pStream, XEvent &event )
{
		PLUTO_SAFETY_LOCK( streamLock, pStream->m_xineStreamMutex );

    Atom XA_DELETE_WINDOW;

    switch ( event.type )
    {
            case ClientMessage:
            {
                XA_DELETE_WINDOW = XInternAtom( XServerDisplay, "WM_DELETE_WINDOW", False );

                if ( ( unsigned ) event.xclient.data.l[ 0 ] == XA_DELETE_WINDOW )
                    pStream->m_bIsRendering = false;
                break;
            }

            case MotionNotify:
            {
                XMotionEvent *mevent = ( XMotionEvent * ) & event;
                int x, y;

                if ( pStream->m_pOwner->translate_point( mevent->x, mevent->y, &x, &y ) )
                {
                    xine_event_t xineEvent;
                    xine_input_data_t xineInput;

                    xineEvent.type = XINE_EVENT_INPUT_MOUSE_MOVE;
                    xineEvent.stream = pStream->m_pStream;
                    xineEvent.data = &xineInput;
                    xineEvent.data_length = sizeof( xineInput );
                    gettimeofday( &xineEvent.tv, NULL );

                    xineInput.button = 0;
                    xineInput.x = x;
                    xineInput.y = y;
                    xine_event_send( pStream->m_pStream, &xineEvent );
                }
                break;
            }

            case ButtonPress:
            {
                XButtonEvent *bevent = ( XButtonEvent * ) & event;
                xine_input_data_t xineInputData;
                xine_event_t xineEvent;

                x11_rectangle_t rect;

                g_pPlutoLogger->Write( LV_STATUS, "Xine player: mouse button event: mx=%d my=%d", bevent->x, bevent->y );

                rect.x = bevent->x;
                rect.y = bevent->y;
                rect.w = 0;
                rect.h = 0;

                xine_gui_send_vo_data ( pStream->m_pStream, XINE_GUI_SEND_TRANSLATE_GUI_TO_VIDEO, ( void* ) & rect );
                g_pPlutoLogger->Write( LV_STATUS, "Xine player: mouse button event after translation: mx=%d my=%d", rect.x, rect.y );

                xineEvent.stream = pStream->m_pStream;
                xineEvent.type = XINE_EVENT_INPUT_MOUSE_BUTTON;
                xineEvent.data = &xineInputData;
                xineEvent.data_length = sizeof( xineInputData );

                xineInputData.button = bevent->button;
                xineInputData.x = rect.x;
                xineInputData.y = rect.y;

                gettimeofday( &xineEvent.tv, NULL );
                XLockDisplay( bevent->display );
                xine_event_send( pStream->m_pStream, &xineEvent );
                XUnlockDisplay( bevent->display );

                break;
            }

            case KeyPress:
            {
                XKeyEvent kevent;
                KeySym ksym;
                char kbuf[ 256 ];
                int len;

                kevent = event.xkey;

                XLockDisplay( kevent.display );
                len = XLookupString( &kevent, kbuf, sizeof( kbuf ), &ksym, NULL );
                XUnlockDisplay( kevent.display );
                // g_pPlutoLogger->Write(LV_STATUS, "Key (%d), \"%s\" - %d", len, kbuf, ksym);

                xine_event_t xineEvent;
                // xine_input_data_t  xineInput;

                switch ( ksym )
                {
                        case XK_Up:
                        xineEvent.type = XINE_EVENT_INPUT_UP;
                        break;
                        case XK_Down:
                        xineEvent.type = XINE_EVENT_INPUT_DOWN;
                        break;
                        case XK_Left:
                        xineEvent.type = XINE_EVENT_INPUT_LEFT;
                        break;
                        case XK_Right:
                        xineEvent.type = XINE_EVENT_INPUT_RIGHT;
                        break;
                        case XK_Return:
                        xineEvent.type = XINE_EVENT_INPUT_SELECT;
                        break;
                        default:
                        xineEvent.type = 0;
                }
                xineEvent.stream = pStream->m_pStream;
                xineEvent.data = NULL;
                xineEvent.data_length = 0;
                gettimeofday( &xineEvent.tv, NULL );
                xine_event_send( pStream->m_pStream, &xineEvent );
                break;
            }

            case Expose:
            {
                XExposeEvent *exposeEvent = ( XExposeEvent * ) & event;
                // g_pPlutoLogger->Write(LV_STATUS, "Expose with count %d", exposeEvent->count);

                if ( exposeEvent->count != 0 )
                    break;

                if ( pStream->m_pOwner->m_pXineVideo )
                    xine_port_send_gui_data( pStream->m_pOwner->m_pXineVideo, XINE_GUI_SEND_EXPOSE_EVENT, exposeEvent );

                break;
            }

            case ConfigureNotify:
            {
                XConfigureEvent *cev = ( XConfigureEvent * ) & event;
                Window tmp_win;

                pStream->m_iImgWidth = cev->width;
                pStream->m_iImgHeight = cev->height;

                if ( ( cev->x == 0 ) && ( cev->y == 0 ) )
                {
                    XLockDisplay( cev->display );
                    XTranslateCoordinates( cev->display, cev->window, DefaultRootWindow( cev->display ), 0, 0, &pStream->m_iImgXPos, &pStream->m_iImgYPos, &tmp_win );
                    XUnlockDisplay( cev->display );
                }
                else
                {
                    pStream->m_iImgXPos = cev->x;
                    pStream->m_iImgYPos = cev->y;
                }
            }
            break;
    }

    return 1;
}

int XineSlaveWrapper::translate_point( int gui_x, int gui_y, int *video_x, int *video_y )
{
    x11_rectangle_t rect;
    //   int             xwin, ywin;
    //   unsigned int    wwin, hwin, bwin, dwin;
    //   float           xf,yf;
    //   float           scale, width_scale, height_scale,aspect;
    //   Window          rootwin;

    rect.x = gui_x;
    rect.y = gui_y;
    rect.w = 0;
    rect.h = 0;

    if ( m_pXineVideo && xine_port_send_gui_data( m_pXineVideo, XINE_GUI_SEND_TRANSLATE_GUI_TO_VIDEO, ( void* ) & rect ) != -1 )
    {
        /**
         driver implements gui->video coordinate space translation, use it
        */
        *video_x = rect.x;
        *video_y = rect.y;
        return 1;
    }

    return 1;
    /**
    * @test
     Driver cannot convert gui->video space, fall back to old code...

     if(XGetGeometry(XServerDisplay, windows[m_iCurrentWindow], &rootwin, &xwin, &ywin, &wwin, &hwin, &bwin, &dwin) == BadDrawable)
        return 0;

     Scale co-ordinate to image dimensions.

      height_scale = (float) m_imageHeight / (float) hwin;
      width_scale  = (float) m_imageWidth / (float) wwin;
      aspect       = (float) m_imageWidth / (float) m_imageHeight;
      if(((float) wwin / (float) hwin) \< aspect) {
      scale      = width_scale;
      xf         = (float) gui_x * scale;
      yf         = (float) gui_y * scale;
      hwin       = (int) (hwin * scale);

     FIXME: The 1.25 should really come from the NAV packets.

        *video_x   = (int) (xf * 1.25 / aspect);
        *video_y   = (int) (yf - ((hwin - m_imageHeight) / 2));
     }
     else {
      scale    = height_scale;
      xf       = (float) gui_x * scale;
      yf       = (float) gui_y * scale;
      wwin     = (int) (wwin * scale);

      FIXME: The 1.25 should really come from the NAV packets.

         *video_x = (int) ((xf - ((wwin - m_imageWidth) / 2)) * 1.25 / aspect);
         *video_y = (int) yf;
       }

       return 1;
    */
}



/**
    \fn XineSlaveWrapper::changePlaybackSpeed(int iStreamID, int iMediaPlaybackSpeed)
 */
void XineSlaveWrapper::changePlaybackSpeed( int iStreamID, PlayBackSpeedType desiredSpeed, bool ReportOnOSD )
{
    XineStream * pStream;
    g_pPlutoLogger->Write(LV_STATUS,"XineSlaveWrapper::changePlaybackSpeed speed %d",(int) desiredSpeed);

    if ( ( pStream = getStreamForId( iStreamID, "Can't set the speed of a non existent stream (%d)!" ) ) == NULL )
        return ;

		PLUTO_SAFETY_LOCK( streamLock, pStream->m_xineStreamMutex );

    int xineSpeed = XINE_SPEED_PAUSE;
    int NewSpecialSeekSpeed = 0;
    pStream->m_iPlaybackSpeed = desiredSpeed;
    m_pAggregatorObject->ReportTimecode( pStream->m_iStreamID, pStream->m_iPlaybackSpeed );
    switch ( desiredSpeed )
    {
            case PLAYBACK_STOP:
            xineSpeed = XINE_SPEED_PAUSE;
            break;
            case PLAYBACK_FF_1_4:
            xineSpeed = XINE_SPEED_SLOW_4;
            break;
            case PLAYBACK_FF_1_2:
            xineSpeed = XINE_SPEED_SLOW_2;
            break;
            case PLAYBACK_FF_1:
            xineSpeed = XINE_SPEED_NORMAL;
            break;
            case PLAYBACK_FF_2:
            //if( pStream->m_bHasVideo )  Now for some reason Xine uses 100% CPU and kills the system if you it run faster than 1x, so do this in our hacked special seek speed loop--nasty hack
            //        xineSpeed = XINE_SPEED_FAST_2;
            //else
            xineSpeed = XINE_SPEED_NORMAL;
            NewSpecialSeekSpeed = desiredSpeed;
            break;
            case PLAYBACK_FF_4:
            //      if( pStream->m_bHasVideo )
            //            xineSpeed = XINE_SPEED_FAST_4;
            //      else
            xineSpeed = XINE_SPEED_NORMAL;
            NewSpecialSeekSpeed = desiredSpeed;  // See above
            break;

			default:
				NewSpecialSeekSpeed = desiredSpeed;
				xineSpeed = XINE_SPEED_NORMAL;
            break;
    }

    if ( ReportOnOSD==false )
       DisplayOSDText( "" );
    else
       DisplaySpeedAndTimeCode();

    if ( g_iSpecialSeekSpeed && !NewSpecialSeekSpeed )
       StopSpecialSeek();
    else if ( NewSpecialSeekSpeed )
       StartSpecialSeek( NewSpecialSeekSpeed );

    g_pPlutoLogger->Write( LV_STATUS, "Setting speed to special %d real %d desired %d", g_iSpecialSeekSpeed, xineSpeed, desiredSpeed );
    if ( ( xineSpeed == XINE_SPEED_PAUSE && desiredSpeed == 0 ) || xineSpeed != XINE_SPEED_PAUSE )
        xine_set_param( pStream->m_pStream, XINE_PARAM_SPEED, xineSpeed );
}

XineSlaveWrapper::PlayBackSpeedType XineSlaveWrapper::getPlaybackSpeed( int iStreamID )
{
    XineStream * pStream;

    if ( ( pStream = getStreamForId( iStreamID, "Can't get the speed of a non existent stream (%d)!" ) ) == NULL )
        return PLAYBACK_STOP;
        
    PLUTO_SAFETY_LOCK( streamLock, pStream->m_xineStreamMutex );

    int currentSpeed;
    switch ( ( currentSpeed = xine_get_param( pStream->m_pStream, XINE_PARAM_SPEED ) ) )
    {
            case XINE_SPEED_SLOW_4:
            return PLAYBACK_FF_1_4;

            case XINE_SPEED_SLOW_2:
            return PLAYBACK_FF_1_2;

            case XINE_SPEED_NORMAL:
            return PLAYBACK_FF_1;

            case XINE_SPEED_FAST_2:
            return PLAYBACK_FF_2;

            case XINE_SPEED_FAST_4:
            return PLAYBACK_FF_4;

            case XINE_SPEED_PAUSE:
            return PLAYBACK_STOP;

            default:
            {
                g_pPlutoLogger->Write( LV_WARNING, "Can't translate current Xine speed %d. Assuming normal playback speed!", currentSpeed );
                return PLAYBACK_FF_1;
            }
    }
}

/**
    \fn XineSlaveWrapper::stopMedia(int iStreamID)
 */
void XineSlaveWrapper::stopMedia( int iStreamID )
{
    XineStream * pStream = getStreamForId( iStreamID, "Can't stop the playback of a non existent stream (%d)!" );

    if ( pStream == NULL )
        return ;
        
	playbackCompleted( pStream->m_iStreamID, false );
    // stop the event thread first
    if ( pStream->eventLoop )
    {
        g_pPlutoLogger->Write( LV_STATUS, "Stopping event thread." );
        m_bExitThread = true;

        pthread_join( pStream->eventLoop, NULL );
        g_pPlutoLogger->Write( LV_STATUS, "Done." );
    }

    if ( pStream->m_pStreamEventQueue )
    {
        g_pPlutoLogger->Write( LV_STATUS, "Disposing the event queue" );
        xine_event_dispose_queue( pStream->m_pStreamEventQueue );
    }

    if ( pStream->m_pStream )
    {
	      PLUTO_SAFETY_LOCK( streamLock, pStream->m_xineStreamMutex );

        g_pPlutoLogger->Write( LV_STATUS, "Calling xine_stop for stream with id: %d", iStreamID );
        xine_stop( pStream->m_pStream );

        g_pPlutoLogger->Write( LV_STATUS, "Calling xine_close for stream with id: %d", iStreamID );
        xine_close( pStream->m_pStream );

        g_pPlutoLogger->Write( LV_STATUS, "Calling xine_dispose for stream with id: %d", iStreamID );
        xine_dispose( pStream->m_pStream );
    }

    g_pPlutoLogger->Write( LV_STATUS, "Going to call a %p and v %p", m_pXineAudio, m_pXineVideo );

    if ( m_pXineAudio )
    {
        g_pPlutoLogger->Write( LV_STATUS, "Calling xine_close_audio_driver for stream with id: %d" );
        xine_close_audio_driver( m_pXine, m_pXineAudio );
        m_pXineAudio = NULL;
    }

    if ( m_pXineVideo )
    {
        g_pPlutoLogger->Write( LV_STATUS, "Calling xine_close_video_driver for stream with id: %d" );
        xine_close_video_driver( m_pXine, m_pXineVideo );
        m_pXineVideo = NULL;
    }

    g_pPlutoLogger->Write( LV_STATUS, "deleting pstream" );
    delete pStream;

    // ignore the return since we know for sure that it was the same stream
    setStreamForId( iStreamID, NULL );

    g_pPlutoLogger->Write( LV_STATUS, "Cleanup completed" );
}

/**
    \fn XineSlaveWrapper::restartMediaStream(int iStreamID)
 */
void XineSlaveWrapper::restartMediaStream( int iStreamID )
{
    changePlaybackSpeed( iStreamID, PLAYBACK_FF_1, false );
}

/**
    \fn XineSlaveWrapper::pauseMediaStream(int iStreamID)
 */
void XineSlaveWrapper::pauseMediaStream( int iStreamID )
{
    int stoppedTime, completeTime;

    if ( getPlaybackSpeed( iStreamID ) == PLAYBACK_STOP )
        changePlaybackSpeed( iStreamID, PLAYBACK_FF_1, false );
    else
        changePlaybackSpeed( iStreamID, PLAYBACK_STOP, false );

    getStreamPlaybackPosition( iStreamID, stoppedTime, completeTime );
    g_pPlutoLogger->Write( LV_STATUS, "Stream paused at time: %d from %d", stoppedTime, completeTime );
}

/**
 * \fn void XineSlaveWrapper::sendInputEvent(int eventID);
 */
void XineSlaveWrapper::sendInputEvent( int eventType )
{
    g_iSpecialSeekSpeed = 0;

    time_t startTime = time( NULL );
    XineStream *pStream;
    if ( ( pStream = getStreamForId( 1, "void XineSlaveWrapper::sendInputEvent(int eventType) getting one stream" ) ) == NULL )
        return ;

    g_pPlutoLogger->Write( LV_STATUS, "Sending %d event to the engine.", eventType );

    xine_event_t event;

    event.type = eventType;
    event.stream = pStream->m_pStream;
    event.data = NULL;
    event.data_length = 0;
    gettimeofday( &event.tv, NULL );

    xine_event_send( pStream->m_pStream, &event );
    g_pPlutoLogger->Write( LV_STATUS, "XineSlaveWrapper::sendInputEvent() Sending input event with ID: %d took %d seconds", eventType, time( NULL ) - startTime );
}

void XineSlaveWrapper::XineStream::setPlaybackSpeed( int speed )
{
    /** @todo: Make this proper.
         changePlaybackSpeed(iS)
    */
}

string XineSlaveWrapper::getRenderingWindowName()
{
    return m_sWindowTitle;
}

void XineSlaveWrapper::selectNextButton( int iStreamID )
{
    XineStream * pStream = getStreamForId( iStreamID, "Can't navigate on an non existent m_pstream (%d)!" );

    if ( pStream == NULL )
        return ;
        
    PLUTO_SAFETY_LOCK( streamLock, pStream->m_xineStreamMutex );

    g_pPlutoLogger->Write( LV_STATUS, "Selecting next hot spot on the m_pstream %d", iStreamID );

    xine_event_t event;

    event.type = XINE_EVENT_INPUT_UP;
    event.stream = pStream->m_pStream;
    event.data = NULL;
    event.data_length = 0;
    gettimeofday( &event.tv, NULL );

    xine_event_send( pStream->m_pStream, &event );
}

void XineSlaveWrapper::selectPrevButton( int iStreamID )
{
    XineStream * pStream = getStreamForId( iStreamID, "Can't navigate on an non existent m_pstream (%d)!" );

    if ( pStream == NULL )
        return ;
        
    PLUTO_SAFETY_LOCK( streamLock, pStream->m_xineStreamMutex );

    g_pPlutoLogger->Write( LV_STATUS, "Selecting prev hot spot on the m_pstream %d", iStreamID );

    xine_event_t event;

    event.type = XINE_EVENT_INPUT_DOWN;
    event.stream = pStream->m_pStream;
    event.data = NULL;
    event.data_length = 0;
    gettimeofday( &event.tv, NULL );

    xine_event_send( pStream->m_pStream, &event );
}

void XineSlaveWrapper::pushCurrentButton( int iStreamID )
{
    XineStream * pStream = getStreamForId( iStreamID, "Can't push the buttons on invalid m_pstream (%d)" );

    if ( pStream == NULL )
        return ;

		PLUTO_SAFETY_LOCK( streamLock, pStream->m_xineStreamMutex );

    g_pPlutoLogger->Write( LV_STATUS, "Selecting next hot spot on the m_pstream %d", iStreamID );

    xine_event_t event;

    event.type = XINE_EVENT_INPUT_SELECT;
    event.stream = pStream->m_pStream;
    event.data = NULL;
    event.data_length = 0;
    gettimeofday( &event.tv, NULL );

    xine_event_send( pStream->m_pStream, &event );
}

void XineSlaveWrapper::setXinePlayerObject( Xine_Player *object )
{
    m_pAggregatorObject = object;
}

void XineSlaveWrapper::getScreenShot( int iStreamID, int iWidth, int iHeight, char *&pData, int &iDataSize, string &sFormat, string &sCMD_Result )
{
    XineStream * pStream = getStreamForId( iStreamID, "I need a valid stream to be able to make screenshots." );

    if ( pStream == NULL )
        return ;

		PLUTO_SAFETY_LOCK( streamLock, pStream->m_xineStreamMutex );

    // only make screenshots if it's a video stream.
    if ( ! pStream->m_bHasVideo )
    {
        iDataSize = 0;
        return ;
    }

    sFormat = "JPG";
    g_pPlutoLogger->Write( LV_STATUS, "Making snapshot" );
    make_snapshot( pStream->m_pStream, sFormat.c_str(), iWidth, iHeight, true, pData, iDataSize );
}

void XineSlaveWrapper::make_snapshot( xine_stream_t *pStream, string sFormat, int iWidth, int iHeight, bool bKeepAspect, char*&pData, int &iDataSize )
{
    //   uint8_t   *yuv, *y, *u, *v, *rgb;
    //   void      *blob;
    //   int        ratio, format;
    //   int        width, height;
    //   double     desired_ratio, image_ratio, f;
    //   static int initMagick = 1;
    //
    // /*   FILE *outfile; */
    //
    //   ExceptionInfo exception;
    //   Image *image;
    //   Image *resize_image;
    //   ImageInfo *image_info;

    int imageWidth = 0, imageHeight = 0, imageRatio = 0, imageFormat = 0;
    uint8_t *imageData = NULL;

    if ( pStream == NULL )
    {
        g_pPlutoLogger->Write( LV_WARNING, "XineSlaveWrapper::make_snapshot(): The xine_stream_t object passed as parameter was null. Can't get the screen shot" );
        return ;
    }
    
    g_pPlutoLogger->Write( LV_STATUS, "XineSlaveWrapper::make_snapshot(): Getting frame info" );
    if ( ! xine_get_current_frame( pStream, &imageWidth, &imageHeight, &imageRatio, &imageFormat, NULL ) )
    {
        g_pPlutoLogger->Write( LV_WARNING, "XineSlaveWrapper::make_snapshot(): Error getting frame info. Returning!" );
        return ;
    }
    g_pPlutoLogger->Write( LV_STATUS, "XineSlaveWrapper::make_snapshot(): Got %d %d %d 0x%x", imageWidth, imageHeight, imageRatio, imageFormat );

    imageData = ( uint8_t* ) malloc ( ( imageWidth + 8 ) * ( imageHeight + 1 ) * 2 );
    g_pPlutoLogger->Write( LV_STATUS, "XineSlaveWrapper::make_snapshot(): Getting frame data" );
    if ( ! xine_get_current_frame ( pStream, &imageWidth, &imageHeight, &imageRatio, &imageFormat, imageData ) )
    {
        g_pPlutoLogger->Write( LV_WARNING, "XineSlaveWrapper::make_snapshot(): Error getting frame data. Returning!" );
        return ;
    }
    g_pPlutoLogger->Write( LV_STATUS, "XineSlaveWrapper::make_snapshot(): Got %d %d %d 0x%x", imageWidth, imageHeight, imageRatio, imageFormat );

    // we should have the image in YV12 format aici
    // if not then we try to convert it.
    if ( imageFormat == XINE_IMGFMT_YUY2 )
    {
        uint8_t * yuy2Data = imageData;
        imageData = ( uint8_t * ) malloc ( imageWidth * imageHeight * 2 );

        yuy2toyv12 (
            imageData,
            imageData + imageWidth * imageHeight,
            imageData + imageWidth * imageHeight * 5 / 4, yuy2Data, imageWidth, imageHeight );

        free ( yuy2Data );
    }

    g_pPlutoLogger->Write( LV_STATUS, "XineSlaveWrapper::make_snapshot(): Converted to YV12" );
    // convert to RGB
    // keep the yv12Data array around to be able to delete it
    uint8_t *yv12Data = imageData;

    /** @brief this function will allocate the output parameter */
    imageData = yv12torgb (
                    imageData,
                    imageData + imageWidth * imageHeight,
                    imageData + imageWidth * imageHeight * 5 / 4,
                    imageWidth, imageHeight );
    g_pPlutoLogger->Write( LV_STATUS, "XineSlaveWrapper::make_snapshot(): Converted to RGB!" );
    free( yv12Data );

    double outputRatio;
    // double currentRatio;

    g_pPlutoLogger->Write( LV_STATUS, "XineSlaveWrapper::make_snapshot(): Temp data was freed here!" );
    switch ( imageRatio )
    {
            case XINE_VO_ASPECT_ANAMORPHIC:                   /* anamorphic     */
            case XINE_VO_ASPECT_PAN_SCAN:                     /* we display pan&scan as widescreen */
            outputRatio = 16.0 / 9.0;
            break;

            case XINE_VO_ASPECT_DVB:                          /* 2.11:1 */
            outputRatio = 2.11 / 1.0;
            break;

            case XINE_VO_ASPECT_SQUARE:                       /* square pels */
            case XINE_VO_ASPECT_DONT_TOUCH:                   /* probably non-mpeg m_pstream => don't touch aspect ratio */
            outputRatio = ( double ) imageWidth / ( double ) imageHeight;
            break;

            default:
            g_pPlutoLogger->Write( LV_WARNING, "XineSlaveWrapper::make_snapshot(): Unknown aspect ratio for image (%d) using 4:3", imageRatio );

            case XINE_VO_ASPECT_4_3:                          /* 4:3             */
            outputRatio = 4.0 / 3.0;
            break;
    }

    double f = outputRatio / ( ( double ) imageWidth / ( double ) imageHeight );

    g_pPlutoLogger->Write( LV_STATUS, "XineSlaveWrapper::make_snapshot(): The ratio between the current ration and the output ratio is %f", f );
    double t_width, t_height;
    //     if( !bKeepAspect )
    //     {
    if ( f >= 1.0 )
    {
        t_width = imageWidth * f;
        t_height = imageHeight;
    }
    else
    {
        t_width = imageWidth;
        t_height = imageHeight / f;
    }

    t_width /= 2;
    t_height /= 2;

    g_pPlutoLogger->Write( LV_STATUS, "XineSlaveWrapper::make_snapshot(): Making jpeg from RGB" );
    JpegEncoderDecoder jpegEncoder;

    jpegEncoder.encodeIntoBuffer( ( char * ) imageData, imageWidth, imageHeight, 3, pData, iDataSize );

    g_pPlutoLogger->Write( LV_STATUS, "XineSlaveWrapper::make_snapshot(): final image size: %d", iDataSize );
    free( imageData );

    FILE * file;
    file = fopen( "/tmp/file.jpg", "wb" );
    g_pPlutoLogger->Write( LV_STATUS, "XineSlaveWrapper::make_snapshot(): temporary image filep %p", file );

    fwrite( pData, iDataSize, 1, file );
    fclose( file );

    g_pPlutoLogger->Write( LV_STATUS, "XineSlaveWrapper::make_snapshot(): At the end. Returning" );
    return ;
}

void XineSlaveWrapper::yuy2toyv12 ( uint8_t *y, uint8_t *u, uint8_t *v, uint8_t *input, int width, int height )
{
    int i, j, w2;

    w2 = width / 2;

    for ( i = 0; i < height; i += 2 )
    {
        for ( j = 0; j < w2; j++ )
        {
            /*
             * packed YUV 422 is: Y[i] U[i] Y[i+1] V[i]
             */
            *( y++ ) = *( input++ );
            *( u++ ) = *( input++ );
            *( y++ ) = *( input++ );
            *( v++ ) = *( input++ );
        }

        /*
         * down sampling
         */
        for ( j = 0; j < w2; j++ )
        {
            /*
             * skip every second line for U and V
             */
            *( y++ ) = *( input++ );
            input++;
            *( y++ ) = *( input++ );
            input++;
        }
    }
}

/*
 *   Create rgb data from yv12
 */

#define clip_8_bit(val)           \
{                                 \
   if (val < 0)                   \
      val = 0;                    \
   else                           \
      if (val > 255) val = 255;   \
}

uint8_t * XineSlaveWrapper::yv12torgb ( uint8_t *src_y, uint8_t *src_u, uint8_t *src_v, int width, int height )
{
    int i, j;

    int y, u, v;
    int r, g, b;

    int sub_i_uv;
    int sub_j_uv;

    int uv_width, uv_height;

    uint8_t *rgb;

    uv_width = width / 2;
    uv_height = height / 2;

    rgb = ( uint8_t* ) malloc ( width * height * 3 );
    if ( !rgb )
        return NULL;

    for ( i = 0; i < height; ++i )
    {
        /*
         * calculate u & v rows
         */
        sub_i_uv = ( ( i * uv_height ) / height );

        for ( j = 0; j < width; ++j )
        {
            /*
             * calculate u & v columns
             */
            sub_j_uv = ( ( j * uv_width ) / width );

            /***************************************************
             *
             *  Colour conversion from
             *    http://www.inforamp.net/~poynton/notes/colour_and_gamma/ColorFAQ.html#RTFToC30
             *
             *  Thanks to Billy Biggs <vektor@dumbterm.net>
             *  for the pointer and the following conversion.
             *
             *   R' = [ 1.1644         0    1.5960 ]   ([ Y' ]   [  16 ])
             *   G' = [ 1.1644   -0.3918   -0.8130 ] * ([ Cb ] - [ 128 ])
             *   B' = [ 1.1644    2.0172         0 ]   ([ Cr ]   [ 128 ])
             *
             *  Where in xine the above values are represented as
             *
             *   Y' == image->y
             *   Cb == image->u
             *   Cr == image->v
             *
             ***************************************************/

            y = src_y[ ( i * width ) + j ] - 16;
            u = src_u[ ( sub_i_uv * uv_width ) + sub_j_uv ] - 128;
            v = src_v[ ( sub_i_uv * uv_width ) + sub_j_uv ] - 128;

            r = ( int ) ( ( 1.1644 * y ) + ( 1.5960 * v ) );
            g = ( int ) ( ( 1.1644 * y ) - ( 0.3918 * u ) - ( 0.8130 * v ) );
            b = ( int ) ( ( 1.1644 * y ) + ( 2.0172 * u ) );

            clip_8_bit ( r );
            clip_8_bit ( g );
            clip_8_bit ( b );

            rgb[ ( i * width + j ) * 3 + 0 ] = r;
            rgb[ ( i * width + j ) * 3 + 1 ] = g;
            rgb[ ( i * width + j ) * 3 + 2 ] = b;
        }
    }

    return rgb;
}

XineSlaveWrapper::XineStream *XineSlaveWrapper::setStreamForId( int iStreamID, XineStream *pNewStream )
{
    XineStream * pXineStream = NULL;

    pXineStream = m_pSameStream;
    m_pSameStream = pNewStream;

    return pXineStream;
}

XineSlaveWrapper::XineStream *XineSlaveWrapper::getStreamForId( int iStreamID, string strMessageInvalid )
{
    /** @warning HACK: use the same m_pstream regardless of the StreamID; */

    //     if ( m_pSameStream == NULL )
    //         m_pSameStream = new XineStream();;

    if ( m_pSameStream == NULL )
        g_pPlutoLogger->Write( LV_STATUS, strMessageInvalid.c_str(), iStreamID );

    return m_pSameStream;
}

/**
* @test
 void XineSlaveWrapper::closePlaybackStream(int iStreamID)
 {
     XineStream *m_pstream;

     m_pstream = m_mapStreams[iStreamID];

     if ( m_pstream == NULL )
     {
         g_pPlutoLogger->Write(LV_WARNING, "Can't close and non existent m_pstream!");
         return;
     }
     xine_close(m_pstream->m_pstream);
 }
 */

void XineSlaveWrapper::selectMenu( int iStreamID, int iMenuType )
{
    XineStream * xineStream = getStreamForId( iStreamID, "Can't select menu for a nonexistent stream!" );

    if ( xineStream == NULL )
        return ;
        
    PLUTO_SAFETY_LOCK( streamLock, xineStream->m_xineStreamMutex );

    xine_event_t xine_event;

    xine_event.type = iMenuType + XINE_EVENT_INPUT_MENU1;
    xine_event.data_length = 0;
    xine_event.data = NULL;
    xine_event.stream = xineStream->m_pStream;
    gettimeofday( &xine_event.tv, NULL );

    xine_event_send( xineStream->m_pStream, &xine_event );
}

void XineSlaveWrapper::playbackCompleted( int iStreamID, bool bWithErrors )
{
    g_pPlutoLogger->Write( LV_STATUS, "Fire playback completed event %d", ( int ) m_isSlimClient );
    XineStream * xineStream = getStreamForId( iStreamID, "Can't get the position of a nonexistent stream!" );

    if ( ! m_isSlimClient )
		m_pAggregatorObject->EVENT_Playback_Completed(m_pAggregatorObject->m_sCurrentFile,iStreamID, bWithErrors );
}

int XineSlaveWrapper::getStreamPlaybackPosition( int iStreamID, int &positionTime, int &totalTime )
{
		//PLUTO_SAFETY_LOCK( xp, m_pAggregatorObject->m_xineSlaveMutex ); // There are lots of problems when 2 threads try to get the position at the same time

    if ( m_pDynamic_Pointer )
        m_pDynamic_Pointer->pointer_check_time();

    XineStream * xineStream = getStreamForId( iStreamID, "Can't get the position of a nonexistent stream!" );

    if ( xineStream == NULL )
        return 0;

		PLUTO_SAFETY_LOCK( streamLock, xineStream->m_xineStreamMutex );

    if ( xine_get_stream_info( xineStream->m_pStream, XINE_STREAM_INFO_SEEKABLE ) == 0 )
    {
        g_pPlutoLogger->Write( LV_STATUS, "Stream is not seekable" );
        positionTime = totalTime = 0;
        return 0;
    }

    int iPosStream = 0;
    int iPosTime = 0;
    int iLengthTime = 0;

    int count = 10;
    while ( --count && ! xine_get_pos_length( xineStream->m_pStream, &iPosStream, &iPosTime, &iLengthTime ) )
    {
        g_pPlutoLogger->Write( LV_STATUS, "Error reading stream position: %d", xine_get_error( xineStream->m_pStream ) );
        Sleep( 30 );
    }

    positionTime = iPosTime;
    totalTime = iLengthTime;
    return positionTime;
}

int XineSlaveWrapper::enableBroadcast( int iStreamID )
{
    XineStream * pStream = getStreamForId( iStreamID, "Can't broadcast a stream that is not available" );

    if ( pStream == NULL )
        return 0;
        
    PLUTO_SAFETY_LOCK( streamLock, pStream->m_xineStreamMutex );

    int portNumber = 7866;
    if ( portNumber != xine_get_param( pStream->m_pStream, XINE_PARAM_BROADCASTER_PORT ) )
    {
        //         if( port && xine_get_param(pStream->m_pStream, XINE_PARAM_BROADCASTER_PORT) )
        xine_set_param( pStream->m_pStream, XINE_PARAM_BROADCASTER_PORT, portNumber );

        /* try up to ten times from port base. sometimes we have trouble
        * binding to the same port we just used.
        */
        for ( int i = 0; i < 10; i++ )
        {
            xine_set_param( pStream->m_pStream, XINE_PARAM_BROADCASTER_PORT, ++portNumber );
            if ( portNumber == xine_get_param( pStream->m_pStream, XINE_PARAM_BROADCASTER_PORT ) )
                return portNumber;
        }
    }

    return 0;
}

void XineSlaveWrapper::simulateMouseClick( int X, int Y )
{
    XineStream * pStream;
    xine_input_data_t xineInputData;
    xine_event_t xineEvent;

    if ( ( pStream = getStreamForId( 1, "XineSlaveWrapper::simulateMouseClick() getting one stream" ) ) == NULL )
        return ;

		PLUTO_SAFETY_LOCK( streamLock, pStream->m_xineStreamMutex );

    g_pPlutoLogger->Write( LV_STATUS, "XineSlaveWrapper::simulateMouseClick(): simulating mouse click: mx=%d my=%d", X, Y );

    xineEvent.stream = pStream->m_pStream;
    xineEvent.type = XINE_EVENT_INPUT_MOUSE_BUTTON;
    xineEvent.data = &xineInputData;
    xineEvent.data_length = sizeof( xineInputData );

    xineInputData.button = 1;
    xineInputData.x = X;
    xineInputData.y = Y;

    gettimeofday( &xineEvent.tv, NULL );
    XLockDisplay( XServerDisplay );
    xine_event_send( pStream->m_pStream, &xineEvent );
    XUnlockDisplay( XServerDisplay );
}

void XineSlaveWrapper::simulateKeystroke( int plutoButton )
{
    Window oldWindow;
    int oldRevertBehaviour;
    g_pPlutoLogger->Write( LV_STATUS, "XineSlaveWrapper::simulateKeystroke(): plutoButton=%d", plutoButton );

    XLockDisplay( XServerDisplay );
    XGetInputFocus( XServerDisplay, &oldWindow, &oldRevertBehaviour );
    XSetInputFocus( XServerDisplay, windows[ m_iCurrentWindow ], RevertToParent, CurrentTime );
    XTestFakeKeyEvent( XServerDisplay, XKeysymToKeycode( XServerDisplay, translatePlutoKeySymToXKeySym( plutoButton ) ), True, 0 );
    XTestFakeKeyEvent( XServerDisplay, XKeysymToKeycode( XServerDisplay, translatePlutoKeySymToXKeySym( plutoButton ) ), False, 0 );

    if ( oldWindow )
        XSetInputFocus( XServerDisplay, oldWindow, oldRevertBehaviour, CurrentTime );

    XFlush( XServerDisplay );
    XUnlockDisplay( XServerDisplay );
}

KeySym XineSlaveWrapper::translatePlutoKeySymToXKeySym( int plutoButton )
{
    switch ( plutoButton )
    {
            case BUTTON_Up_Arrow_CONST:
            return XK_Up;
            case BUTTON_Down_Arrow_CONST:
            return XK_Down;
            case BUTTON_Left_Arrow_CONST:
            return XK_Left;
            case BUTTON_Right_Arrow_CONST:
            return XK_Right;
            case BUTTON_Enter_CONST:
            return XK_Return;

            default:
            g_pPlutoLogger->Write( LV_WARNING, "Translating of the %d pluto key is not handled yet.", plutoButton );
            return 0;
    }
}

static const char *audio_out_types_strs[] =
    {
        "Mono 1.0",
        "Stereo 2.0",
        "Headphones 2.0",
        "Stereo 2.1",
        "Surround 3.0",
        "Surround 4.0",
        "Surround 4.1",
        "Surround 5.0",
        "Surround 5.1",
        "Surround 6.0",
        "Surround 6.1",
        "Surround 7.1",
        "Pass Through",
        NULL
    };

void XineSlaveWrapper::setOutputSpeakerArrangement( string strOutputSpeakerArrangement )
{
    xine_cfg_entry_t xineConfigEntry;

    g_pPlutoLogger->Write( LV_STATUS, "Setting the audio output speaker arrangement: %s", strOutputSpeakerArrangement.c_str() );

    xine_config_register_enum ( m_pXine, "audio.output.speaker_arrangement", 1, ( char ** ) audio_out_types_strs,
                                "Speaker arrangement",
                                NULL, 0, NULL, NULL );

    if ( xine_config_lookup_entry( m_pXine, "audio.output.speaker_arrangement", &xineConfigEntry ) == 0 )
    {
        g_pPlutoLogger->Write( LV_STATUS, "Could not lookup the current Speaker Arrangement Value" );
        return ;
    }

    g_pPlutoLogger->Write( LV_STATUS, "Current value: %s", audio_out_types_strs[ xineConfigEntry.num_value ] );

    g_pPlutoLogger->Write( LV_STATUS, "Trying to set the value to: %s", strOutputSpeakerArrangement.c_str() );
    int i = 0;
    while ( audio_out_types_strs[ i ] != NULL )
    {
        if ( strncmp( strOutputSpeakerArrangement.c_str( ), audio_out_types_strs[ i ], strlen( audio_out_types_strs[ i ] ) ) == 0 )
        {
            xineConfigEntry.num_value = i;
            xine_config_update_entry( m_pXine, &xineConfigEntry );

            if ( xine_config_lookup_entry( m_pXine, "audio.output.speaker_arrangement", &xineConfigEntry ) == 0 )
            {
                g_pPlutoLogger->Write( LV_STATUS, "Could not lookup the current Speaker Arrangement Value after update." );
                return ;
            }

            g_pPlutoLogger->Write( LV_STATUS, "Value changed to: %s", audio_out_types_strs[ xineConfigEntry.num_value ] );
            return ;
        }

        i++;
    }

    return ;
}

int XineSlaveWrapper::getDeviceId()
{
    return m_pAggregatorObject->m_dwPK_Device;
}

bool XineSlaveWrapper::isSlimClient()
{
    return m_isSlimClient;
}

void XineSlaveWrapper::setSlimClient( bool isSlimClient )
{
    m_isSlimClient = isSlimClient;
}

const char *XineSlaveWrapper::TranslateLanguage( const char *abbreviation )
{
    const char * pAbbreviation;
    if ( abbreviation[ 0 ] == ' ' )
        pAbbreviation = &abbreviation[ 1 ];
    else
        pAbbreviation = abbreviation;

    if ( strcmp( pAbbreviation, "en" ) == 0 )
        return "English";
    else if ( strcmp( pAbbreviation, "de" ) == 0 )
        return "Deutsch";
    else if ( strcmp( pAbbreviation, "fr" ) == 0 )
        return "Francais";

    return pAbbreviation;
}

bool XineSlaveWrapper::setSubtitle( int Value )
{
    XineStream * xineStream = getStreamForId( 1, "Trying to set parm for and invalid stream: (%d)" );

    if ( xineStream == NULL )
        return false;
        
    PLUTO_SAFETY_LOCK( streamLock, xineStream->m_xineStreamMutex );

    g_pPlutoLogger->Write( LV_STATUS, "SPU was %d now %d", getSubtitle(), Value );
    xine_set_param( xineStream->m_pStream, XINE_PARAM_SPU_CHANNEL, Value );

    return true;
}

bool XineSlaveWrapper::setAudio( int Value )
{
    XineStream * xineStream = getStreamForId( 1, "Trying to set parm for and invalid stream: (%d)" );

    if ( xineStream == NULL )
        return false;
        
    PLUTO_SAFETY_LOCK( streamLock, xineStream->m_xineStreamMutex );

    g_pPlutoLogger->Write( LV_STATUS, "AUDIO was %d now %d", xine_get_param ( xineStream->m_pStream, XINE_PARAM_AUDIO_CHANNEL_LOGICAL ), Value );
    xine_set_param( xineStream->m_pStream, XINE_PARAM_AUDIO_CHANNEL_LOGICAL, Value );

    return true;
}

int XineSlaveWrapper::getSubtitle()
{
    XineStream * xineStream = getStreamForId( 1, "Trying to set parm for and invalid stream: (%d)" );

    if ( xineStream == NULL )
        return -1;
        
    PLUTO_SAFETY_LOCK( streamLock, xineStream->m_xineStreamMutex );

    return xine_get_param ( xineStream->m_pStream, XINE_PARAM_SPU_CHANNEL );
}

int XineSlaveWrapper::getAudio()
{
    XineStream * xineStream = getStreamForId( 1, "Trying to set parm for and invalid stream: (%d)" );

    if ( xineStream == NULL )
        return 0;
        
    PLUTO_SAFETY_LOCK( streamLock, xineStream->m_xineStreamMutex );

    return xine_get_param ( xineStream->m_pStream, XINE_PARAM_AUDIO_CHANNEL_LOGICAL );
}

void XineSlaveWrapper::DisplaySpeedAndTimeCode()
{
    XineStream * xineStream = getStreamForId( 1, "Trying to set parm for and invalid stream: (%d)" );

    if ( xineStream == NULL )
        return ;
        
    PLUTO_SAFETY_LOCK( streamLock, xineStream->m_xineStreamMutex );

    int Whole = xineStream->m_iPlaybackSpeed / 1000;
    int Fraction = xineStream->m_iPlaybackSpeed % 1000;
    string sSpeed;

    if ( Fraction < 0 )
    {
        Fraction *= -1;
        sSpeed += "-";
    }
    else if ( Whole )
        sSpeed += StringUtils::itos( Whole );
    else
        sSpeed += "0";

    if ( Fraction )
        sSpeed += "." + StringUtils::itos( Fraction );

    sSpeed += "x     ";

    int seconds, totalTime;
    getStreamPlaybackPosition( 1, seconds, totalTime );
    g_pPlutoLogger->Write( LV_STATUS, "seconds %d", seconds );
    seconds /= 1000;
    int seconds_only = seconds;
    int hours = seconds / 3600;
    seconds -= hours * 3600;
    int minutes = seconds / 60;
    seconds -= minutes * 60;
    g_pPlutoLogger->Write( LV_STATUS, "h %d m %d s %d", hours, minutes, seconds );
    if ( hours )
    {
        sSpeed += StringUtils::itos( hours ) + ":";
        if ( minutes < 10 )
            sSpeed += "0" + StringUtils::itos( minutes ) + ":";
        else
            sSpeed += StringUtils::itos( minutes ) + ":";
    }
    else
        sSpeed += StringUtils::itos( minutes ) + ":";

    if ( seconds < 10 )
        sSpeed += "0" + StringUtils::itos( seconds );
    else
        sSpeed += StringUtils::itos( seconds );

//    if ( ( g_iSpecialSeekSpeed == 0 ) || ( seconds_only == 1 ) )
//        DisplayOSDText("");
//    else
        DisplayOSDText( sSpeed );
}

void XineSlaveWrapper::DisplayOSDText( string sText )
{
    XineStream * xineStream = getStreamForId( 1, "Trying to set parm for and invalid stream: (%d)" );
    g_pPlutoLogger->Write( LV_CRITICAL, "XineSlaveWrapper::DisplayOSDText %p %s", (void *) m_xine_osd_t, sText.c_str() );
    if ( xineStream == NULL )
        return ;

		PLUTO_SAFETY_LOCK( streamLock, xineStream->m_xineStreamMutex );

    // now fixed
    // todo -- put this back?!!!  Something is freeing the osd and it's not us, so this crashes!
    if ( sText.size() == 0 )
    {
        g_pPlutoLogger->Write( LV_CRITICAL, "Clearing OSD %p", m_xine_osd_t );
        if ( m_xine_osd_t )
            xine_osd_free( m_xine_osd_t );
        m_xine_osd_t = NULL;
	// this is already done in main changePlayback
        //g_iSpecialSeekSpeed = 0;
        return ;
    }

    if ( m_xine_osd_t )
        xine_osd_free( m_xine_osd_t );
    m_xine_osd_t = xine_osd_new( xineStream->m_pStream, 0, 0, 1000, 100 );
    xine_osd_set_font( m_xine_osd_t, "sans", 20 );
    xine_osd_set_text_palette( m_xine_osd_t,
                               XINE_TEXTPALETTE_WHITE_BLACK_TRANSPARENT, XINE_OSD_TEXT1 );
    xine_osd_draw_rect( m_xine_osd_t, 0, 0, 999, 99, XINE_OSD_TEXT1, 1 );
    xine_osd_draw_text( m_xine_osd_t, 20, 20, sText.c_str(), XINE_OSD_TEXT1 );
    xine_osd_show( m_xine_osd_t, 0 );

    g_pPlutoLogger->Write( LV_CRITICAL, "DisplayOSDText() : Attempting to display %s", sText.c_str() );
}

void XineSlaveWrapper::StartSpecialSeek( int Speed )
{
    XineStream * xineStream = getStreamForId( 1, "Trying to set parm for and invalid stream: (%d)" );

    if ( xineStream == NULL )
        return ;

		PLUTO_SAFETY_LOCK( streamLock, xineStream->m_xineStreamMutex );

    xine_set_param(xineStream->m_pStream, XINE_PARAM_IGNORE_AUDIO, 1);

    int totalTime;
    gettimeofday( &m_tsLastSpecialSeek, NULL );
    getStreamPlaybackPosition( 1, m_posLastSpecialSeek, totalTime );

    g_pPlutoLogger->Write( LV_STATUS, "Starting special seek %d", Speed );
    xine_set_param( xineStream->m_pStream, XINE_PARAM_METRONOM_PREBUFFER, 9000 );
    m_iPrebuffer = xine_get_param( xineStream->m_pStream, XINE_PARAM_METRONOM_PREBUFFER );
    g_iSpecialSeekSpeed = Speed;
//    xineStream->m_iPlaybackSpeed = PLAYBACK_NORMAL;
    DisplaySpeedAndTimeCode();
    g_pPlutoLogger->Write( LV_STATUS, "done Starting special seek %d", Speed );
}

void XineSlaveWrapper::StopSpecialSeek()
{
    XineStream * xineStream = getStreamForId( 1, "Trying to set parm for and invalid stream: (%d)" );

    if ( xineStream == NULL )
        return ;

		PLUTO_SAFETY_LOCK( streamLock, xineStream->m_xineStreamMutex );

    xine_set_param(xineStream->m_pStream, XINE_PARAM_IGNORE_AUDIO, 0);

    g_pPlutoLogger->Write( LV_STATUS, "Stopping special seek" );
    g_iSpecialSeekSpeed = 0;
    xineStream->m_iPlaybackSpeed = PLAYBACK_NORMAL;
    DisplayOSDText("");
    xine_set_param( xineStream->m_pStream, XINE_PARAM_METRONOM_PREBUFFER, m_iPrebuffer );
    g_pPlutoLogger->Write( LV_STATUS, "done Stopping special seek" );
}

void XineSlaveWrapper::Dynamic_Pointer::pointer_hide()
{
    Window window = m_pXineSlaveWrapper->windows[m_pXineSlaveWrapper->m_iCurrentWindow];
    XDefineCursor( m_pXineSlaveWrapper->XServerDisplay, window, *m_pCursor_hidden );
    m_start_time = 0;
}

void XineSlaveWrapper::Dynamic_Pointer::pointer_show()
{
    Window window = m_pXineSlaveWrapper->windows[m_pXineSlaveWrapper->m_iCurrentWindow];
    XDefineCursor( m_pXineSlaveWrapper->XServerDisplay, window, *m_pCursor_normal );
    m_start_time = time( NULL );
}

void XineSlaveWrapper::Dynamic_Pointer::pointer_check_time()
{
    if ( m_start_time == 0 )
        return ;
    time_t now_time = time( NULL );
    if ( difftime( now_time, m_start_time ) >= POINTER_HIDE_SECONDS )
        pointer_hide();
}

XineSlaveWrapper::Dynamic_Pointer::Dynamic_Pointer(
    XineSlaveWrapper *pXineSlaveWrapper,
    Cursor *pCursor_normal,
    Cursor *pCursor_hidden
) :
        m_pXineSlaveWrapper(pXineSlaveWrapper),
        m_pCursor_normal(pCursor_normal),
        m_pCursor_hidden(pCursor_hidden),
        m_start_time(0)
{
    g_pDynamic_Pointer = this;
}

XineSlaveWrapper::Dynamic_Pointer::~Dynamic_Pointer()
{
    g_pDynamic_Pointer = NULL;
}

void XineSlaveWrapper::Seek(int pos,int tolerance_ms)
{
    XineStream * xineStream = getStreamForId( 1, "Trying to set parm for and invalid stream: (%d)" );
    if ( xineStream == NULL )
        return ;
        
    PLUTO_SAFETY_LOCK( streamLock, xineStream->m_xineStreamMutex );

    if( tolerance_ms==0 )
    {

        timespec ts1,ts2,tsElapsed;
        gettimeofday( &ts1, NULL );

        //xine_play( xineStream->m_pStream, 0, pos );
	xine_seek( xineStream->m_pStream, 0, pos );

        gettimeofday( &ts2, NULL );
        tsElapsed = ts2-ts1;
        int positionTime, totalTime;
        getStreamPlaybackPosition( 1, positionTime, totalTime );
        g_pPlutoLogger->Write(LV_STATUS,"Seek took %d ms.  Tried for pos %d landed at %d, off by %d",
            tsElapsed.tv_sec * 1000 + tsElapsed.tv_nsec / 1000000,
            pos,positionTime,positionTime-pos);
        return ;
    }

    g_pPlutoLogger->Write( LV_WARNING, "XineSlaveWrapper::Seek seek to %d tolerance %d", pos, tolerance_ms );

    for ( int i = 0;i < 10;++i )
    {
        int positionTime, totalTime;
        if ( abs( getStreamPlaybackPosition( 1, positionTime, totalTime ) - pos ) < tolerance_ms )
        {
            g_pPlutoLogger->Write( LV_WARNING, "XineSlaveWrapper::Seek Close enough %d %d total %d", positionTime, pos, totalTime );
            break;
        }
        else
        {
            g_pPlutoLogger->Write( LV_WARNING, "XineSlaveWrapper::Seek get closer currently at: %d target pos: %d ctr %d", positionTime, pos, i );
	    xine_seek( xineStream->m_pStream, 0, pos );
            //xine_play( xineStream->m_pStream, 0, pos );
        }
    }
}

void XineSlaveWrapper::HandleSpecialSeekSpeed()
{
    XineStream * xineStream = getStreamForId( 1, "Trying to set parm for and invalid stream: (%d)" );
    if ( xineStream == NULL || m_tsLastSpecialSeek.tv_sec==0 )  // Should not happen
        return ;

		PLUTO_SAFETY_LOCK( streamLock, xineStream->m_xineStreamMutex );

    DisplaySpeedAndTimeCode();

    timespec ts;
    gettimeofday( &ts, NULL );

    timespec tsElapsed = ts-m_tsLastSpecialSeek;
    int msElapsed = tsElapsed.tv_sec * 1000 + tsElapsed.tv_nsec / 1000000;
    int seekTime = m_posLastSpecialSeek + (msElapsed * g_iSpecialSeekSpeed / 1000);  // Take the time that did elapse, factor the speed difference, and add it to the last seek
    int positionTime, totalTime;
    getStreamPlaybackPosition( 1, positionTime, totalTime );

    g_pPlutoLogger->Write(LV_STATUS,"HandleSpecialSeekSpeed %d elapsed: %d ms last: %d this: %d pos %d",
        g_iSpecialSeekSpeed, msElapsed,
        m_posLastSpecialSeek,seekTime,positionTime);

    if ( seekTime < 0 || seekTime > totalTime )
    {
        g_pPlutoLogger->Write( LV_CRITICAL, "aborting seek" );
        StopSpecialSeek();
        m_pAggregatorObject->ReportTimecode( xineStream->m_iStreamID, xineStream->m_iPlaybackSpeed );
        return;
    }

    m_tsLastSpecialSeek=ts;
    m_posLastSpecialSeek=seekTime;
    Seek(seekTime,0);
}
