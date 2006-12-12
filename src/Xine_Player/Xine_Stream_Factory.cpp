#include "DCE/Logger.h"
#include "DCE/DCEConfig.h"


#include "Xine_Stream_Factory.h"
#include "Xine_Stream.h"
#include "Xine_Player.h"

#include <xine.h>
#include <xine/xineutils.h>
#include <xine/video_out.h>
#include <xine/audio_out.h>

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

namespace DCE
{ // DCE namespace begin

Xine_Stream_Factory::Xine_Stream_Factory(Xine_Player *pOwner):
		m_factoryMutex("xine-stream-factory-access-mutex"),
		m_pPlayer(pOwner)
{
	pthread_mutexattr_t mutexAttr;
	pthread_mutexattr_init( &mutexAttr );
	pthread_mutexattr_settype( &mutexAttr,  PTHREAD_MUTEX_RECURSIVE_NP );
	m_factoryMutex.Init( &mutexAttr );
		
	m_bInitialized = false;
	m_pXineLibrary = NULL;
	
	m_sWindowTitle = "pluto-xine-playback-window";
	m_pXDisplay = NULL;
	m_iCurrentScreen = 0;
	m_iCurrentWindow = 0;
	
	m_iLastRenderingStream  = 0;

	// default drivers names
	m_sXineAudioDriverName = "alsa";
	m_sXineVideoDriverName = "xv";

	DCEConfig * pConfig = new DCEConfig();
	m_sConfigFile = pConfig->ReadString( "XinePlayerConfigurationFile", "/etc/pluto/xine.conf" );
	delete pConfig;
}

Xine_Stream_Factory::~Xine_Stream_Factory()
{
	if (m_bInitialized)
		ShutdownFactory();
}

bool Xine_Stream_Factory::StartupFactory()
{
	PLUTO_SAFETY_LOCK( factoryLock, m_factoryMutex );
	
	// avoid double-initialization
	if (m_bInitialized) 
	{
		g_pPlutoLogger->Write( LV_WARNING, "Double initialization attempted - bad code?");
		return false;
	}
	
	// initing X threads
	if ( !XInitThreads() )
	{
		g_pPlutoLogger->Write( LV_WARNING, "XInitThreads() failed!" );
		return false;
	}

	// connect to libxine
	m_pXineLibrary = xine_new();
	 if (!m_pXineLibrary)
	 {
		g_pPlutoLogger->Write( LV_WARNING, "Cannot connect to Xine Library");
		return false;
	 }
	
	// loading config
	g_pPlutoLogger->Write( LV_STATUS, "Loading config from %s", m_sConfigFile.c_str() );		
	xine_config_load( m_pXineLibrary, m_sConfigFile.c_str() );
	// be verbose
	xine_engine_set_param(m_pXineLibrary, XINE_ENGINE_PARAM_VERBOSITY, XINE_VERBOSITY_DEBUG);
	xine_init( m_pXineLibrary );	

	// detecting output drivers
	DetectOutputDrivers();
	
	// setting audio settings 
	setAudioSettings();
	
	// setting output driver
	setVideoDriver( m_pPlayer->DATA_Get_Hardware_acceleration() );
	
	m_sDeinterlacingConfig = m_pPlayer->DATA_Get_Deinterlacing_Mode();
	m_bUseDeinterlacing = (m_sDeinterlacingConfig!="");
	g_pPlutoLogger->Write( LV_STATUS, "Deinterlacing Mode is '%s' => deinterlacing is %s", m_sDeinterlacingConfig.c_str(), m_bUseDeinterlacing?"on":"off" );
	
	 if (!CreateWindows())
	 {
		g_pPlutoLogger->Write( LV_WARNING, "Cannot create output windows");
		return false;
	 }
	
	m_bInitialized = true;
	return true;
}

bool Xine_Stream_Factory::ShutdownFactory()
{
	// avoid double-deinitialization
	if (!m_bInitialized) 
	{
		g_pPlutoLogger->Write( LV_WARNING, "Double deinitialization attempted - wrong code?");
		return false;
	}
	
	// destroying all streams
	g_pPlutoLogger->Write( LV_WARNING, "Destroying all active streams");
	map<int, Xine_Stream*>::iterator stream;
	while(true)
	{
		int streamID;
		{
			PLUTO_SAFETY_LOCK( factoryLock, m_factoryMutex );
			stream = streamsMap.begin();
			if (stream == streamsMap.end())
				break;
			else
				streamID = (*stream).first;
		}
		DestroyStream( streamID);
	}
	
	DestroyWindows();
	
	PLUTO_SAFETY_LOCK( factoryLock, m_factoryMutex );
	
	// disconnect from libxine
	xine_exit(m_pXineLibrary);
	m_pXineLibrary = NULL;
	m_bInitialized = false;
	
	return true;
}

// detecting currently used drivers
void Xine_Stream_Factory::DetectOutputDrivers()
{
	//registering video key param

	xine_cfg_entry_t xineConfigEntry;
	char **driver_ids;
	int i;
	const char *const *driver_names;
	
	driver_names = xine_list_video_output_plugins( m_pXineLibrary );
	
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
	xine_config_register_enum( m_pXineLibrary, "video.driver", 0, driver_ids, "video driver to use", "Choose video driver. NOTE: you may restart xine to use the new driver", 0, NULL, NULL );	
	
	// setting video driver
	if ( xine_config_lookup_entry ( m_pXineLibrary, "video.driver", &xineConfigEntry ) )
	{
		driver_names = xine_list_video_output_plugins( m_pXineLibrary );
		m_sXineVideoDriverName = driver_ids[ xineConfigEntry.num_value ];
		g_pPlutoLogger->Write( LV_STATUS, "Using video driver: %s", m_sXineVideoDriverName.c_str() );
	}
	else
		g_pPlutoLogger->Write( LV_STATUS, "Video driver key was not defined in the config file, using hardcoded default: %s", m_sXineVideoDriverName.c_str() );
	
	free( driver_ids );


	// registering audio keyparam

	driver_names = xine_list_audio_output_plugins( m_pXineLibrary );
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
	xine_config_register_enum( m_pXineLibrary, "audio.driver", 0, driver_ids, "audio driver to use", "Choose audio driver. NOTE: you may restart xine to use the new driver", 0, NULL, NULL );

	// audio driver
	if ( xine_config_lookup_entry ( m_pXineLibrary, "audio.driver", &xineConfigEntry ) )
	{
		driver_names = xine_list_audio_output_plugins( m_pXineLibrary );
		m_sXineAudioDriverName = driver_ids[ xineConfigEntry.num_value ];
		g_pPlutoLogger->Write( LV_STATUS, "Using audio driver: %s", m_sXineAudioDriverName.c_str() );
	}
	else
		g_pPlutoLogger->Write( LV_STATUS, "Audio driver key was not defined in the config file, using hardcoded default: %s", m_sXineAudioDriverName.c_str() );
	
	free( driver_ids );
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

void Xine_Stream_Factory::setAudioSettings()
{
	string sAudioSettings = m_pPlayer->Get_MD_AudioSettings();

	if (sAudioSettings=="")
	{
		g_pPlutoLogger->Write( LV_STATUS, "M/D Audio Settings are empty, assuming 'M'");
		sAudioSettings = "M";
	}
	else
		g_pPlutoLogger->Write( LV_STATUS, "M/D Audio Settings: %s", sAudioSettings.c_str());
	
	bool updateConfig = true;
	
	// if value remains empty after scan, it won't be set
	string sAlsaFrontDevice = "";
	string sSpeakersArrangement = "Stereo 2.0";
	
	for (int i=0; i<sAudioSettings.length(); i++)
	{
		switch (sAudioSettings[i])
		{
		case 'C':
		case 'O':
			sAlsaFrontDevice = "asym_spdif";
			sSpeakersArrangement = "Pass Through";
			break;
		
		case 'S':
		
		case 'L':
			sAlsaFrontDevice = "plughw:0";
			break;
		
		case '3':
			sSpeakersArrangement = "Pass Through";
			break;
		
		case 'M':
			updateConfig = false;
			break;
		
		default:
			g_pPlutoLogger->Write( LV_STATUS, "Unknown audio settings flag: '%c'", sAudioSettings[i]);
		}
	}
	
	if (!updateConfig)
	{
		g_pPlutoLogger->Write( LV_STATUS, "Flag 'M' found, we won't override the defaults from /etc/pluto/xine.conf");
	}
	
	
	// processing ALSA front device
	xine_cfg_entry_t cfgAlsaFrontDevice;
	xine_config_register_string(m_pXineLibrary, "audio.alsa_front_device", "default", "ALSA front device setting", NULL, 0, NULL, NULL);
	if (xine_config_lookup_entry(m_pXineLibrary, "audio.alsa_front_device", &cfgAlsaFrontDevice) == 0)
	{
		g_pPlutoLogger->Write( LV_STATUS, "Could not lookup the current 'ALSA front device', skipping" );
	}
	else
	{
		g_pPlutoLogger->Write( LV_STATUS, "Current value for 'ALSA front device': %s", cfgAlsaFrontDevice.str_value );
		if (updateConfig)
		if (sAlsaFrontDevice=="")
		{
			g_pPlutoLogger->Write( LV_STATUS, "Configured value for 'ALSA front device' is empty, not overriding it" );
		}
		else
		{
			g_pPlutoLogger->Write( LV_STATUS, "Updating value for 'ALSA front device' to: %s", sAlsaFrontDevice.c_str() );
			cfgAlsaFrontDevice.str_value = strdup(sAlsaFrontDevice.c_str());
			xine_config_update_entry( m_pXineLibrary, &cfgAlsaFrontDevice );
			if (xine_config_lookup_entry(m_pXineLibrary, "audio.alsa_front_device", &cfgAlsaFrontDevice) == 0)
			{
				g_pPlutoLogger->Write( LV_STATUS, "Could not lookup the 'ALSA front device' after update" );
			}
			else
			{
				g_pPlutoLogger->Write( LV_STATUS, "Updated value for 'ALSA front device': %s", cfgAlsaFrontDevice.str_value );
			}			
		}
	}
	
	// processing Speakers Arrangement
	xine_cfg_entry_t cfgSpeakersArrangement;
	xine_config_register_enum ( m_pXineLibrary, "audio.output.speaker_arrangement", 1, ( char ** ) audio_out_types_strs, 
		"Speakers arrangement", NULL, 0, NULL, NULL );
	if ( xine_config_lookup_entry( m_pXineLibrary, "audio.output.speaker_arrangement", &cfgSpeakersArrangement ) == 0 )
	{
		g_pPlutoLogger->Write( LV_STATUS, "Could not lookup the current 'Speakers Arrangement', skipping" );
	}
	else
	{
		g_pPlutoLogger->Write( LV_STATUS, "Current value for 'Speakers Arrangement': %s", audio_out_types_strs[ cfgSpeakersArrangement.num_value ] );
		if (updateConfig)
		if (sSpeakersArrangement=="")
		{
			g_pPlutoLogger->Write( LV_STATUS, "Configured value for 'Speakers Arrangement' is empty, not overriding it" );
		}
		else
		{
			g_pPlutoLogger->Write( LV_STATUS, "Updating value for 'Speakers Arrangement' to: %s", sSpeakersArrangement.c_str() );
			int i = 0;
			while ( audio_out_types_strs[ i ] != NULL )
			{
				if ( strncmp( sSpeakersArrangement.c_str( ), audio_out_types_strs[ i ], strlen( audio_out_types_strs[ i ] ) ) == 0 )
				{
					cfgSpeakersArrangement.num_value = i;
					xine_config_update_entry( m_pXineLibrary, &cfgSpeakersArrangement );
		
					if ( xine_config_lookup_entry( m_pXineLibrary, "audio.output.speaker_arrangement", &cfgSpeakersArrangement ) == 0 )
					{
						g_pPlutoLogger->Write( LV_STATUS, "Could not lookup the 'Speakers Arrangement' after update" );
					}
					else
					{
						g_pPlutoLogger->Write( LV_STATUS, "Updated value for 'Speakers Arrangement': %s", audio_out_types_strs[ cfgSpeakersArrangement.num_value ] );
					}
					
					break;
				}
		
				i++;
			}
		}
	}
}


// returns pointer to stream if exists/was created or NULL otherwise
Xine_Stream *Xine_Stream_Factory::GetStream(int streamID, bool createIfNotExist, int requestingObject, bool bBroadcasting)
{
	{
		PLUTO_SAFETY_LOCK( factoryLock, m_factoryMutex );
		
		if( streamID==0 )
		{
			g_pPlutoLogger->Write(LV_CRITICAL,"Xine_Stream_Factory::GetStream streamID is 0");
			streamID = m_iLastRenderingStream;
		}

		map<int, Xine_Stream*>::iterator stream = streamsMap.find(streamID);
		if (stream != streamsMap.end())
		{
			Xine_Stream *pStream = (*stream).second;
			pStream->StartupStream();
			return pStream;
		}
	}
		
	if (createIfNotExist && streamID!=0)
	{
		Xine_Stream *new_stream = new Xine_Stream(this, m_pXineLibrary, streamID, m_pPlayer->DATA_Get_Time_Code_Report_Frequency(), requestingObject, bBroadcasting);
		new_stream->m_bUseDeinterlacing = m_bUseDeinterlacing;
		new_stream->m_sDeinterlacingConfig = m_sDeinterlacingConfig;
		if ((new_stream!=NULL) && new_stream->StartupStream())
		{
			g_pPlutoLogger->Write(LV_WARNING,"Created new stream with ID=%i", streamID);
			
			PLUTO_SAFETY_LOCK( factoryLock, m_factoryMutex );
			streamsMap[streamID] = new_stream;
			return new_stream;
		}
		else
		{
			g_pPlutoLogger->Write(LV_WARNING,"Cannot create new stream with ID=%i", streamID);
			if (new_stream!=NULL)
				delete new_stream;
			return NULL;
		}
	}
	else
		return NULL;
}


void Xine_Stream_Factory::ReportAudioTracks(string sTracks)
{
	m_pPlayer->DATA_Set_Audio_Tracks(sTracks);
}

void Xine_Stream_Factory::ReportSubtitles( string sSubtitles)
{
	m_pPlayer->DATA_Set_Subtitles(sSubtitles);
}

void Xine_Stream_Factory::ReportAVInfo( string sFilename, int iStreamID, string sMediaInfo, string sAudioInfo, string sVideoInfo )
{
	g_pPlutoLogger->Write(LV_WARNING, "Xine_Player::EVENT_Playback_Started(streamID=%i) <= AV info", iStreamID);
	m_pPlayer->EVENT_Playback_Started(sFilename, iStreamID, sMediaInfo, sAudioInfo, sVideoInfo);
}


void Xine_Stream_Factory::ReportTimecodeViaIP(int iStreamID, int Speed)
{
	m_pPlayer->ReportTimecodeViaIP( iStreamID, Speed);
}

void Xine_Stream_Factory::DestroyStream(int iStreamID)
{
	Xine_Stream *pStream = NULL;
	map<int, Xine_Stream*>::iterator stream;
	
	{	
		PLUTO_SAFETY_LOCK( factoryLock, m_factoryMutex );
		stream = streamsMap.find(iStreamID);
		
		if (stream != streamsMap.end())
		{
			pStream = (*stream).second;
			streamsMap.erase(stream);
		}
	}
	
	if (pStream)
	{
		delete pStream;
	}
	
	g_pPlutoLogger->Write(LV_WARNING,"Destroyed stream with ID=%i", iStreamID);
}

void Xine_Stream_Factory::HideStreamWindows(int iStreamID)
{
	Xine_Stream *pStream = NULL;
	map<int, Xine_Stream*>::iterator stream;
	
	{	
		PLUTO_SAFETY_LOCK( factoryLock, m_factoryMutex );
		stream = streamsMap.find(iStreamID);
		
		if (stream != streamsMap.end())
		{
			pStream = (*stream).second;
		}
	}
	
	if (pStream)
	{
		pStream->hideWindows();
	}
	g_pPlutoLogger->Write(LV_WARNING,"Hide windows for stream with ID=%i", iStreamID);
}

void Xine_Stream_Factory::CloseStreamAV(int iStreamID)
{
	Xine_Stream *pStream = NULL;
	map<int, Xine_Stream*>::iterator stream;
	
	{	
		PLUTO_SAFETY_LOCK( factoryLock, m_factoryMutex );
		stream = streamsMap.find(iStreamID);
		
		if (stream != streamsMap.end())
		{
			pStream = (*stream).second;
		}
	}
	
	if (pStream)
	{
		pStream->ShutdownStream();
	}
	
	g_pPlutoLogger->Write(LV_WARNING,"Closed stream AV with ID=%i", iStreamID);
}

void Xine_Stream_Factory::setVideoDriver(string strVideoDriver)
{
	if ( (strVideoDriver!="") && (strVideoDriver=="cle266x11"||strVideoDriver=="xv"||strVideoDriver=="xxmc"||strVideoDriver=="opengl"||strVideoDriver=="sdl"||strVideoDriver=="xshm") )
	{
		g_pPlutoLogger->Write( LV_STATUS, "Overriding video driver setting, using '%s' for video output", strVideoDriver.c_str());
		m_sXineVideoDriverName = strVideoDriver;
	}
	else
	{
		g_pPlutoLogger->Write( LV_STATUS, "Not overriding video driver setting, device data is not acceptable: '%s'", strVideoDriver.c_str());
	}
}


// creates stream windows
bool Xine_Stream_Factory::CreateWindows()
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
	
	//XSynchronize( m_pXDisplay, true);

	XLockDisplay( m_pXDisplay );

	m_iCurrentScreen = XDefaultScreen( m_pXDisplay );
	// creating it on last (invisible) screen
	int iScreens = XScreenCount( m_pXDisplay );
	g_pPlutoLogger->Write( LV_WARNING, "Count of screens at this display: %i", iScreens);
	//m_iCurrentScreen = iScreens-1;
	xpos = 10;
	ypos = 20;
	width = 100;
	height = 100;
	int border = 0;

	windows[ 0 ] = XCreateSimpleWindow( m_pXDisplay, XDefaultRootWindow( m_pXDisplay ), xpos, ypos, width, height, border, 0, 0 );
	windows[ 1 ] = XCreateSimpleWindow( m_pXDisplay, XDefaultRootWindow( m_pXDisplay ), xpos, ypos, width, height, border, 0, 0 );

/*
	windows[ 0 ] = XCreateSimpleWindow( m_pXDisplay, XRootWindow( m_pXDisplay, m_iCurrentScreen ), xpos, ypos, width, height, 
																			1, 0, 0 );
	windows[ 1 ] = XCreateSimpleWindow( m_pXDisplay, XRootWindow( m_pXDisplay, m_iCurrentScreen ),
																			0, 0, ( DisplayWidth( m_pXDisplay, m_iCurrentScreen ) ), DisplayHeight( m_pXDisplay, m_iCurrentScreen ),
																			0, 0, 0 );
*/
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

// 	noCursor = XCreateBitmapFromData( m_pXDisplay, ( DefaultRootWindow( m_pXDisplay ) ), noCursorDataDescription, 8, 8 );
// 	cursors[0] = XCreateFontCursor(m_pXDisplay, XC_left_ptr);
// 	cursors[1] = XCreatePixmapCursor(m_pXDisplay, noCursor, noCursor, &black, &black, 0, 0);
// 	
// 	if ( m_pDynamic_Pointer )
// 		delete m_pDynamic_Pointer;
// 	m_pDynamic_Pointer = new Dynamic_Pointer(this, &cursors[0], &cursors[1]);
// 
// 	XFreePixmap( m_pXDisplay, noCursor );
// 
// 	XDefineCursor( m_pXDisplay, windows[ m_iCurrentWindow ], cursors[ m_iCurrentWindow ] );
 	XMapRaised( m_pXDisplay, windows[ m_iCurrentWindow ] );
// 
// 	if ( m_pDynamic_Pointer )
// 		m_pDynamic_Pointer->pointer_hide();
// 
// 	res_h = ( DisplayWidth( m_pXDisplay, m_iCurrentScreen ) * 1000 / DisplayWidthMM( m_pXDisplay, m_iCurrentScreen ) );
// 	res_v = ( DisplayHeight( m_pXDisplay, m_iCurrentScreen ) * 1000 / DisplayHeightMM( m_pXDisplay, m_iCurrentScreen ) );
// 
// 	m_dPixelAspect = res_v / res_h;
// 
// 	g_pPlutoLogger->Write( LV_STATUS, "XServer aspect %f", m_dPixelAspect );
// 
// 	if ( fabs( m_dPixelAspect - 1.0 ) < 0.01 )
// 		m_dPixelAspect = 1.0;

	//XSync( m_pXDisplay, True );
	XUnlockDisplay( m_pXDisplay );

	return true;
}

bool Xine_Stream_Factory::DestroyWindows()
{
	if ( m_pXDisplay != NULL )
	{
//		if (!m_pDynamic_Pointer)
//			delete m_pDynamic_Pointer;
//		m_pDynamic_Pointer = NULL;
		
		XLockDisplay( m_pXDisplay );

		XUnmapWindow( m_pXDisplay, windows[ m_iCurrentWindow ] );

//		XFreeCursor( m_pXDisplay, cursors[ 0 ] );
//		XFreeCursor( m_pXDisplay, cursors[ 1 ] );

		XDestroyWindow( m_pXDisplay, windows[ 0 ] );
		XDestroyWindow( m_pXDisplay, windows[ 1 ] );

		XUnlockDisplay( m_pXDisplay );
		XCloseDisplay ( m_pXDisplay );

		m_pXDisplay = NULL;
	}

	return true;
}

} // DCE namespace end
