#include <math.h>

#include <xine.h>
#include <xine/xineutils.h>
#include <xine/video_out.h>
#include <xine/audio_out.h>

#include "DCE/Logger.h"
#include "DCE/DCEConfig.h"

#include "Xine_Stream.h"
#include "Xine_Player.h"
#include "Colorspace_Utils.h"
#include "JpegEncoderDecoder.h"

#include "pluto_main/Define_Button.h"
#include "Gen_Devices/AllCommandsRequests.h"

#define POINTER_HIDE_SECONDS 2

static DCE::Xine_Stream::Dynamic_Pointer *g_pDynamic_Pointer = NULL;

/** Defined this here to avoid some dependency. */
typedef struct
{
	uint32_t flags;
	uint32_t functions;
	uint32_t decorations;
	int32_t input_mode;
	uint32_t status;
}
MWMHints;

#define INPUT_MOTION (ExposureMask | ButtonPressMask | KeyPressMask | \
                      ButtonMotionMask | StructureNotifyMask | PropertyChangeMask | PointerMotionMask)

static const char noCursorDataDescription[] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

namespace DCE
{ // DCE namespace begin

Xine_Stream::Xine_Stream(Xine_Stream_Factory* pFactory, xine_t *pXineLibrary, int ID, int iTimeCodeReportFrequency, int iRequestingObject) :
		m_streamMutex("xine-stream-access-mutex"),
		m_xine_osd_t(NULL)
{
	pthread_mutexattr_t mutexAttr;
	pthread_mutexattr_init( &mutexAttr );
	pthread_mutexattr_settype( &mutexAttr,  PTHREAD_MUTEX_RECURSIVE_NP );
	m_streamMutex.Init( &mutexAttr );	
	
	m_pFactory = pFactory;
	m_pXineLibrary = pXineLibrary;
	m_sXineAudioDriverName = m_pFactory->GetAudioDriver();
	m_sXineVideoDriverName = m_pFactory->GetVideoDriver();
	
	m_pXineVisualizationPlugin = NULL;
	m_pXineDeinterlacePlugin = NULL;

	m_bInitialized = false;
	
	m_pXineAudioOutput = NULL;
	m_pXineVideoOutput = NULL;
	
	m_sWindowTitle = "pluto-xine-playback-window";
	m_pXDisplay = NULL;
	m_iCurrentScreen = 0;
	m_iCurrentWindow = 0;
	
	m_iSpecialSeekSpeed = 0;
	m_iSpecialOneTimeSeek = 0;
	m_iPrebuffer = 90000;
	
	m_iTimeCodeReportFrequency = iTimeCodeReportFrequency;
	
	m_bExitThread = false;
	m_bTrickModeActive = false;
	m_iStreamID = ID;
	m_iRequestingObject = iRequestingObject;
	
	threadEventLoop = NULL;
	m_pXineStreamEventQueue = NULL;
	
	m_pDynamic_Pointer = new Dynamic_Pointer(this, &cursors[0], &cursors[1]);
}

Xine_Stream::~Xine_Stream()
{
	ShutdownStream();
	DestroyWindows();
}

// prepare stream for usage
bool Xine_Stream::StartupStream()
{
	if (m_bInitialized)
	{
		g_pPlutoLogger->Write( LV_WARNING, "Double stream initialization attempted - aborting command");
		return false;
	}

	if ( !CreateWindows() )
	{
		g_pPlutoLogger->Write( LV_WARNING, "Stream output window creation failed");
		return false;
	}
	
	if ( !InitXineAVOutput() )
	{
		g_pPlutoLogger->Write( LV_WARNING, "Stream audio/video initialization failed");
		return false;
	}

	m_iTitle=m_iChapter=-1;
	
	m_iPrebuffer = xine_get_param( m_pXineStream, XINE_PARAM_METRONOM_PREBUFFER );
	
	m_bInitialized = true;
	return true;
}

// deinitialize stream
bool Xine_Stream::ShutdownStream()
{
	{
		PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
		if (!m_bInitialized)
			return false;
	}
	
	playbackCompleted(false );
	
	{
		PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
		m_bInitialized = false; 
	}
	
	// stop the event thread first
	if ( threadEventLoop )
	{
		g_pPlutoLogger->Write( LV_STATUS, "Stopping event processor." );
		
		{
			PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
			m_bExitThread = true;
		}

		pthread_join( threadEventLoop, NULL );
		g_pPlutoLogger->Write( LV_STATUS, "Done." );
	}
	
	if ( m_pXineStreamEventQueue )
	{
		g_pPlutoLogger->Write( LV_STATUS, "Disposing the event queue" );
		xine_event_dispose_queue( m_pXineStreamEventQueue );
		{
			PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
			m_pXineStreamEventQueue = NULL;
		}
	}
	
	if ( m_pXineStream )
	{	
		g_pPlutoLogger->Write( LV_STATUS, "Calling xine_stop for stream with id: %d", m_iStreamID );
		xine_stop( m_pXineStream );

		g_pPlutoLogger->Write( LV_STATUS, "Calling xine_close for stream with id: %d", m_iStreamID );
		xine_close( m_pXineStream );

		
		g_pPlutoLogger->Write( LV_STATUS, "Calling xine_dispose for stream with id: %d", m_iStreamID );
		xine_dispose( m_pXineStream );
		
		PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
		m_pXineStream = NULL;
	}

	g_pPlutoLogger->Write( LV_STATUS, "Going to call a %p and v %p", m_pXineAudioOutput, m_pXineVideoOutput );

	if ( m_pXineAudioOutput )
	{
		g_pPlutoLogger->Write( LV_STATUS, "Calling xine_close_audio_driver for stream with id: %d", m_iStreamID );
		xine_close_audio_driver( m_pXineLibrary, m_pXineAudioOutput );
		PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
		m_pXineAudioOutput = NULL;
	}

	if ( m_pXineVideoOutput )
	{
		g_pPlutoLogger->Write( LV_STATUS, "Calling xine_close_video_driver for stream with id: %d", m_iStreamID );
		xine_close_video_driver( m_pXineLibrary, m_pXineVideoOutput );
		PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
		m_pXineVideoOutput = NULL;
	}

	g_pPlutoLogger->Write( LV_STATUS, "Cleanup completed" );
	
	{
		PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
		m_sCurrentFile = "";
		m_iTitle=m_iChapter=-1;		
	}
	
	return true;
}

// creates stream windows
bool Xine_Stream::CreateWindows()
{
	XColor black;
	XSizeHints sizeHints;
	MWMHints wmHints;
	XClassHint classHint;

	long propertyValues[ 1 ];
	int completionEvent;
	int xpos, ypos, width, height;

	double res_h, res_v;

	if ( ( m_pXDisplay = XOpenDisplay( getenv( "DISPLAY" ) ) ) == NULL )
	{
		g_pPlutoLogger->Write( LV_WARNING, "Could not open X DISPLAY from: %s", getenv( "DISPLAY" ) );
		return false;
	}

	XLockDisplay( m_pXDisplay );

	m_iCurrentScreen = XDefaultScreen( m_pXDisplay );
	xpos = 10;
	ypos = 20;
	width = 720;
	height = 540;

	windows[ 0 ] = XCreateSimpleWindow( m_pXDisplay, XDefaultRootWindow( m_pXDisplay ), xpos, ypos, width, height, 
																			1, 0, 0 );
	windows[ 1 ] = XCreateSimpleWindow( m_pXDisplay, XDefaultRootWindow( m_pXDisplay ),
																			0, 0, ( DisplayWidth( m_pXDisplay, m_iCurrentScreen ) ), DisplayHeight( m_pXDisplay, m_iCurrentScreen ),
																			0, 0, 0 );

	classHint.res_name = ( char* ) m_sWindowTitle.c_str();
	classHint.res_class = ( char* ) m_sWindowTitle.c_str();
	XSetClassHint ( m_pXDisplay, windows[ 0 ], &classHint );

	classHint.res_class = ( char* ) m_sWindowTitle.c_str();
	XSetClassHint ( m_pXDisplay, windows[ 0 ], &classHint );

	XSelectInput( m_pXDisplay, windows[ 0 ], INPUT_MOTION );
	XSelectInput( m_pXDisplay, windows[ 1 ], INPUT_MOTION );

	XSetStandardProperties( m_pXDisplay, windows[ 0 ], m_sWindowTitle.c_str(), m_sWindowTitle.c_str(), None, NULL, 0, 0 );
	XSetStandardProperties( m_pXDisplay, windows[ 1 ], m_sWindowTitle.c_str(), m_sWindowTitle.c_str(), None, NULL, 0, 0 );

	sizeHints.win_gravity = StaticGravity;
	sizeHints.flags = PPosition | PSize | PWinGravity;

	XSetWMNormalHints( m_pXDisplay, windows[ 0 ], &sizeHints );
	XSetWMNormalHints( m_pXDisplay, windows[ 1 ], &sizeHints );

	Atom XA_DELETE_WINDOW = XInternAtom( m_pXDisplay, "WM_DELETE_WINDOW", False );
	XSetWMProtocols( m_pXDisplay, windows[ 0 ], &XA_DELETE_WINDOW, 1 );
	XSetWMProtocols( m_pXDisplay, windows[ 1 ], &XA_DELETE_WINDOW, 1 );

	Atom XA_NO_BORDER = XInternAtom( m_pXDisplay, "_MOTIF_WM_HINTS", False );
	wmHints.flags = ( 1L << 1 ); // MWM_HINTS_DECORATIONS
	wmHints.decorations = 0;

	XChangeProperty( m_pXDisplay, windows[ 1 ], XA_NO_BORDER, XA_NO_BORDER, 32,
									 PropModeReplace, ( unsigned char * ) & wmHints,
									 5 /* PROP_MWM_HINTS_ELEMENTS */ );

	Atom XA_WIN_LAYER = XInternAtom( m_pXDisplay, "_NET_WM_STATE", False );
	propertyValues[ 0 ] = 10;
	XChangeProperty( m_pXDisplay, windows[ 1 ], XA_WIN_LAYER, XA_CARDINAL, 32, PropModeReplace, ( unsigned char * ) propertyValues, 1 );


	XA_WIN_LAYER = XInternAtom( m_pXDisplay, "_NET_WM_STATE", False );
	propertyValues[ 0 ] = 12;
	XChangeProperty( m_pXDisplay, windows[ 1 ], XA_WIN_LAYER, XA_CARDINAL, 32, PropModeReplace, ( unsigned char * ) propertyValues, 1 );

	if ( XShmQueryExtension( m_pXDisplay ) == True )
		completionEvent = XShmGetEventBase( m_pXDisplay ) + ShmCompletion;
	else
		completionEvent = -1;

	noCursor = XCreateBitmapFromData( m_pXDisplay, ( DefaultRootWindow( m_pXDisplay ) ), noCursorDataDescription, 8, 8 );
	cursors[0] = XCreateFontCursor(m_pXDisplay, XC_left_ptr);
	cursors[1] = XCreatePixmapCursor(m_pXDisplay, noCursor, noCursor, &black, &black, 0, 0);
	
	if ( m_pDynamic_Pointer )
		delete m_pDynamic_Pointer;
	m_pDynamic_Pointer = new Dynamic_Pointer(this, &cursors[0], &cursors[1]);

	XFreePixmap( m_pXDisplay, noCursor );

	XDefineCursor( m_pXDisplay, windows[ m_iCurrentWindow ], cursors[ m_iCurrentWindow ] );
	XMapRaised( m_pXDisplay, windows[ m_iCurrentWindow ] );

	if ( m_pDynamic_Pointer )
		m_pDynamic_Pointer->pointer_hide();

	res_h = ( DisplayWidth( m_pXDisplay, m_iCurrentScreen ) * 1000 / DisplayWidthMM( m_pXDisplay, m_iCurrentScreen ) );
	res_v = ( DisplayHeight( m_pXDisplay, m_iCurrentScreen ) * 1000 / DisplayHeightMM( m_pXDisplay, m_iCurrentScreen ) );

	m_dPixelAspect = res_v / res_h;

	g_pPlutoLogger->Write( LV_STATUS, "XServer aspect %f", m_dPixelAspect );

	if ( fabs( m_dPixelAspect - 1.0 ) < 0.01 )
		m_dPixelAspect = 1.0;

	XSync( m_pXDisplay, True );
	XUnlockDisplay( m_pXDisplay );

	return true;
}

// initializes audion/video output ports, prepares stream for open
bool Xine_Stream::InitXineAVOutput()
{
	// init visual for xine video
	m_x11Visual.display = m_pXDisplay;
	m_x11Visual.screen = m_iCurrentScreen;
	m_x11Visual.d = windows[ m_iCurrentWindow ];
	
	m_x11Visual.dest_size_cb = &destinationSizeCallback;
	m_x11Visual.frame_output_cb = &frameOutputCallback;
	
	m_x11Visual.user_data = this;

	// init video output
	g_pPlutoLogger->Write( LV_STATUS, "Opening Video Driver" );
	{
		PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
		if ( ( m_pXineVideoOutput = xine_open_video_driver( m_pXineLibrary, m_sXineVideoDriverName.c_str(), XINE_VISUAL_TYPE_X11, ( void * ) & m_x11Visual ) ) == NULL )
		{
			g_pPlutoLogger->Write( LV_WARNING, "I'm unable to initialize m_pXine's '%s' video driver. Falling to 'xshm'.", m_sXineVideoDriverName.c_str() );
			if ( ( m_pXineVideoOutput = xine_open_video_driver( m_pXineLibrary, "xshm", XINE_VISUAL_TYPE_X11, ( void * ) & m_x11Visual ) ) == NULL )
			{
				g_pPlutoLogger->Write( LV_WARNING, "I'm unable to initialize m_pXine's 'xshm' video driver. Giving up." );
				return false;
			}
		}
	}

	// init audio output
	g_pPlutoLogger->Write( LV_STATUS, "Opening Audio Driver" );
	{
		PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
		if ( ( m_pXineAudioOutput = xine_open_audio_driver( m_pXineLibrary, m_sXineAudioDriverName.c_str(), NULL ) ) == NULL )
		{
			g_pPlutoLogger->Write( LV_WARNING, "I'm unable to initialize m_pXine's '%s' audio driver.", m_sXineAudioDriverName.c_str() );
			xine_close_video_driver(m_pXineLibrary, m_pXineVideoOutput);
			return false;
		}
	}

	// init xine stream
	g_pPlutoLogger->Write( LV_STATUS, "Calling xine_stream_new" );
	{
		PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
		if ( ( m_pXineStream = xine_stream_new( m_pXineLibrary, m_pXineAudioOutput, m_pXineVideoOutput ) ) == NULL )
		{
			g_pPlutoLogger->Write( LV_WARNING, "Could not create stream" );
			xine_close_audio_driver(m_pXineLibrary, m_pXineAudioOutput);
			xine_close_video_driver(m_pXineLibrary, m_pXineVideoOutput);
			return false;
		}
	}
	
	// creating new queue
	g_pPlutoLogger->Write( LV_STATUS, "Calling xine_event_new_queue" );
	{
		PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
		m_pXineStreamEventQueue = xine_event_new_queue( m_pXineStream );
	}
	xine_event_create_listener_thread( m_pXineStreamEventQueue, XineStreamEventListener, this );

	xine_port_send_gui_data( m_pXineVideoOutput, XINE_GUI_SEND_VIDEOWIN_VISIBLE, ( void * ) 0 );
	xine_port_send_gui_data( m_pXineVideoOutput, XINE_GUI_SEND_DRAWABLE_CHANGED, ( void * ) windows[ m_iCurrentWindow ] );
	xine_port_send_gui_data( m_pXineVideoOutput, XINE_GUI_SEND_VIDEOWIN_VISIBLE, ( void * ) 1 );
	
	return true;
}

bool Xine_Stream::CloseMedia()
{
	if ( m_pXineStream )
	{	
		g_pPlutoLogger->Write( LV_STATUS, "Calling xine_stop for stream with id: %d", m_iStreamID );
		xine_stop( m_pXineStream );

		g_pPlutoLogger->Write( LV_STATUS, "Calling xine_close for stream with id: %d", m_iStreamID );
		xine_close( m_pXineStream );
	}
}

bool Xine_Stream::OpenMedia(string fileName, string &sMediaInfo, string sMediaPosition)
{
	if (!m_bInitialized)
	{
		g_pPlutoLogger->Write( LV_WARNING, "Open media called on non-initialized stream - aborting command");
		return false;
	}
	
	g_pPlutoLogger->Write( LV_STATUS, "Attempting to open media for %s (%s)", fileName.c_str(), sMediaPosition.c_str() );
	
	g_pPlutoLogger->Write( LV_STATUS, "Calling xine_open" );
	
	{
		PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
		m_bIsVDR = fileName.substr( 0, 4 ) == "vdr:";
	}
	
	// getting possible suffix for media: chapters and titles
	string sURLsuffix;
	CalculatePosition( sMediaPosition, &sURLsuffix, NULL, NULL, NULL); 
	
	bool mediaOpened = false;
	
	if (sURLsuffix!="")
	{
		g_pPlutoLogger->Write(LV_WARNING, "Opening media with chapters/title position: %s ", (fileName+sURLsuffix).c_str() );
		mediaOpened = xine_open( m_pXineStream, (fileName+sURLsuffix).c_str() );
		if (!mediaOpened)
			g_pPlutoLogger->Write(LV_WARNING, "Opening media FAILED");
	}
	
	if (!mediaOpened)
	{
		g_pPlutoLogger->Write(LV_WARNING, "Opening media without chapters/title position: %s ", fileName.c_str() );
		mediaOpened = xine_open( m_pXineStream, fileName.c_str() );
		if (!mediaOpened)
			g_pPlutoLogger->Write(LV_WARNING, "Opening media FAILED");
	}
	
	
	setDebuggingLevel(true );	
	
	// opening media
	if ( mediaOpened )
	{
		g_pPlutoLogger->Write( LV_STATUS, "Media opened " );

		setDebuggingLevel( false );
		{
			PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
			m_bHasVideo = xine_get_stream_info( m_pXineStream, XINE_STREAM_INFO_HAS_VIDEO );

			if ( m_iImgWidth == 0 ) m_iImgWidth++;
			if ( m_iImgHeight == 0 ) m_iImgHeight++;
		}

		// depending on video availability, enabling deinterlacing plugin or visualizing
		if ( m_bHasVideo )
		{
			g_pPlutoLogger->Write( LV_STATUS, "Media has video - enabling deinterlacing plugin" );
			EnableDeinterlacing();
		}
		else
		{
			g_pPlutoLogger->Write( LV_STATUS, "Media doesn't have video -  enabling visualizing plugin" );
			
			if ( EnableVisualizing() )
			{
				g_pPlutoLogger->Write( LV_STATUS, "Visualizing plugin enabled" );
				PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
				m_iImgWidth = 100;
				m_iImgHeight = 100;
			}
			else
			{
				g_pPlutoLogger->Write( LV_WARNING, "Visualizing plugin not enabled" );
			}
		}
		
		// reporting about image
		g_pPlutoLogger->Write( LV_STATUS, "Got image dimensions: %dx%d", m_iImgWidth, m_iImgWidth );

		xine_port_send_gui_data( m_pXineVideoOutput, XINE_GUI_SEND_DRAWABLE_CHANGED, ( void * ) windows[ m_iCurrentWindow ] );
		xine_port_send_gui_data( m_pXineVideoOutput, XINE_GUI_SEND_VIDEOWIN_VISIBLE, ( void * ) 1 );
		
		{
			PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
			m_bIsRendering = true;
			m_bExitThread = false;
		}
		
		
		{
			PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
			if (!threadEventLoop)
			{
				g_pPlutoLogger->Write( LV_STATUS, "Creating event processor" );
				pthread_create( &threadEventLoop, NULL, EventProcessingLoop, this );
				g_pPlutoLogger->Write( LV_STATUS, "Event processor started" );
			}
		}

		if ( ! m_bHasVideo )
		{
			g_pPlutoLogger->Write( LV_STATUS, "Stream is seekable: %d", xine_get_stream_info( m_pXineStream, XINE_STREAM_INFO_SEEKABLE ) );
			xine_set_param( m_pXineStream, XINE_PARAM_SPEED, XINE_SPEED_PAUSE );
		}

	}
	else
	{
		g_pPlutoLogger->Write( LV_WARNING, "Xine_Stream::OpenMedia failed! Aborting!" );
		return false;
	}

	// reading information about media
	sMediaInfo = "";
	int titlesCount = xine_get_stream_info(m_pXineStream, XINE_STREAM_INFO_DVD_TITLE_COUNT);
	g_pPlutoLogger->Write( LV_STATUS, "Stream titles count: %d", titlesCount);
	int hasChapters = xine_get_stream_info(m_pXineStream, XINE_STREAM_INFO_HAS_CHAPTERS);
	g_pPlutoLogger->Write( LV_STATUS, "Stream has chapters: %d", hasChapters);
	
	if (!hasChapters)
	{
		//simply enumerating titles count
		for (int i=1; i<=titlesCount; i++)
		{
			sMediaInfo += "Track " + StringUtils::itos(i) + "\t" + StringUtils::itos(i) + "\n";
		}
	}
	else
	{
		/*
		int chaptersCount = xine_get_stream_info(m_pXineStream, XINE_STREAM_INFO_DVD_CHAPTER_COUNT);
		g_pPlutoLogger->Write( LV_STATUS, "Stream chapters count: %d", chaptersCount);*/
		
		//also simply enumerating as we can't find the number of chapters using the current xine api
		for (int i=1; i<=titlesCount; i++)
		{
			sMediaInfo += "Title " + StringUtils::itos(i) + "\t\t" + StringUtils::itos(i) + "\n";
		}		
	}
	
	g_pPlutoLogger->Write( LV_STATUS, "Stream media info: %s", sMediaInfo.c_str());
	
	return true;
}

bool Xine_Stream::EnableDeinterlacing()
{
	if (!m_pXineDeinterlacePlugin)
	{
		// we need a NULL-terminated list of output ports
		xine_video_port_t **volist;
		volist = (xine_video_port_t **)malloc(2 * sizeof(xine_video_port_t*));
		
		volist[0] = m_pXineVideoOutput;
		volist[1] = NULL;

		{
			PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
			m_pXineDeinterlacePlugin = xine_post_init(m_pXineLibrary, "tvtime", 0, NULL, &m_pXineVideoOutput);
		}
		
		free(volist);
	}
	
	bool rewiringResult = false;
	
	if (m_pXineDeinterlacePlugin)
	{
		xine_post_in_t *input_api;
		xine_post_api_t *api;
		xine_post_api_descr_t *desc;
		xine_post_api_parameter_t *parm;
		char *data;
			
		input_api = (xine_post_in_t *)xine_post_input(m_pXineDeinterlacePlugin, "parameters");
		if (!input_api) {
			g_pPlutoLogger->Write( LV_WARNING, "Failed to setup post plugin" );
			return false;
		}
			
		api = (xine_post_api_t *)input_api->data;
		desc = api->get_param_descr();
		parm = desc->parameter;
		data = (char *)malloc(desc->struct_size);
		api->get_parameters(m_pXineDeinterlacePlugin, (void *)data);
			
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
		api->set_parameters(m_pXineDeinterlacePlugin, (void *)data);
		free(data);
		
		rewiringResult = xine_post_wire_video_port(xine_get_video_source(m_pXineStream), m_pXineDeinterlacePlugin->video_input[0]);
		if (!rewiringResult)
		{
			g_pPlutoLogger->Write( LV_WARNING, "post-processing plugin rewiring failed");
		}
		
		return rewiringResult;
	}
	else
	{
		g_pPlutoLogger->Write( LV_WARNING, "tvtime failed to load");
		return false;
	}
}

void Xine_Stream::DisableDeinterlacing()
{
	if (m_pXineDeinterlacePlugin)
	{
		g_pPlutoLogger->Write( LV_STATUS, "Disabling deinterlacing" );
		
		xine_post_wire_video_port( xine_get_video_source( m_pXineStream ), m_pXineVideoOutput );
		xine_post_dispose( m_pXineLibrary, m_pXineDeinterlacePlugin );
		
		PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
		m_pXineDeinterlacePlugin = NULL;
	}
}

bool Xine_Stream::EnableVisualizing()
{
	return false;
	
	{
		PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);	
		if ( ! m_pXineVisualizationPlugin )
		{
			//TODO: NULL-terminated list here
			m_pXineVisualizationPlugin = xine_post_init( m_pXineLibrary, "goom", 1, &m_pXineAudioOutput, &m_pXineVideoOutput );
			if (!m_pXineVisualizationPlugin)
			{
				g_pPlutoLogger->Write( LV_WARNING, "xine_post_init call failed for visualization plugin" );
			}
		}
	}
	
	bool rewiringResult = false;

	if (m_pXineVisualizationPlugin)
	{
		rewiringResult = xine_post_wire_audio_port(xine_get_audio_source(m_pXineStream), m_pXineVisualizationPlugin->audio_input[0]);
		
		if (!rewiringResult)
		{
			g_pPlutoLogger->Write( LV_WARNING, "post-processing plugin rewiring failed");
		}
	}
	
	return rewiringResult;
}

void Xine_Stream::DisableVisualizing()
{
	if ( m_pXineVisualizationPlugin )
	{
		g_pPlutoLogger->Write( LV_STATUS, "Disabling visualization" );
		
		xine_post_wire_audio_port( xine_get_audio_source( m_pXineStream ), m_pXineAudioOutput );
		xine_post_dispose( m_pXineLibrary, m_pXineVisualizationPlugin );
		
		PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
		m_pXineVisualizationPlugin = NULL;
	}
}

void *Xine_Stream::EventProcessingLoop( void *arguments )
{
	Xine_Stream * pStream = ( Xine_Stream* ) arguments;

	Bool checkResult;

	// counter for timecode report
	int iCounter_TimeCode = 0;
	
	// 1/10th second interval counter
	int iCounter = 0, jCounter = 0;
	
	XEvent event;
	while ( ! pStream->m_bExitThread )
	{
		//reading and process X-events
		if ( pStream->m_bIsRendering )
		{
			do
			{
				XLockDisplay( pStream->m_pXDisplay );
				checkResult = XCheckWindowEvent( pStream->m_pXDisplay, pStream->windows[ pStream->m_iCurrentWindow ], INPUT_MOTION, &event );
				XUnlockDisplay( pStream->m_pXDisplay );

				if ( checkResult == True )
					pStream->XServerEventProcessor( event );
			}
			while ( checkResult == True );
		}

		// updating every second - position
		if ( iCounter++ > 10 )
		{
			g_pPlutoLogger->Write( LV_WARNING, "%s (seek %d) t.c. ctr %d freq %d,", pStream->GetPosition().c_str(), pStream->m_iSpecialSeekSpeed, iCounter_TimeCode, pStream->m_iTimeCodeReportFrequency );
			iCounter = 0;
			
			//if it is a time - reporting our timecode to player object
			if ( pStream->m_iTimeCodeReportFrequency && ++iCounter_TimeCode >= pStream->m_iTimeCodeReportFrequency )
			{
				pStream->ReportTimecode();
				iCounter_TimeCode = 1;
			}
			
		}
		
		// We need to wait 500ms after the stream starts before doing the seek!
		if ( pStream->m_iSpecialOneTimeSeek )
		{
		 	jCounter++;
			if ( jCounter > 5 ) 
			{
				pStream->Seek(pStream->m_iSpecialOneTimeSeek,10000); // As long as we're within 10 seconds that's fine
				pStream->m_iSpecialOneTimeSeek = 0;
				pStream->ReportTimecode();
				pStream->changePlaybackSpeed( PLAYBACK_NORMAL );
			}
		}
		else
		{
		  	jCounter = 0;
		}
		
		
		//updating time and speed when @trickplay mode
		if (pStream->m_bTrickModeActive)
		{
			//only every 0.2s
			if (iCounter%2==0)
			{
				pStream->DisplaySpeedAndTimeCode();
			}
		}
		
		if ( pStream->m_iSpecialSeekSpeed )
			pStream->HandleSpecialSeekSpeed();

		usleep( 100000 );
	}

	return NULL;
}

int Xine_Stream::XServerEventProcessor(XEvent &event )
{
	if (!m_bInitialized)
	{
		g_pPlutoLogger->Write( LV_WARNING, "XServerEventProcessor called on non-initialized stream - aborting command");
		return false;
	}	
	
	Atom XA_DELETE_WINDOW;

	switch ( event.type )
	{
		case ClientMessage:
		{
			XA_DELETE_WINDOW = XInternAtom( m_pXDisplay, "WM_DELETE_WINDOW", False );

			if ( ( unsigned ) event.xclient.data.l[ 0 ] == XA_DELETE_WINDOW )
				m_bIsRendering = false;
			break;
		}

		case MotionNotify:
		{
			XMotionEvent *mevent = ( XMotionEvent * ) & event;
			int x, y;

			if ( translate_point( mevent->x, mevent->y, &x, &y ) )
			{
				xine_event_t xineEvent;
				xine_input_data_t xineInput;

				xineEvent.type = XINE_EVENT_INPUT_MOUSE_MOVE;
				xineEvent.stream = m_pXineStream;
				xineEvent.data = &xineInput;
				xineEvent.data_length = sizeof( xineInput );
				gettimeofday( &xineEvent.tv, NULL );

				xineInput.button = 0;
				xineInput.x = x;
				xineInput.y = y;
				xine_event_send( m_pXineStream, &xineEvent );
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

			xine_gui_send_vo_data ( m_pXineStream, XINE_GUI_SEND_TRANSLATE_GUI_TO_VIDEO, ( void* ) & rect );
			g_pPlutoLogger->Write( LV_STATUS, "Xine player: mouse button event after translation: mx=%d my=%d", rect.x, rect.y );

			xineEvent.stream = m_pXineStream;
			xineEvent.type = XINE_EVENT_INPUT_MOUSE_BUTTON;
			xineEvent.data = &xineInputData;
			xineEvent.data_length = sizeof( xineInputData );

			xineInputData.button = bevent->button;
			xineInputData.x = rect.x;
			xineInputData.y = rect.y;

			gettimeofday( &xineEvent.tv, NULL );
			XLockDisplay( bevent->display );
			xine_event_send( m_pXineStream, &xineEvent );
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
					g_pPlutoLogger->Write( LV_STATUS, "Xine player: key press event: XK_Up");
					break;
				case XK_Down:
					xineEvent.type = XINE_EVENT_INPUT_DOWN;
					g_pPlutoLogger->Write( LV_STATUS, "Xine player: key press event: XK_Down");
					break;
				case XK_Left:
					xineEvent.type = XINE_EVENT_INPUT_LEFT;
					g_pPlutoLogger->Write( LV_STATUS, "Xine player: key press event: XK_Left");
					break;
				case XK_Right:
					xineEvent.type = XINE_EVENT_INPUT_RIGHT;
					g_pPlutoLogger->Write( LV_STATUS, "Xine player: key press event: XK_Right");
					break;
				case XK_Return:
					g_pPlutoLogger->Write( LV_STATUS, "Xine player: key press event: XK_Return");
					xineEvent.type = XINE_EVENT_INPUT_SELECT;
					break;
				default:
					g_pPlutoLogger->Write( LV_STATUS, "Xine player: key press event: not in (Up, Down, Left, Right, Return)");
					xineEvent.type = 0;
			}
			xineEvent.stream = m_pXineStream;
			xineEvent.data = NULL;
			xineEvent.data_length = 0;
			gettimeofday( &xineEvent.tv, NULL );
			xine_event_send( m_pXineStream, &xineEvent );
			break;
		}

		case Expose:
		{
			XExposeEvent *exposeEvent = ( XExposeEvent * ) & event;
                // g_pPlutoLogger->Write(LV_STATUS, "Expose with count %d", exposeEvent->count);

			if ( exposeEvent->count != 0 )
				break;

			if ( m_pXineVideoOutput )
				xine_port_send_gui_data( m_pXineVideoOutput, XINE_GUI_SEND_EXPOSE_EVENT, exposeEvent );

			break;
		}

		case ConfigureNotify:
		{
			XConfigureEvent *cev = ( XConfigureEvent * ) & event;
			Window tmp_win;

			m_iImgWidth = cev->width;
			m_iImgHeight = cev->height;

			if ( ( cev->x == 0 ) && ( cev->y == 0 ) )
			{
				XLockDisplay( cev->display );
				XTranslateCoordinates( cev->display, cev->window, DefaultRootWindow( cev->display ), 0, 0, &m_iImgXPos, &m_iImgYPos, &tmp_win );
				XUnlockDisplay( cev->display );
			}
			else
			{
				m_iImgXPos = cev->x;
				m_iImgYPos = cev->y;
			}
		}
		break;
	}

	return true;
}

void Xine_Stream::Seek(int pos,int tolerance_ms)
{
	if (!m_bInitialized)
	{
		g_pPlutoLogger->Write( LV_WARNING, "Seek called on non-initialized stream - aborting command");
		return;
	}

	
	if( tolerance_ms==0 )
	{
		timespec ts1,ts2,tsElapsed;
		gettimeofday( &ts1, NULL );

		xine_seek( m_pXineStream, 0, pos );

		gettimeofday( &ts2, NULL );
		tsElapsed = ts2-ts1;
		int positionTime, totalTime;
		getStreamPlaybackPosition( positionTime, totalTime );
		g_pPlutoLogger->Write(LV_STATUS,"Seek took %d ms.  Tried for pos %d landed at %d, off by %d",
													tsElapsed.tv_sec * 1000 + tsElapsed.tv_nsec / 1000000,
													pos,positionTime,positionTime-pos);
		return ;
	}

	g_pPlutoLogger->Write( LV_WARNING, "Xine_Stream::Seek seek to %d tolerance %d", pos, tolerance_ms );

	for ( int i = 0;i < 10;++i )
	{
		int positionTime, totalTime;
		if ( abs( getStreamPlaybackPosition( positionTime, totalTime ) - pos ) < tolerance_ms )
		{
			g_pPlutoLogger->Write( LV_WARNING, "Xine_Stream::Seek Close enough %d %d total %d", positionTime, pos, totalTime );
			break;
		}
		else
		{
			g_pPlutoLogger->Write( LV_WARNING, "Xine_Stream::Seek get closer currently at: %d target pos: %d ctr %d", positionTime, pos, i );
			xine_seek( m_pXineStream, 0, pos );
		}
	}
}

void Xine_Stream::HandleSpecialSeekSpeed()
{
	if ( m_tsLastSpecialSeek.tv_sec==0 )  // Should not happen
		return ;

	if (!m_bInitialized)
	{
		g_pPlutoLogger->Write( LV_WARNING, "HandleSpecialSeek called on non-initialized stream - aborting command");
		return;
	}
	
	DisplaySpeedAndTimeCode();

	timespec ts;
	gettimeofday( &ts, NULL );

	timespec tsElapsed = ts-m_tsLastSpecialSeek;
	int msElapsed = tsElapsed.tv_sec * 1000 + tsElapsed.tv_nsec / 1000000;
	int seekTime = m_posLastSpecialSeek + (msElapsed * m_iSpecialSeekSpeed / 1000);  // Take the time that did elapse, factor the speed difference, and add it to the last seek
	int positionTime, totalTime;
	getStreamPlaybackPosition( positionTime, totalTime );

	g_pPlutoLogger->Write(LV_STATUS,"HandleSpecialSeekSpeed %d elapsed: %d ms last: %d this: %d pos %d",
												m_iSpecialSeekSpeed, msElapsed,
												m_posLastSpecialSeek,seekTime,positionTime);

	if ( seekTime < 0 || seekTime > totalTime )
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "aborting seek" );
		StopSpecialSeek();
		ReportTimecode();
		return;
	}

	PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
	m_tsLastSpecialSeek=ts;
	m_posLastSpecialSeek=seekTime;
	Seek(seekTime,0);
}

int Xine_Stream::getStreamPlaybackPosition( int &positionTime, int &totalTime )
{
	if ( m_pDynamic_Pointer )
		m_pDynamic_Pointer->pointer_check_time();

	if (!m_bInitialized)
	{
		g_pPlutoLogger->Write( LV_WARNING, "getStreamPlaybackPosition called on non-initialized stream - aborting command");
		return false;
	}

	
	if ( xine_get_stream_info( m_pXineStream, XINE_STREAM_INFO_SEEKABLE ) == 0 )
	{
		g_pPlutoLogger->Write( LV_STATUS, "Stream is not seekable" );
		positionTime = totalTime = 0;
		return 0;
	}

	int iPosStream = 0;
	int iPosTime = 0;
	int iLengthTime = 0;

	int count = 10;
	while ( --count && ! xine_get_pos_length( m_pXineStream, &iPosStream, &iPosTime, &iLengthTime ) )
	{
		g_pPlutoLogger->Write( LV_STATUS, "Error reading stream position: %d", xine_get_error( m_pXineStream ) );
		Sleep( 30 );
	}

	positionTime = iPosTime;
	totalTime = iLengthTime;
	return positionTime;
}

void Xine_Stream::DisplaySpeedAndTimeCode()
{

	if (!m_bInitialized)
	{
		g_pPlutoLogger->Write( LV_WARNING, "DisplaySpeedAndTimeCode called on non-initialized stream - aborting command");
		return;
	}

	int iDirection = m_iSpecialSeekSpeed<0?-1:1;
		
	int Whole = iDirection*m_iSpecialSeekSpeed / 1000;
	int Fraction = iDirection*m_iSpecialSeekSpeed % 1000;
	
	if (m_bTrickModeActive)
	{
		iDirection = 1;
		Whole = m_iTrickPlaySpeed / 1000;
		Fraction = m_iTrickPlaySpeed % 1000;
	}
	
	string sSpeed;

	if ( iDirection < 0 )
	{
		sSpeed += "-";
	}
	
	if ( Whole )
		sSpeed += StringUtils::itos( Whole );
	else
		sSpeed += "0";

	if ( Fraction )
		sSpeed += "." + StringUtils::itos( Fraction );

	sSpeed += "x     ";

	int seconds, totalTime;
	getStreamPlaybackPosition( seconds, totalTime );
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

	if ( (( m_iSpecialSeekSpeed == 0 )&&!m_bTrickModeActive) || ( seconds_only == 1 ) )
		DisplayOSDText("");
	else
		DisplayOSDText( sSpeed );
}


void Xine_Stream::DisplayOSDText( string sText )
{
	if (!m_bInitialized)
	{
		g_pPlutoLogger->Write( LV_WARNING, "DisplayOSDText called on non-initialized stream - aborting command");
		return;
	}

	
	if ( sText.size() == 0 )
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "Clearing OSD %p", m_xine_osd_t );
		
		PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
		if ( m_xine_osd_t )
		{
			xine_osd_free( m_xine_osd_t );
			m_xine_osd_t = NULL;
		}
		return ;
	}

	PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
	
	if ( m_xine_osd_t )
	{
		xine_osd_free( m_xine_osd_t );
	}
	
	m_xine_osd_t = xine_osd_new( m_pXineStream, 0, 0, 1000, 100 );
	xine_osd_set_font( m_xine_osd_t, "sans", 20 );
	xine_osd_set_text_palette( m_xine_osd_t, XINE_TEXTPALETTE_WHITE_BLACK_TRANSPARENT, XINE_OSD_TEXT1 );
	xine_osd_draw_rect( m_xine_osd_t, 0, 0, 999, 99, XINE_OSD_TEXT1, 1 );
	xine_osd_draw_text( m_xine_osd_t, 20, 20, sText.c_str(), XINE_OSD_TEXT1 );
	xine_osd_show( m_xine_osd_t, 0 );

	g_pPlutoLogger->Write( LV_WARNING, "DisplayOSDText() : Attempting to display %s", sText.c_str() );
}

void Xine_Stream::StartSpecialSeek( int Speed )
{	
	if (!m_bInitialized)
	{
		g_pPlutoLogger->Write( LV_WARNING, "StartSpecialSeek called on non-initialized stream - aborting command");
		return;
	}

	
	xine_set_param(m_pXineStream, XINE_PARAM_IGNORE_AUDIO, 1);
	
	int totalTime;
	gettimeofday( &m_tsLastSpecialSeek, NULL );
	getStreamPlaybackPosition( m_posLastSpecialSeek, totalTime );

	g_pPlutoLogger->Write( LV_STATUS, "Starting special seek %d", Speed );
	
	{
		PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
		m_iPrebuffer = xine_get_param( m_pXineStream, XINE_PARAM_METRONOM_PREBUFFER );
	}
	
	xine_set_param( m_pXineStream, XINE_PARAM_METRONOM_PREBUFFER, 9000 );
	
	{
		m_iSpecialSeekSpeed = Speed;
	}
//	m_iPlaybackSpeed = PLAYBACK_NORMAL;
	DisplaySpeedAndTimeCode();
	g_pPlutoLogger->Write( LV_STATUS, "done Starting special seek %d", Speed );
	/*	
	g_pPlutoLogger->Write( LV_STATUS, "V0: %d",  m_iSpecialSeekSpeed);
	g_pPlutoLogger->Write( LV_STATUS, "V1: %d",  xine_get_param( m_pXineStream, XINE_PARAM_SPEED));
	g_pPlutoLogger->Write( LV_STATUS, "V2: %d",  xine_get_param( m_pXineStream, XINE_PARAM_METRONOM_PREBUFFER));
	*/
}

void Xine_Stream::StopSpecialSeek()
{
	if (!m_bInitialized)
	{
		g_pPlutoLogger->Write( LV_WARNING, "StopSpecialSeek called on non-initialized stream - aborting command");
		return;
	}

	
	if (!m_iSpecialSeekSpeed)
		return;
	
	xine_set_param(m_pXineStream, XINE_PARAM_IGNORE_AUDIO, 0);
	
	g_pPlutoLogger->Write( LV_STATUS, "Stopping special seek" );
	{
		m_iSpecialSeekSpeed = 0;
	}
	//m_iPlaybackSpeed = PLAYBACK_NORMAL;
	DisplayOSDText("");
	xine_set_param( m_pXineStream, XINE_PARAM_METRONOM_PREBUFFER, m_iPrebuffer );
	g_pPlutoLogger->Write( LV_STATUS, "done Stopping special seek" );
}


bool Xine_Stream::setSubtitle( int Value )
{
	PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
	
	if (!m_bInitialized)
	{
		g_pPlutoLogger->Write( LV_WARNING, "SetSubtitle called on non-initialized stream - aborting command");
		return false;
	}

	
	g_pPlutoLogger->Write( LV_STATUS, "SPU was %d now %d", getSubtitle(), Value );
	xine_set_param( m_pXineStream, XINE_PARAM_SPU_CHANNEL, Value );

	return true;
}

bool Xine_Stream::setAudio( int Value )
{
	PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
	
	if (!m_bInitialized)
	{
		g_pPlutoLogger->Write( LV_WARNING, "SetAudio called on non-initialized stream - aborting command");
		return false;
	}

	
	g_pPlutoLogger->Write( LV_STATUS, "AUDIO was %d now %d", xine_get_param ( m_pXineStream, XINE_PARAM_AUDIO_CHANNEL_LOGICAL ), Value );
	xine_set_param( m_pXineStream, XINE_PARAM_AUDIO_CHANNEL_LOGICAL, Value );

	return true;
}

int Xine_Stream::getSubtitle()
{
	PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
	
	if (!m_bInitialized)
	{
		g_pPlutoLogger->Write( LV_WARNING, "getSubtitle called on non-initialized stream - aborting command");
		return false;
	}

	
	return xine_get_param ( m_pXineStream, XINE_PARAM_SPU_CHANNEL );
}

int Xine_Stream::getAudio()
{
	PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
	
	if (!m_bInitialized)
	{
		g_pPlutoLogger->Write( LV_WARNING, "getAudio called on non-initialized stream - aborting command");
		return false;
	}
	
	return xine_get_param ( m_pXineStream, XINE_PARAM_AUDIO_CHANNEL_LOGICAL );
}

const char *Xine_Stream::TranslateLanguage( const char *abbreviation )
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

void Xine_Stream::XineStreamEventListener( void *streamObject, const xine_event_t *event )
{
	Xine_Stream * pXineStream = ( Xine_Stream * ) streamObject;
	
	if (!pXineStream->m_bInitialized)
	{
		g_pPlutoLogger->Write( LV_WARNING, "XineStreamEventListener called on non-initialized stream - aborting command");
		return;
	}


	switch ( event->type )
	{
		case XINE_EVENT_UI_PLAYBACK_FINISHED:
			g_pPlutoLogger->Write( LV_STATUS, "Got XINE_EVENT_UI_PLAYBACK_FINISHED" );
			pXineStream->StopSpecialSeek();
			pXineStream->ReportTimecode();
			pXineStream->playbackCompleted( false );
			{			
				PLUTO_SAFETY_LOCK(streamLock, pXineStream->m_streamMutex);
				pXineStream->m_bIsRendering = false;
			}
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
			pXineStream->StopSpecialSeek();
			pXineStream->ReportTimecode();
			int iButtons = ( ( xine_ui_data_t * ) event->data ) ->num_buttons;

			g_pPlutoLogger->Write( LV_STATUS, "Menu with %d buttons", iButtons );
									
			pXineStream->FireMenuOnScreen(iButtons);
			
		}
		break;

		case XINE_EVENT_UI_SET_TITLE:
			if ( pXineStream->m_iSpecialSeekSpeed )
				break; // Ignore this while we're doing all those seeks
			{
				PLUTO_SAFETY_LOCK(streamLock, pXineStream->m_streamMutex);

				xine_ui_data_t *data = ( xine_ui_data_t * ) event->data;
				
				g_pPlutoLogger->Write( LV_STATUS, "UI set title: %s %s", data->str, pXineStream->GetPosition().c_str() );
                		//UI set title: Title 58, Chapter 1,
				const char *p = strstr( data->str, "Title " );
				if ( p )
					pXineStream->m_iTitle = atoi( p + 6 );

				p = strstr( data->str, "Chapter " );
				if ( p )
					pXineStream->m_iChapter = atoi( p + 8 );
			}
			break;

		case XINE_EVENT_INPUT_MOUSE_MOVE:
			if ( pXineStream->m_pDynamic_Pointer )
				pXineStream->m_pDynamic_Pointer->pointer_show();
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
			if ( pXineStream->m_iSpecialSeekSpeed )
				break; // Ignore this while we're doing all those seeks
			{
				int numaudio = xine_get_stream_info( pXineStream->m_pXineStream, XINE_STREAM_INFO_MAX_AUDIO_CHANNEL );
				if ( numaudio > 1 )
				{
					string sAudioTracks = StringUtils::itos( pXineStream->getAudio() ) + "\n";
					int iResult = 1, i;
					for ( i = 0;i < 100;++i )
					{
						char lang[ XINE_LANG_MAX ];
						strcpy( lang, "**error**" );
						iResult = xine_get_audio_lang( pXineStream->m_pXineStream, i, lang );
						if ( iResult == 0 )
							break;
						sAudioTracks += string( pXineStream->TranslateLanguage( lang ) ) + "\n";
					}
					if ( i >= 100 )
						g_pPlutoLogger->Write( LV_CRITICAL, "Something went wrong audio tracks>100" );
					
					pXineStream->m_pFactory->ReportAudioTracks( sAudioTracks );
				}
				else
					g_pPlutoLogger->Write( LV_STATUS, "Ignoring XINE_EVENT_UI_CHANNELS_CHANGED since there aren't multiple audio tracks" );

				int numsubtitle = xine_get_stream_info( pXineStream->m_pXineStream, XINE_STREAM_INFO_MAX_SPU_CHANNEL );
				if ( numsubtitle > 1 )
				{
					string sSubtitles = StringUtils::itos( pXineStream->getSubtitle() ) + "\n";
					int iResult = 1, i;
					for ( i = 0;i < 100;++i )
					{
						char lang[ XINE_LANG_MAX ];
						strcpy( lang, "**error**" );
						iResult = xine_get_spu_lang( pXineStream->m_pXineStream, i, lang );
						if ( iResult == 0 )
							break;
						sSubtitles += string( pXineStream->TranslateLanguage( lang ) ) + "\n";
					}
					if ( i >= 100 )
						g_pPlutoLogger->Write( LV_CRITICAL, "Something went wrong subitltes>100" );
					
					pXineStream->m_pFactory->ReportSubtitles( sSubtitles );
				}
				else
					g_pPlutoLogger->Write( LV_STATUS, "Ignoring XINE_EVENT_UI_CHANNELS_CHANGED since there aren't multiple subtitles" );
			}
			break;

		default:
			g_pPlutoLogger->Write( LV_STATUS, "Got unprocessed Xine playback event: %d", event->type );
			break;
	}

	if ( pXineStream->m_pDynamic_Pointer )
		pXineStream->m_pDynamic_Pointer->pointer_check_time();
}

void Xine_Stream::selectNextButton()
{
	if (!m_bInitialized)
	{
		g_pPlutoLogger->Write( LV_WARNING, "SelectNextButton called on non-initialized stream - aborting command");
		return;
	}

	
	g_pPlutoLogger->Write( LV_STATUS, "Selecting next hot spot on the m_pstream %d", m_iStreamID );

	xine_event_t event;

	event.type = XINE_EVENT_INPUT_UP;
	event.stream = m_pXineStream;
	event.data = NULL;
	event.data_length = 0;
	gettimeofday( &event.tv, NULL );

	xine_event_send( m_pXineStream, &event );
}

void Xine_Stream::selectPrevButton()
{
	if (!m_bInitialized)
	{
		g_pPlutoLogger->Write( LV_WARNING, "Select Prev Button called on non-initialized stream - aborting command");
		return;
	}

	
	g_pPlutoLogger->Write( LV_STATUS, "Selecting next hot spot on the m_pstream %d", m_iStreamID );

	xine_event_t event;

	event.type = XINE_EVENT_INPUT_DOWN;
	event.stream = m_pXineStream;
	event.data = NULL;
	event.data_length = 0;
	gettimeofday( &event.tv, NULL );

	xine_event_send( m_pXineStream, &event );
}

void Xine_Stream::pushCurrentButton()
{
	if (!m_bInitialized)
	{
		g_pPlutoLogger->Write( LV_WARNING, "PushCurrentButton called on non-initialized stream - aborting command");
		return;
	}

	
	g_pPlutoLogger->Write( LV_STATUS, "Selecting next hot spot on the m_pstream %d", m_iStreamID );

	xine_event_t event;

	event.type = XINE_EVENT_INPUT_SELECT;
	event.stream = m_pXineStream;
	event.data = NULL;
	event.data_length = 0;
	gettimeofday( &event.tv, NULL );

	xine_event_send( m_pXineStream, &event );
}

bool Xine_Stream::playStream( string mediaPosition)
{
	StopSpecialSeek();

	if (!m_bInitialized)
	{
		g_pPlutoLogger->Write( LV_WARNING, "playStream called on non-initialized stream - aborting command");
		return false;
	}

	{
		PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
		m_iPlaybackSpeed = PLAYBACK_NORMAL;
	}
	time_t startTime = time( NULL );

	int Subtitle = -2, Angle = -2, AudioTrack = -2;
	
	int pos = CalculatePosition( mediaPosition, NULL, &Subtitle, &Angle, &AudioTrack );

	if ( xine_play( m_pXineStream, 0, 0) )
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

/*
		if ( playbackStopped )
			xine_set_param( m_pXineStream, XINE_PARAM_SPEED, XINE_SPEED_PAUSE );
		else
			xine_set_param( m_pXineStream, XINE_PARAM_SPEED, XINE_SPEED_NORMAL );
*/
		
		ReportTimecode();

		return true;
	}
	else
	{
		g_pPlutoLogger->Write( LV_WARNING, "Play failed with error %d", xine_get_error( m_pXineStream ) );
		return false;
	}
}

void Xine_Stream::changePlaybackSpeed( PlayBackSpeedType desiredSpeed )
{
	g_pPlutoLogger->Write(LV_STATUS,"Xine_Stream::changePlaybackSpeed speed %d",(int) desiredSpeed);
	
	if (!m_bInitialized)
	{
		g_pPlutoLogger->Write( LV_WARNING, "changePlayback speed called on non-initialized stream - aborting command");
		return;
	}

	
	//TODO: initialize correctly
	// bool trickModeSupported = xine_get_stream_info(m_pXineStream, XINE_STREAM_INFO_TRICK_PLAY_SUPPORTED);
	// bool trickModeActive = xine_get_stream_info(m_pXineStream, XINE_STREAM_INFO_TRICK_PLAY_ENABLED) ;
	bool trickModeSupported = (strncasecmp("dvd:", m_sCurrentFile.c_str(), 4)==0);
	bool trickModeActive = m_bTrickModeActive;
	
	
	// there few possible modes between which we have to switch correctly:
	// normal play
	// trick play
	// special seek
	{
		PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);	
		m_iPlaybackSpeed = desiredSpeed;
	}
	
	// pause or normal playback requested - stopping any special mode and playing as usually	
	if ((desiredSpeed == PLAYBACK_FF_1)||(desiredSpeed == PLAYBACK_STOP) )
	{
		g_pPlutoLogger->Write(LV_STATUS,"Xine_Stream::changePlaybackSpeed stopping any seeker");
		
		if (m_iSpecialSeekSpeed)
		{
			StopSpecialSeek();
		} 
		else 
		if ( trickModeSupported &&  trickModeActive )
		{
			g_pPlutoLogger->Write(LV_STATUS,"Xine_Stream::changePlaybackSpeed stopping trick play");
			
			xine_stop_trick_play(m_pXineStream);
			xine_set_param( m_pXineStream, XINE_PARAM_METRONOM_PREBUFFER, m_iPrebuffer );
			EnableDeinterlacing();
			PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);	
			m_iTrickPlaySpeed = 0;
			m_bTrickModeActive = false;
		}
		else
			g_pPlutoLogger->Write(LV_WARNING,"Xine_Stream::changePlaybackSpeed no running seekers found");

		
		xine_set_param( m_pXineStream, XINE_PARAM_SPEED, (desiredSpeed == PLAYBACK_FF_1)?XINE_SPEED_NORMAL:XINE_SPEED_PAUSE );
		
		DisplayOSDText( "" );
		
		return;
	}
	
	
	// reverse play requested or trick_play is not supported for this stream
	// or we need ultra-fast-forward (>16x)
	const int UltraFastFWD = 16*1000;
	if ( (desiredSpeed < 0)||( (desiredSpeed > 0) && !trickModeSupported )||(desiredSpeed>UltraFastFWD) )
	{	
		if ( trickModeSupported &&  trickModeActive )
		{
			g_pPlutoLogger->Write(LV_STATUS,"Xine_Stream::changePlaybackSpeed stopping trick play");	
			xine_stop_trick_play(m_pXineStream);
			xine_set_param( m_pXineStream, XINE_PARAM_METRONOM_PREBUFFER, m_iPrebuffer );
			EnableDeinterlacing();
			PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);	
			m_iTrickPlaySpeed = 0;
			m_bTrickModeActive = false;
		}
		
		if (m_iSpecialSeekSpeed==0)
		{
			StartSpecialSeek( desiredSpeed );
		}
		else
		{
			g_pPlutoLogger->Write(LV_STATUS,"Xine_Stream::changePlaybackSpeed changing special seek speed");
			PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);	
			m_iSpecialSeekSpeed = desiredSpeed;
		}
		
		return;
	}
	
	
	// trick play is supported and  desired speed is ok
	if ((desiredSpeed > 0) && trickModeSupported)
	{
		if (m_iSpecialSeekSpeed)
		{
			StopSpecialSeek();
		} 
		
		g_pPlutoLogger->Write(LV_STATUS,"Xine_Stream::changePlaybackSpeed starting trick play");

		// are we doing restart
		if (!m_bTrickModeActive)
		{
			PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);	
			m_iPrebuffer = xine_get_param( m_pXineStream, XINE_PARAM_METRONOM_PREBUFFER);
			xine_set_param( m_pXineStream, XINE_PARAM_METRONOM_PREBUFFER, 9000 );
			DisableDeinterlacing();
		}
		else
		{
			xine_stop_trick_play(m_pXineStream);	
		}
		
		xine_start_trick_play(m_pXineStream, desiredSpeed*1000);
		PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);	
		m_iTrickPlaySpeed = desiredSpeed;
		m_bTrickModeActive = true;
		
		return;
	}
	
	g_pPlutoLogger->Write(LV_WARNING,"Xine_Stream::changePlaybackSpeed IMPOSSIBLE_STATE");
}

Xine_Stream::PlayBackSpeedType Xine_Stream::getPlaybackSpeed()
{
	/*!
	XineStream * pStream;

	if ( ( pStream = getStreamForId( iStreamID, "Can't get the speed of a non existent stream (%d)!" ) ) == NULL )
		return PLAYBACK_STOP;
*/
	
	if (!m_bInitialized)
	{
		g_pPlutoLogger->Write( LV_WARNING, "getPlaybackSpeed called on non-initialized stream - aborting command");
		return PLAYBACK_STOP;
	}

	
	int currentSpeed;
	switch ( ( currentSpeed = xine_get_param( m_pXineStream, XINE_PARAM_SPEED ) ) )
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

bool Xine_Stream::DestroyWindows()
{
	if ( m_pXDisplay != NULL )
	{
		XLockDisplay( m_pXDisplay );

		XUnmapWindow( m_pXDisplay, windows[ m_iCurrentWindow ] );

		XFreeCursor( m_pXDisplay, cursors[ 0 ] );
		XFreeCursor( m_pXDisplay, cursors[ 1 ] );

		XDestroyWindow( m_pXDisplay, windows[ 0 ] );
		XDestroyWindow( m_pXDisplay, windows[ 1 ] );

		XUnlockDisplay( m_pXDisplay );
		XCloseDisplay ( m_pXDisplay );

		m_pXDisplay = NULL;

		if (!m_pDynamic_Pointer)
			delete m_pDynamic_Pointer;
		m_pDynamic_Pointer = NULL;
	}

	return true;
}

void Xine_Stream::make_snapshot( string sFormat, int iWidth, int iHeight, bool bKeepAspect, char*&pData, int &iDataSize )
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

	/*
	if ( pStream == NULL )
	{
	g_pPlutoLogger->Write( LV_WARNING, "Xine_Stream::make_snapshot(): The xine_stream_t object passed as parameter was null. Can't get the screen shot" );
		return ;
	}
	*/

	g_pPlutoLogger->Write( LV_STATUS, "Xine_Stream::make_snapshot(): Getting frame info" );
	if ( ! xine_get_current_frame( m_pXineStream, &imageWidth, &imageHeight, &imageRatio, &imageFormat, NULL ) )
	{
		g_pPlutoLogger->Write( LV_WARNING, "Xine_Stream::make_snapshot(): Error getting frame info. Returning!" );
		return ;
	}
	g_pPlutoLogger->Write( LV_STATUS, "Xine_Stream::make_snapshot(): Got %d %d %d 0x%x", imageWidth, imageHeight, imageRatio, imageFormat );

	imageData = ( uint8_t* ) malloc ( ( imageWidth + 8 ) * ( imageHeight + 1 ) * 2 );
	g_pPlutoLogger->Write( LV_STATUS, "Xine_Stream::make_snapshot(): Getting frame data" );
	{
		if ( ! xine_get_current_frame ( m_pXineStream, &imageWidth, &imageHeight, &imageRatio, &imageFormat, imageData ) )
		{
			g_pPlutoLogger->Write( LV_WARNING, "Xine_Stream::make_snapshot(): Error getting frame data. Returning!" );
			return ;
		}
	}
	g_pPlutoLogger->Write( LV_STATUS, "Xine_Stream::make_snapshot(): Got %d %d %d 0x%x", imageWidth, imageHeight, imageRatio, imageFormat );

    // we should have the image in YV12 format aici
    // if not then we try to convert it.
	if ( imageFormat == XINE_IMGFMT_YUY2 )
	{
		uint8_t * yuy2Data = imageData;
		imageData = ( uint8_t * ) malloc ( imageWidth * imageHeight * 2 );

		Colorspace_Utils::yuy2toyv12 (
				imageData,
		imageData + imageWidth * imageHeight,
		imageData + imageWidth * imageHeight * 5 / 4, yuy2Data, imageWidth, imageHeight );

		free ( yuy2Data );
	}

	g_pPlutoLogger->Write( LV_STATUS, "Xine_Stream::make_snapshot(): Converted to YV12" );
    // convert to RGB
    // keep the yv12Data array around to be able to delete it
	uint8_t *yv12Data = imageData;

	/** @brief this function will allocate the output parameter */
	imageData = Colorspace_Utils::yv12torgb (
			imageData,
	imageData + imageWidth * imageHeight,
	imageData + imageWidth * imageHeight * 5 / 4,
	imageWidth, imageHeight );
	g_pPlutoLogger->Write( LV_STATUS, "Xine_Stream::make_snapshot(): Converted to RGB!" );
	free( yv12Data );

	double outputRatio;
    // double currentRatio;

	g_pPlutoLogger->Write( LV_STATUS, "Xine_Stream::make_snapshot(): Temp data was freed here!" );
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
						case XINE_VO_ASPECT_DONT_TOUCH:                   /* probably non-mpeg m_m_pXineStream => don't touch aspect ratio */
							outputRatio = ( double ) imageWidth / ( double ) imageHeight;
							break;

		default:
			g_pPlutoLogger->Write( LV_WARNING, "Xine_Stream::make_snapshot(): Unknown aspect ratio for image (%d) using 4:3", imageRatio );

			case XINE_VO_ASPECT_4_3:                          /* 4:3             */
				outputRatio = 4.0 / 3.0;
				break;
	}

	double f = outputRatio / ( ( double ) imageWidth / ( double ) imageHeight );

	g_pPlutoLogger->Write( LV_STATUS, "Xine_Stream::make_snapshot(): The ratio between the current ration and the output ratio is %f", f );
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

	g_pPlutoLogger->Write( LV_STATUS, "Xine_Stream::make_snapshot(): Making jpeg from RGB" );
	JpegEncoderDecoder jpegEncoder;

	jpegEncoder.encodeIntoBuffer( ( char * ) imageData, imageWidth, imageHeight, 3, pData, iDataSize );

	g_pPlutoLogger->Write( LV_STATUS, "Xine_Stream::make_snapshot(): final image size: %d", iDataSize );
	free( imageData );

	FILE * file;
	file = fopen( "/tmp/file.jpg", "wb" );
	g_pPlutoLogger->Write( LV_STATUS, "Xine_Stream::make_snapshot(): temporary image filep %p", file );

	fwrite( pData, iDataSize, 1, file );
	fclose( file );

	g_pPlutoLogger->Write( LV_STATUS, "Xine_Stream::make_snapshot(): At the end. Returning" );
	return ;
}

/**
    \fn Xine_Stream::restartMediaStream(int iStreamID)
 */
void Xine_Stream::restartMediaStream()
{
	if (!m_bInitialized)
	{
		g_pPlutoLogger->Write( LV_WARNING, "restartMediaStream called on non-initialized stream - aborting command");
		return;
	}

	changePlaybackSpeed( PLAYBACK_FF_1 );
}

/**
    \fn Xine_Stream::pauseMediaStream(int iStreamID)
 */
void Xine_Stream::pauseMediaStream( )
{
	if (!m_bInitialized)
	{
		g_pPlutoLogger->Write( LV_WARNING, "pauseMediaStream called on non-initialized stream - aborting command");
		return;
	}
	
	int stoppedTime, completeTime;

	if ( getPlaybackSpeed() == PLAYBACK_STOP )
		changePlaybackSpeed( PLAYBACK_FF_1 );
	else
		changePlaybackSpeed( PLAYBACK_STOP );

	getStreamPlaybackPosition( stoppedTime, completeTime );
	g_pPlutoLogger->Write( LV_STATUS, "Stream paused at time: %d from %d", stoppedTime, completeTime );
}

KeySym Xine_Stream::translatePlutoKeySymToXKeySym( int plutoButton )
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

void Xine_Stream::selectMenu( int iMenuType )
{
	if (!m_bInitialized)
	{
		g_pPlutoLogger->Write( LV_WARNING, "selectMenu called on non-initialized stream - aborting command");
		return;
	}

	
	xine_event_t xine_event;

	xine_event.type = iMenuType + XINE_EVENT_INPUT_MENU1;
	xine_event.data_length = 0;
	xine_event.data = NULL;
	xine_event.stream = m_pXineStream;
	gettimeofday( &xine_event.tv, NULL );

	xine_event_send( m_pXineStream, &xine_event );
}

void Xine_Stream::playbackCompleted( bool bWithErrors )
{
	if (!m_bInitialized)
	{
		g_pPlutoLogger->Write( LV_WARNING, "playbackCompleted called on non-initialized stream - aborting command");
		return;
	}
	
	g_pPlutoLogger->Write( LV_STATUS, "Fire playback completed event %d", ( int ) m_isSlimClient );
	//XineStream * xineStream = getStreamForId( iStreamID, "Can't get the position of a nonexistent stream!" );

	if ( ! m_isSlimClient )
		m_pFactory->m_pPlayer->EVENT_Playback_Completed(m_sCurrentFile,m_iStreamID, bWithErrors );
}

int Xine_Stream::enableBroadcast( int iStreamID )
{
	if (!m_bInitialized)
	{
		g_pPlutoLogger->Write( LV_WARNING, "enableBroadcast called on non-initialized stream - aborting command");
		return false;
	}

	
	int portNumber = 7866;
	if ( portNumber != xine_get_param( m_pXineStream, XINE_PARAM_BROADCASTER_PORT ) )
	{
        //         if( port && xine_get_param(pStream->m_pStream, XINE_PARAM_BROADCASTER_PORT) )
		xine_set_param( m_pXineStream, XINE_PARAM_BROADCASTER_PORT, portNumber );

        /* try up to ten times from port base. sometimes we have trouble
		* binding to the same port we just used.
				*/
		for ( int i = 0; i < 10; i++ )
		{
			xine_set_param( m_pXineStream, XINE_PARAM_BROADCASTER_PORT, ++portNumber );
			if ( portNumber == xine_get_param( m_pXineStream, XINE_PARAM_BROADCASTER_PORT ) )
				return portNumber;
		}
	}

	return 0;
}

void Xine_Stream::simulateMouseClick( int X, int Y )
{
//	XineStream * pStream;
	xine_input_data_t xineInputData;
	xine_event_t xineEvent;

//	if ( ( pStream = getStreamForId( 1, "Xine_Stream::simulateMouseClick() getting one stream" ) ) == NULL )
//		return ;

	g_pPlutoLogger->Write( LV_STATUS, "Xine_Stream::simulateMouseClick(): simulating mouse click: mx=%d my=%d", X, Y );

	if (!m_bInitialized)
	{
		g_pPlutoLogger->Write( LV_WARNING, "simulateMouseClick called on non-initialized stream - aborting command");
		return;
	}

			
	xineEvent.stream = m_pXineStream;
	xineEvent.type = XINE_EVENT_INPUT_MOUSE_BUTTON;
	xineEvent.data = &xineInputData;
	xineEvent.data_length = sizeof( xineInputData );

	xineInputData.button = 1;
	xineInputData.x = X;
	xineInputData.y = Y;

	gettimeofday( &xineEvent.tv, NULL );
	XLockDisplay( m_pXDisplay );
	xine_event_send( m_pXineStream, &xineEvent );
	XUnlockDisplay( m_pXDisplay );
}

void Xine_Stream::simulateKeystroke( int plutoButton )
{
	Window oldWindow;
	int oldRevertBehaviour;
	g_pPlutoLogger->Write( LV_STATUS, "Xine_Stream::simulateKeystroke(): plutoButton=%d", plutoButton );

	XLockDisplay( m_pXDisplay );
	XGetInputFocus( m_pXDisplay, &oldWindow, &oldRevertBehaviour );
	XSetInputFocus( m_pXDisplay, windows[ m_iCurrentWindow ], RevertToParent, CurrentTime );
	XTestFakeKeyEvent( m_pXDisplay, XKeysymToKeycode( m_pXDisplay, translatePlutoKeySymToXKeySym( plutoButton ) ), True, 0 );
	XTestFakeKeyEvent( m_pXDisplay, XKeysymToKeycode( m_pXDisplay, translatePlutoKeySymToXKeySym( plutoButton ) ), False, 0 );

	if ( oldWindow )
		XSetInputFocus( m_pXDisplay, oldWindow, oldRevertBehaviour, CurrentTime );

	XFlush( m_pXDisplay );
	XUnlockDisplay( m_pXDisplay );
}

void Xine_Stream::sendInputEvent( int eventType )
{
	if (!m_bInitialized)
	{
		g_pPlutoLogger->Write( LV_WARNING, "sendInputEvent on non-initialized stream - aborting command");
		return;
	}

	
	//m_iSpecialSeekSpeed = 0;

	time_t startTime = time( NULL );
	//XineStream *pStream;
	//if ( ( pStream = getStreamForId( 1, "void Xine_Stream::sendInputEvent(int eventType) getting one stream" ) ) == NULL )
	//	return ;

	g_pPlutoLogger->Write( LV_STATUS, "Sending %d event to the engine.", eventType );

	xine_event_t event;

	event.type = eventType;
	event.stream = m_pXineStream;
	event.data = NULL;
	event.data_length = 0;
	gettimeofday( &event.tv, NULL );

	xine_event_send( m_pXineStream, &event );
	g_pPlutoLogger->Write( LV_STATUS, "Xine_Stream::sendInputEvent() Sending input event with ID: %d took %d seconds", eventType, time( NULL ) - startTime );
}

/**
 * xine Callbacks.
 */

void Xine_Stream::destinationSizeCallback( void *data, int video_width, int video_height, double video_pixel_aspect,
		int *dest_width, int *dest_height,
		double *dest_pixel_aspect )
{
	Xine_Stream * pStream = ( Xine_Stream* ) data;

    /**
	 * @test
	if( ! m_pStream->m_bIsRendering )
	g_pPlutoLogger->Write(LV_STATUS, "Destination size callback called (not rendering)!");
		 */

	*dest_width = pStream->m_iImgWidth + ( pStream->m_bIsRendering ? 0 : 1 );
	*dest_height = pStream->m_iImgHeight + ( pStream->m_bIsRendering ? 0 : 1 );
	*dest_pixel_aspect = pStream->m_dPixelAspect;
}

void Xine_Stream::frameOutputCallback( void *data, int video_width, int video_height, double video_pixel_aspect,
																						int *dest_x, int *dest_y, int *dest_width, int *dest_height,
																						double *dest_pixel_aspect,
																						int *win_x, int *win_y )
{
	Xine_Stream * pStream = ( Xine_Stream* ) data;

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
	*dest_pixel_aspect = pStream->m_dPixelAspect;
}


bool Xine_Stream::setDebuggingLevel( bool newValue )
{
	//XineStream * xineStream = getStreamForId( m_itreamID, "Trying to set debugging level for and invalid stream: (%d)" );

	//if ( xineStream == NULL )
//		return false;

	if (!m_bInitialized)
	{
		g_pPlutoLogger->Write( LV_WARNING, "Open media called on non-initialized stream - aborting command");
		return false;
	}

	
	if ( newValue )
	{
		xine_set_param( m_pXineStream, XINE_PARAM_VERBOSITY, XINE_VERBOSITY_DEBUG );
	}
	else
	{
		xine_set_param( m_pXineStream, XINE_PARAM_VERBOSITY, XINE_VERBOSITY_NONE );
	}

	return true;
}


void Xine_Stream::resume()
{
	if ( m_pXineVideoOutput != NULL )
	{
		int count = 50;

		while ( --count &&
								!xine_port_send_gui_data( m_pXineVideoOutput, XINE_GUI_SEND_DRAWABLE_CHANGED, ( void * ) windows[ m_iCurrentWindow ] ) )
			xine_usec_sleep( 20000 );

		if ( !count )
		{
			g_pPlutoLogger->Write(LV_STATUS, "RESUME video driver failed. make sure we have a patched xine-lib with XV2 driver\n" );
			g_pPlutoLogger->Write(LV_STATUS, "  and no other application is using the XVideo port.\n" );
			return ;
		}

		xine_port_send_gui_data( m_pXineVideoOutput, XINE_GUI_SEND_VIDEOWIN_VISIBLE, ( void * ) 1 );
	}
}


int Xine_Stream::translate_point( int gui_x, int gui_y, int *video_x, int *video_y )
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

	if ( m_pXineVideoOutput && xine_port_send_gui_data( m_pXineVideoOutput, XINE_GUI_SEND_TRANSLATE_GUI_TO_VIDEO, ( void* ) & rect ) != -1 )
	{
        /**
		driver implements gui->video coordinate space translation, use it
				 */
		*video_x = rect.x;
		*video_y = rect.y;
		return 1;
	}

	return 1;
}


void Xine_Stream::Dynamic_Pointer::pointer_hide()
{
	Window window = m_pOwner->windows[m_pOwner->m_iCurrentWindow];
	XDefineCursor( m_pOwner->m_pXDisplay, window, *m_pCursor_hidden );
	m_start_time = 0;
}

void Xine_Stream::Dynamic_Pointer::pointer_show()
{
	Window window = m_pOwner->windows[m_pOwner->m_iCurrentWindow];
	XDefineCursor( m_pOwner->m_pXDisplay, window, *m_pCursor_normal );
	m_start_time = time( NULL );
}

void Xine_Stream::Dynamic_Pointer::pointer_check_time()
{
	if ( m_start_time == 0 )
		return ;
	time_t now_time = time( NULL );
	if ( difftime( now_time, m_start_time ) >= POINTER_HIDE_SECONDS )
		pointer_hide();
}

Xine_Stream::Dynamic_Pointer::Dynamic_Pointer(
		Xine_Stream *pOwner,
    Cursor *pCursor_normal,
		Cursor *pCursor_hidden
																									) :
		m_pOwner(pOwner),
        m_pCursor_normal(pCursor_normal),
				m_pCursor_hidden(pCursor_hidden),
				m_start_time(0)
{
}

Xine_Stream::Dynamic_Pointer::~Dynamic_Pointer()
{
}

void Xine_Stream::getScreenShot( int iWidth, int iHeight, char *&pData, int &iDataSize, string &sFormat, string &sCMD_Result )
{

   	// only make screenshots if it's a video stream.
		
	if (!m_bInitialized)
	{
		g_pPlutoLogger->Write( LV_WARNING, "getScreenshot called on non-initialized stream - aborting command");
		return;
	}

	
	if ( !m_bHasVideo )
	{
		iDataSize = 0;
		return ;
	}

	sFormat = "JPG";
	g_pPlutoLogger->Write( LV_STATUS, "Making snapshot" );
	make_snapshot( sFormat.c_str(), iWidth, iHeight, true, pData, iDataSize );
}


string Xine_Stream::getRenderingWindowName()
{
	return m_sWindowTitle;
}

int Xine_Stream::getDeviceId()
{
	return 0;
	//!return m_pAggregatorObject->m_dwPK_Device;
}

bool Xine_Stream::isSlimClient()
{
	return m_isSlimClient;
}

void Xine_Stream::setSlimClient( bool isSlimClient )
{
	m_isSlimClient = isSlimClient;
}

string Xine_Stream::GetPosition()
{
	string sPosition;
	
	if( m_iChapter!=-1 )
		sPosition += " CHAPTER:" + StringUtils::itos(m_iChapter);
	
	int currentTime, totalTime;
	int iMediaPosition = getStreamPlaybackPosition(currentTime, totalTime);
	sPosition += " POS:" + StringUtils::itos(currentTime);

	if( m_iTitle!=-1 )
		sPosition += " TITLE:" + StringUtils::itos(m_iTitle);

	sPosition += " SUBTITLE:" + StringUtils::itos(getSubtitle());
	sPosition += " AUDIO:" + StringUtils::itos(getAudio());
	sPosition += " TOTAL:" + StringUtils::itos(totalTime);

	return sPosition;
}

void Xine_Stream::ReportTimecode()
{
	if (!m_bInitialized)
	{
		g_pPlutoLogger->Write( LV_WARNING, "ReportTimecode called on non-initialized stream - aborting command");
		return;
	}

	if (!m_iTimeCodeReportFrequency )
		return;
	
	if( m_bIsVDR )
	{
		g_pPlutoLogger->Write(LV_STATUS,"ignoring vdr timecode for now");
		return;
	}
	
	m_pFactory->ReportTimecode( m_iStreamID, m_iPlaybackSpeed);
}

int Xine_Stream::CalculatePosition(string &sMediaPosition,string *sMRL,int *Subtitle,int *Angle,int *AudioTrack)
{
	string::size_type pos=0;
	int iTitle=-1,iChapter=-1,iPos=0;

	if( sMRL )
	{
		pos = sMediaPosition.find(" TITLE:");
		if( pos!=string::npos )
			iTitle = atoi( sMediaPosition.substr(pos+7).c_str() );

		pos = sMediaPosition.find(" CHAPTER:");
		if( pos!=string::npos )
			iChapter = atoi( sMediaPosition.substr(pos+9).c_str() );

		if( iTitle!=-1 && iChapter!=-1 )
			(*sMRL)+="/" + StringUtils::itos(iTitle) + "." + StringUtils::itos(iChapter);
	}

	if( Subtitle )
	{
		pos = sMediaPosition.find(" SUBTITLE:");
		if( pos!=string::npos )
			*Subtitle = atoi( sMediaPosition.substr(pos+10).c_str() );
	}

	if( AudioTrack )
	{
		pos = sMediaPosition.find(" AUDIO:");
		if( pos!=string::npos )
			*AudioTrack = atoi( sMediaPosition.substr(pos+7).c_str() );
	}

	pos = sMediaPosition.find(" POS:");
	if( pos!=string::npos )
		iPos = atoi( sMediaPosition.substr(pos+5).c_str() );

	return iPos;
}

void Xine_Stream::FireMenuOnScreen(int iButtons)
{
	m_pFactory->m_pPlayer->FireMenuOnScreen( m_iRequestingObject, m_iStreamID, iButtons != 0 );
}

} // DCE namespace end
