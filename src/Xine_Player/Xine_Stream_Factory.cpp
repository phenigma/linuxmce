#include "DCE/Logger.h"
#include "DCE/DCEConfig.h"


#include "Xine_Stream_Factory.h"
#include "Xine_Stream.h"
#include "Xine_Player.h"

namespace DCE
{ // DCE namespace begin

Xine_Stream_Factory::Xine_Stream_Factory(Xine_Player *pOwner):
		m_pPlayer(pOwner),
		m_factoryMutex("xine-stream-factory-access-mutex")
{
	pthread_mutexattr_t mutexAttr;
	pthread_mutexattr_init( &mutexAttr );
	pthread_mutexattr_settype( &mutexAttr,  PTHREAD_MUTEX_RECURSIVE_NP );
	m_factoryMutex.Init( &mutexAttr );
		
	m_bInitialized = false;
	m_pXineLibrary = NULL;
	
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
	xine_init( m_pXineLibrary );
	
	// detecting output drivers
	DetectOutputDrivers();
	
	m_bInitialized = true;
	return true;
}

bool Xine_Stream_Factory::ShutdownFactory()
{
	PLUTO_SAFETY_LOCK( factoryLock, m_factoryMutex );
	
	// avoid double-deinitialization
	if (!m_bInitialized) 
	{
		g_pPlutoLogger->Write( LV_WARNING, "Double deinitialization attempted - wrong code?");
		return false;
	}
	
	// disconnect from libxine
	xine_exit(m_pXineLibrary);
	m_pXineLibrary = NULL;
	m_bInitialized = false;
	
	return true;
}

// detecting currently used drivers
void Xine_Stream_Factory::DetectOutputDrivers()
{
	xine_cfg_entry_t xineConfigEntry;
	const char *const *driver_names;
	
	// video driver
	if ( xine_config_lookup_entry ( m_pXineLibrary, "video.driver", &xineConfigEntry ) )
	{
		driver_names = xine_list_video_output_plugins( m_pXineLibrary );
		m_sXineVideoDriverName = driver_names[ xineConfigEntry.num_value ];
	}
	else
		g_pPlutoLogger->Write( LV_STATUS, "Video driver key was not defined in the config file, using hardcoded default: %s", m_sXineVideoDriverName.c_str() );

	// audio driver
	if ( xine_config_lookup_entry ( m_pXineLibrary, "audio.driver", &xineConfigEntry ) )
	{
		driver_names = xine_list_audio_output_plugins( m_pXineLibrary );
		m_sXineAudioDriverName = driver_names[ xineConfigEntry.num_value ];
	}
	else
		g_pPlutoLogger->Write( LV_STATUS, "Audio driver key was not defined in the config file, using hardcoded default: %s", m_sXineAudioDriverName.c_str() );
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

void Xine_Stream_Factory::setOutputSpeakerArrangement( string strOutputSpeakerArrangement )
{
	xine_cfg_entry_t xineConfigEntry;

	g_pPlutoLogger->Write( LV_STATUS, "Setting the audio output speaker arrangement: %s", strOutputSpeakerArrangement.c_str() );

	xine_config_register_enum ( m_pXineLibrary, "audio.output.speaker_arrangement", 1, ( char ** ) audio_out_types_strs,
															"Speaker arrangement",
															NULL, 0, NULL, NULL );

	if ( xine_config_lookup_entry( m_pXineLibrary, "audio.output.speaker_arrangement", &xineConfigEntry ) == 0 )
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
			xine_config_update_entry( m_pXineLibrary, &xineConfigEntry );

			if ( xine_config_lookup_entry( m_pXineLibrary, "audio.output.speaker_arrangement", &xineConfigEntry ) == 0 )
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

// returns pointer to stream if exists/was created or NULL otherwise
Xine_Stream *Xine_Stream_Factory::GetStream(int streamID)
{
	PLUTO_SAFETY_LOCK( factoryLock, m_factoryMutex );
	
	map<int, Xine_Stream*>::iterator stream = streamsMap.find(streamID);
	if (stream != streamsMap.end())
		return (*stream).second;
	else
	{
		Xine_Stream *new_stream = new Xine_Stream(this, m_pXineLibrary, streamID, m_pPlayer->DATA_Get_Time_Code_Report_Frequency());
		if ((new_stream!=NULL) && new_stream->StartupStream())
		{
			g_pPlutoLogger->Write(LV_WARNING,"Created new stream with ID=%i", streamID);
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
}

void Xine_Stream_Factory::ReportAudioTracks(string sTracks)
{
	m_pPlayer->DATA_Set_Audio_Tracks(sTracks);
}

void Xine_Stream_Factory::ReportSubtitles( string sSubtitles)
{
	m_pPlayer->DATA_Set_Subtitles(sSubtitles);
}

void Xine_Stream_Factory::ReportTimecode(int iStreamID, int Speed)
{
	m_pPlayer->ReportTimecode( iStreamID, Speed);
}

void Xine_Stream_Factory::DestroyStream(int iStreamID)
{
	PLUTO_SAFETY_LOCK( factoryLock, m_factoryMutex );
	
	Xine_Stream *pStream = NULL;
	
	map<int, Xine_Stream*>::iterator stream = streamsMap.find(iStreamID);
	if (stream != streamsMap.end())
	{
		pStream = (*stream).second;
		streamsMap.erase(stream);
		delete pStream;
		g_pPlutoLogger->Write(LV_WARNING,"Destroyed stream with ID=%i", iStreamID);
	}
}

} // DCE namespace end
