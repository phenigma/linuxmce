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

/** Define this here to avoid some dependency. */
typedef struct
{
  uint32_t  flags;
  uint32_t  functions;
  uint32_t  decorations;
  int32_t   input_mode;
  uint32_t  status;
} MWMHints;

XineSlaveWrapper::Xine_Player *m_pAggregatorObject;  


XineSlaveWrapper::XineSlaveWrapper()
    : m_sWindowTitle("pluto-xine-playback-window"),
      XServerDisplay(NULL),
      m_pSameStream(NULL)
{
    m_iCurrentWindow = 0;

    m_sXineAudioDriverName = "alsa";
    m_sXineVideoDriverName = "xv";

    DCEConfig * pConfig = new DCEConfig();
    m_sConfigFile = pConfig->ReadString("XinePlayerConfigurationFile", "/etc/pluto/xine.conf");
    delete pConfig;

    m_pXineVisualizationPlugin = NULL;
	m_bExitThread = false;
}

XineSlaveWrapper::~XineSlaveWrapper()
{
	stopMedia(1); // any number will do.
	closeWindow();
}

static const char noCursorDataDescription[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


/**
 *   \fn bool XineSlaveWrapper::createWindow()
 */
bool XineSlaveWrapper::createWindow()
{
    XColor          black; //, dummy;
    XSizeHints      sizeHints;
    MWMHints        wmHints;
	XClassHint		classHint;

    long            propertyValues[1];
    int             completionEvent;
    int             xpos, ypos, width, height;

    double          res_h, res_v;

    if(!XInitThreads())
    {
            g_pPlutoLogger->Write(LV_WARNING, "XInitThreads() failed!");
            return false;
    }

    if((XServerDisplay = XOpenDisplay(getenv("DISPLAY"))) == NULL)
    {
        g_pPlutoLogger->Write(LV_WARNING, "Could not open X DISPLAY from: %s", getenv("DISPLAY"));
        return false;
    }

    XLockDisplay(XServerDisplay);

    m_iCurrentScreen   = XDefaultScreen(XServerDisplay);
    xpos            = 10;
    ypos            = 20;
    width           = 720;
    height          = 540;

    windows[0] = XCreateSimpleWindow(XServerDisplay, XDefaultRootWindow(XServerDisplay),
                    xpos, ypos, width, height,
                    1, 0, 0);
    windows[1] = XCreateSimpleWindow(XServerDisplay, XDefaultRootWindow(XServerDisplay),
                    0, 0, (DisplayWidth(XServerDisplay, m_iCurrentScreen)), DisplayHeight(XServerDisplay, m_iCurrentScreen),
                    0, 0, 0);

	classHint.res_name = (char*)m_sWindowTitle.c_str();
	classHint.res_class = (char*)m_sWindowTitle.c_str();
	XSetClassHint ( XServerDisplay, windows[0], &classHint );

	classHint.res_class = (char*)m_sWindowTitle.c_str();
	XSetClassHint ( XServerDisplay, windows[0], &classHint );

    XSelectInput(XServerDisplay, windows[0], INPUT_MOTION);
    XSelectInput(XServerDisplay, windows[1], INPUT_MOTION);

    XSetStandardProperties(XServerDisplay, windows[0], m_sWindowTitle.c_str(), m_sWindowTitle.c_str(), None, NULL, 0, 0);
    XSetStandardProperties(XServerDisplay, windows[1], m_sWindowTitle.c_str(), m_sWindowTitle.c_str(), None, NULL, 0, 0);

    sizeHints.win_gravity = StaticGravity;
    sizeHints.flags       = PPosition | PSize | PWinGravity;

    XSetWMNormalHints(XServerDisplay, windows[0], &sizeHints);
    XSetWMNormalHints(XServerDisplay, windows[1], &sizeHints);

    Atom XA_DELETE_WINDOW = XInternAtom(XServerDisplay, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(XServerDisplay, windows[0], &XA_DELETE_WINDOW, 1);
    XSetWMProtocols(XServerDisplay, windows[1], &XA_DELETE_WINDOW, 1);

    Atom XA_NO_BORDER   = XInternAtom(XServerDisplay, "_MOTIF_WM_HINTS", False);
    wmHints.flags       = (1L << 1); // MWM_HINTS_DECORATIONS
    wmHints.decorations = 0;

    XChangeProperty(XServerDisplay, windows[1], XA_NO_BORDER, XA_NO_BORDER, 32,
                PropModeReplace, (unsigned char *) &wmHints,
                5 /* PROP_MWM_HINTS_ELEMENTS */);

    Atom XA_WIN_LAYER = XInternAtom(XServerDisplay, "_NET_WM_STATE", False);
    propertyValues[0] = 10;
    XChangeProperty(XServerDisplay, windows[1], XA_WIN_LAYER, XA_CARDINAL, 32, PropModeReplace, (unsigned char *)propertyValues, 1);


    XA_WIN_LAYER = XInternAtom(XServerDisplay, "_NET_WM_STATE", False);
    propertyValues[0] = 12;
    XChangeProperty(XServerDisplay, windows[1], XA_WIN_LAYER, XA_CARDINAL, 32, PropModeReplace, (unsigned char *)propertyValues, 1);

    if(XShmQueryExtension(XServerDisplay) == True)
            completionEvent = XShmGetEventBase(XServerDisplay) + ShmCompletion;
    else
            completionEvent = -1;

    noCursor   = XCreateBitmapFromData(XServerDisplay, (DefaultRootWindow(XServerDisplay)), noCursorDataDescription, 8, 8);
    cursors[0] = XCreateFontCursor(XServerDisplay, XC_left_ptr);
    cursors[1] = XCreatePixmapCursor(XServerDisplay, noCursor, noCursor, &black, &black, 0, 0);
    XFreePixmap(XServerDisplay, noCursor);

    XDefineCursor(XServerDisplay, windows[m_iCurrentWindow], cursors[m_iCurrentWindow]);
    XMapRaised(XServerDisplay, windows[m_iCurrentWindow]);

    res_h = (DisplayWidth(XServerDisplay, m_iCurrentScreen) * 1000 / DisplayWidthMM(XServerDisplay, m_iCurrentScreen));
    res_v = (DisplayHeight(XServerDisplay, m_iCurrentScreen) * 1000 / DisplayHeightMM(XServerDisplay, m_iCurrentScreen));

    m_dPixelAspect = res_v / res_h;

    g_pPlutoLogger->Write(LV_STATUS, "XServer aspect %f", m_dPixelAspect);

    if(fabs(m_dPixelAspect - 1.0) < 0.01)
        m_dPixelAspect = 1.0;

    XSync(XServerDisplay, True);
    XUnlockDisplay(XServerDisplay);

    return true;
}


/**
 *
 *  \fn XineSlaveWrapper::createStream(string fileName, int streamID, int mediaPosition, int iRequestingObject);
 */
bool XineSlaveWrapper::createStream(string fileName, int streamID, int iRequestingObject)
{
    g_pPlutoLogger->Write(LV_STATUS, "Got a create stream command command for %s", fileName.c_str());

	bool isNewStream = false;

    XineStream *xineStream = getStreamForId(streamID, ""); /** @warning HACK: This should be changed to behave properly */

    g_pPlutoLogger->Write(LV_STATUS, "Testing to see if we can reuse the stream.");

    if ( xineStream == NULL )
    {
		g_pPlutoLogger->Write(LV_STATUS, "Nope .. making a new one");
        isNewStream = true;
        xineStream = new XineStream();
        xineStream->m_pOwner = this;
        xineStream->m_iStreamID = streamID;
		setStreamForId(streamID, xineStream);
    }
    else
    {
        g_pPlutoLogger->Write(LV_STATUS, "Closing old stream");
        if ( m_pXineVisualizationPlugin )
        {
            xine_post_wire_audio_port(xine_get_audio_source(xineStream->m_pStream), m_pXineAudio);
            xine_post_dispose(m_pXine, m_pXineVisualizationPlugin);
            m_pXineVisualizationPlugin = NULL;
        }

		// this can be null if we weren't able to open any ports in a previous atempt
		if ( xineStream->m_pStream )
        	xine_close(xineStream->m_pStream);

        g_pPlutoLogger->Write(LV_STATUS, "Closed!");

        xineStream->m_bIsRendering = false;
        xineStream->m_iStreamID = streamID;
    }

    m_x11Visual.display           = XServerDisplay;
    m_x11Visual.screen            = m_iCurrentScreen;
    m_x11Visual.d                 = windows[m_iCurrentWindow];
    m_x11Visual.dest_size_cb      = &destinationSizeCallback;
    m_x11Visual.frame_output_cb   = &frameOutputCallback;
    m_x11Visual.user_data         = xineStream;

    g_pPlutoLogger->Write(LV_STATUS, "Opening Video Driver");
    if( isNewStream && (m_pXineVideo = xine_open_video_driver(m_pXine, m_sXineVideoDriverName.c_str(), XINE_VISUAL_TYPE_X11, (void *) &m_x11Visual)) == NULL)
    {
        g_pPlutoLogger->Write(LV_WARNING, "I'm unable to initialize m_pXine's '%s' video driver. Giving up.", m_sXineVideoDriverName.c_str());
        return false;
    }

    g_pPlutoLogger->Write(LV_STATUS, "Opening Audio Driver");
    if ( isNewStream && (m_pXineAudio = xine_open_audio_driver(m_pXine, m_sXineAudioDriverName.c_str(), NULL)) == NULL )
    {
        g_pPlutoLogger->Write(LV_WARNING, "I'm unable to initialize m_pXine's '%s' audio driver.", m_sXineAudioDriverName.c_str());
        return false;
    }

    g_pPlutoLogger->Write(LV_STATUS, "Calling xine_stream_new");
    if ( isNewStream && (xineStream->m_pStream = xine_stream_new(m_pXine, m_pXineAudio, m_pXineVideo)) == NULL )
    {
        g_pPlutoLogger->Write(LV_WARNING, "Could not create stream (wanted to play: %s)", fileName.c_str());
        delete xineStream;
        return false;
    }

    xineStream->m_iRequestingObject = iRequestingObject;

    if (isNewStream )
        m_mapStreams[streamID] = xineStream; /** @todo: check for leaks (if someone will send me multiple playStream with the same Stream ID it will leak memory) */

    if ( isNewStream )
    {
	    g_pPlutoLogger->Write(LV_STATUS, "xine_event_new_queue");
        xineStream->m_pStreamEventQueue = xine_event_new_queue(xineStream->m_pStream);
        xine_event_create_listener_thread(xineStream->m_pStreamEventQueue, xineEventListener, xineStream);
    }

	xine_port_send_gui_data(m_pXineVideo, XINE_GUI_SEND_VIDEOWIN_VISIBLE, (void *) 0);
    xine_port_send_gui_data(m_pXineVideo, XINE_GUI_SEND_DRAWABLE_CHANGED, (void *) windows[m_iCurrentWindow]);
    xine_port_send_gui_data(m_pXineVideo, XINE_GUI_SEND_VIDEOWIN_VISIBLE, (void *) 1);

	setXineStreamDebugging(streamID, true);

    g_pPlutoLogger->Write(LV_STATUS, "Ready to call xine_open");
    if( xine_open(xineStream->m_pStream, fileName.c_str()))
    {
        g_pPlutoLogger->Write(LV_STATUS, "Opened..... ");

        setXineStreamDebugging(streamID, false);
        xineStream->m_bHasVideo = xine_get_stream_info(xineStream->m_pStream, XINE_STREAM_INFO_HAS_VIDEO);

        if ( xineStream->m_iImgWidth == 0)
             xineStream->m_iImgWidth++;

        if ( xineStream->m_iImgHeight == 0)
             xineStream->m_iImgHeight++;

        if ( xineStream->m_bHasVideo )
        {
            xine_post_wire_audio_port( xine_get_audio_source(xineStream->m_pStream), m_pXineAudio);
        }
        else
        {
            if ( ! m_pXineVisualizationPlugin )
                m_pXineVisualizationPlugin = xine_post_init(m_pXine, "goom", 1, &m_pXineAudio, &m_pXineVideo);

            if  ( m_pXineVisualizationPlugin )
            {
                // const char *const *inputs;
                bool bPostWireResult;

//                 bPostWireResult = xine_post_wire_audio_port(xine_get_audio_source(xineStream->m_pStream), m_pXineVisualizationPlugin->audio_input[1]);
                bPostWireResult = false;

                g_pPlutoLogger->Write(LV_STATUS, "Visualization plugin %s", bPostWireResult ?  "enabled" : "disabled");
                if ( bPostWireResult )
                {
                    xineStream->m_iImgWidth = 100;
                    xineStream->m_iImgHeight = 100;
                }
            }
        }

        g_pPlutoLogger->Write(LV_STATUS, "Got image dimensions: %dx%d", xineStream->m_iImgWidth, xineStream->m_iImgWidth );

        xine_port_send_gui_data(m_pXineVideo, XINE_GUI_SEND_DRAWABLE_CHANGED, (void *) windows[m_iCurrentWindow]);
        xine_port_send_gui_data(m_pXineVideo, XINE_GUI_SEND_VIDEOWIN_VISIBLE, (void *) 1);

        xineStream->m_bIsRendering = true;

        if ( isNewStream )
        {
            g_pPlutoLogger->Write(LV_STATUS, "Creating event processor");
			m_bExitThread = false;
            pthread_create(&xineStream->eventLoop, NULL, eventProcessingLoop, xineStream);
            g_pPlutoLogger->Write(LV_STATUS, "Event processor started");
        }

        if ( ! xineStream->m_bHasVideo )
        {
            g_pPlutoLogger->Write(LV_STATUS, "Stream is seekable: %d", xine_get_stream_info(xineStream->m_pStream, XINE_STREAM_INFO_SEEKABLE));
            xine_set_param(xineStream->m_pStream, XINE_PARAM_SPEED, XINE_SPEED_PAUSE);
        }
    }
    else
    {
        g_pPlutoLogger->Write(LV_STATUS, "Failed to open %s", fileName.c_str());
        xineStream->m_pOwner->playbackCompleted(xineStream->m_iStreamID,true);
    }

	g_pPlutoLogger->Write(LV_STATUS, "Done with CreateStream");
	return true;
}

/**
 * @brief play the stream with the streamID identifier.
 */
bool XineSlaveWrapper::playStream(int streamID, int mediaPosition, bool playbackStopped)
{
	XineStream *xineStream = getStreamForId(streamID, "XineSlaveWrapper::playStream() could not play an empty stream!");

	if ( xineStream == NULL )
		return false;

	time_t startTime = time(NULL);

	if ( xine_play(xineStream->m_pStream, 0, 0) )
	{
		g_pPlutoLogger->Write(LV_STATUS, "Playing... The command took %d seconds to complete: ", time(NULL) - startTime);

		g_pPlutoLogger->Write(LV_STATUS, "Starting audio stream from position: %d", mediaPosition);
		xine_play(xineStream->m_pStream, mediaPosition, 0);

		if ( playbackStopped )
			xine_set_param(xineStream->m_pStream, XINE_PARAM_SPEED, XINE_SPEED_PAUSE);
		else
			xine_set_param(xineStream->m_pStream, XINE_PARAM_SPEED, XINE_SPEED_NORMAL);
		return true;
	}
    else
	{
		xineStream->m_pOwner->playbackCompleted(xineStream->m_iStreamID, false);
		return false;
	}
}

bool XineSlaveWrapper::closeWindow()
{
    if ( XServerDisplay != NULL )
    {
        XLockDisplay(XServerDisplay);

        XUnmapWindow(XServerDisplay,  windows[m_iCurrentWindow]);


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

        XFreeCursor(XServerDisplay, cursors[0]);
        XFreeCursor(XServerDisplay, cursors[1]);

        XDestroyWindow(XServerDisplay,  windows[0]);
        XDestroyWindow(XServerDisplay,  windows[1]);

        XUnlockDisplay(XServerDisplay);
        XCloseDisplay (XServerDisplay);

		m_bExitThread = true;
		if ( m_pSameStream )
			pthread_join(m_pSameStream->eventLoop, NULL);

        XServerDisplay = NULL;
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
    g_pPlutoLogger->Write(LV_STATUS, "Loading config from %s", m_sConfigFile.c_str());
    xine_config_load(m_pXine, m_sConfigFile.c_str());
	xine_init(m_pXine);

	xine_config_register_enum(m_pXine,
		"gui.experience_level", 0,
		experience_labels,
		"display of configuration settings",
		"Controls whether more advanced configuration settings are shown.",
     	0, NULL, (void*)1);

	return true;
}

bool XineSlaveWrapper::createXineLibConnection()
{
	if (!loadXineConfiguration())
		return false;

	xine_cfg_entry_t xineConfigEntry;
    char **driver_ids;
    int i;
    const char *const *driver_names;

    driver_names= xine_list_video_output_plugins(m_pXine);

	i = 0;
    while(driver_names[i++]);
    driver_ids  = (char **) xine_xmalloc(sizeof(char *) * (i + 1));
    i = 0;
    driver_ids[i] = strdup("auto");

    while( driver_names[i] )
	{
		driver_ids[i + 1] = strdup(driver_names[i]);
		i++;
    }

    driver_ids[i + 1] = NULL;
    xine_config_register_enum(m_pXine, "video.driver", 0, driver_ids, "video driver to use", "Choose video driver. NOTE: you may restart xine to use the new driver", 0, NULL, NULL);
    if ( xine_config_lookup_entry (m_pXine, "video.driver", &xineConfigEntry) == 1 )
        m_sXineVideoDriverName = driver_ids[xineConfigEntry.num_value];
    else
        g_pPlutoLogger->Write(LV_STATUS, "Video driver key not defined in the config file using: %s", m_sXineVideoDriverName.c_str());

    free(driver_ids);

    driver_names = xine_list_audio_output_plugins(m_pXine);
    i = 0;
    while(driver_names[i++]);
    driver_ids  = (char **) xine_xmalloc(sizeof(char *) * (i + 1));
    i = 0;
    driver_ids[i] = strdup("auto");
    while(driver_names[i]) {
      driver_ids[i + 1] = strdup(driver_names[i]);
      i++;
    }

    driver_ids[i + 1] = NULL;
    xine_config_register_enum(m_pXine, "audio.driver", 0, driver_ids, "audio driver to use", "Choose audio driver. NOTE: you may restart xine to use the new driver", 0, NULL, NULL);
    if ( xine_config_lookup_entry (m_pXine, "audio.driver", &xineConfigEntry) == 1 )
        m_sXineAudioDriverName = driver_ids[xineConfigEntry.num_value];
    else
        g_pPlutoLogger->Write(LV_STATUS, "Audio driver key not defined in the config file using: %s", m_sXineVideoDriverName.c_str());
    free(driver_ids);

    g_pPlutoLogger->Write(LV_STATUS, "Using Video driver %s and Audio driver %s", m_sXineVideoDriverName.c_str(), m_sXineAudioDriverName.c_str());

	return true;
}


/**
 * xine Callbacks.
 */

void XineSlaveWrapper::destinationSizeCallback(void *data, int video_width, int video_height, double video_pixel_aspect,
                                               int *dest_width, int *dest_height,
                                               double *dest_pixel_aspect)
{
    XineStream *pStream = (XineStream*)data;

/**
* @test
  if( ! m_pStream->m_bIsRendering )
         g_pPlutoLogger->Write(LV_STATUS, "Destination size callback called (not rendering)!");
*/

    *dest_width        = pStream->m_iImgWidth  + (pStream->m_bIsRendering ? 0 : 1);
    *dest_height       = pStream->m_iImgHeight + (pStream->m_bIsRendering ? 0 : 1);
    *dest_pixel_aspect = pStream->m_pOwner->m_dPixelAspect;
}

void XineSlaveWrapper::frameOutputCallback(void *data, int video_width, int video_height, double video_pixel_aspect,
                                          int *dest_x, int *dest_y, int *dest_width, int *dest_height,
                                          double *dest_pixel_aspect,
                                          int *win_x, int *win_y)
{
    XineStream *pStream = (XineStream*)data;

/**
* @test
*     if( ! pStream->m_bIsRendering)
*         g_pPlutoLogger->Write(LV_STATUS, "Framer Output callback called (not rendering).");
*/

    *dest_x            = 0;
    *dest_y            = 0;
    *win_x             = pStream->m_iImgXPos;
    *win_y             = pStream->m_iImgYPos;
    *dest_width        = pStream->m_iImgWidth + (pStream->m_bIsRendering ? 0 : 1);
    *dest_height       = pStream->m_iImgHeight + (pStream->m_bIsRendering ? 0 : 1);
    *dest_pixel_aspect = pStream->m_pOwner->m_dPixelAspect;
}


/**
    \fn XineSlaveWrapper::xineEventListener(void *userData)
 */
void XineSlaveWrapper::xineEventListener(void *userData, const xine_event_t *event)
{
    PLUTO_SAFETY_LOCK(xineSlaveLock, m_xineSlaveMutex);
    XineStream *xineStream = (XineStream *)userData;

    switch(event->type)
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
            g_pPlutoLogger->Write(LV_WARNING, "Playback finished for m_pstream: %d", xineStream->m_iStreamID);
			usleep(2000000);  // AB 10-June-05 -- Xine seems to report this before the buffers are flushed (about 1.5 seconds too early).  So we'll add a little delay
g_pPlutoLogger->Write(LV_WARNING, "Playback finished for m_pstream: %d after sleep", xineStream->m_iStreamID);
            xineStream->m_pOwner->playbackCompleted(xineStream->m_iStreamID,false);
            xineStream->m_bIsRendering = false;
            break;
		case XINE_EVENT_QUIT:
			g_pPlutoLogger->Write(LV_STATUS, "Stream was disposed");
			// the playback completed is sent from another place. (see the stopMedia)
			break;

        case XINE_EVENT_PROGRESS:
			{
				xine_progress_data_t *pProgressEvent = (xine_progress_data_t *) event->data;
				g_pPlutoLogger->Write(LV_WARNING, "Playback (%s) is at %d%.", pProgressEvent->description, pProgressEvent->percent);
			}
            break;

        case XINE_EVENT_UI_NUM_BUTTONS:
            {
                int iButtons = ((xine_ui_data_t *)event->data)->num_buttons;

				g_pPlutoLogger->Write(LV_STATUS, "Menu with %d buttons", iButtons);
                if ( xineStream->m_pOwner->m_pAggregatorObject )
                    xineStream->m_pOwner->m_pAggregatorObject->FireMenuOnScreen(xineStream->m_iRequestingObject, xineStream->m_iStreamID, iButtons != 0);
            }
            break;

        case XINE_EVENT_UI_SET_TITLE:
			{
//				char volume[256];
//                int dummy;
                xine_ui_data_t *data = (xine_ui_data_t *) event->data;

				//if(sscanf(data->str, "DVD Navigator: Menu, %s", volume)==1 ||
                   //sscanf(data->str, "DVD Menu, %s", volume)==1 ||
				   //sscanf(data->str, "DVD Root Menu, %s", volume)==1)
                //{
                    //if( ! cur_mrl || ! strchr(cur_mrl, TITLE_DELIMITER) )
//						g_pPlutoLogger->Write(LV_STATUS, "data: %s", data->str);

                    // send_event(XINE_SE_DVD_TITLE, volume);
  //              }
    //            if(data->str && sscanf(data->str,"Title %d, Chapter %d",&dummy,&dvd_chapter) == 2) {
      //              send_event(XINE_SE_CHAPTER, "%d", dvd_chapter);
        //        } else {
          //          dvd_chapter = 0;
            //    }
            	// g_pPlutoLogger->Write(LV_STATUS, "Event ");
				// xineStream->m_pOwner->
				g_pPlutoLogger->Write(LV_STATUS, "UI set title: %s", data->str);
/*
				g_pPlutoLogger->Write(LV_STATUS, "Stream title: %s", xine_get_meta_info(xineStream->m_pStream, XINE_META_INFO_TITLE));
				g_pPlutoLogger->Write(LV_STATUS, "Stream comment: %s", xine_get_meta_info(xineStream->m_pStream, XINE_META_INFO_COMMENT));
				g_pPlutoLogger->Write(LV_STATUS, "Stream artist: %s", xine_get_meta_info(xineStream->m_pStream, XINE_META_INFO_ARTIST));
				g_pPlutoLogger->Write(LV_STATUS, "Stream genre: %s", xine_get_meta_info(xineStream->m_pStream, XINE_META_INFO_GENRE));
				g_pPlutoLogger->Write(LV_STATUS, "Stream album: %s", xine_get_meta_info(xineStream->m_pStream, XINE_META_INFO_ALBUM));
				g_pPlutoLogger->Write(LV_STATUS, "Stream year: %s", xine_get_meta_info(xineStream->m_pStream, XINE_META_INFO_YEAR));
				g_pPlutoLogger->Write(LV_STATUS, "Stream track: %s", xine_get_meta_info(xineStream->m_pStream, XINE_META_INFO_TRACK_NUMBER));
*/
			}
			break;

		case XINE_EVENT_SPU_BUTTON:
		case XINE_EVENT_INPUT_MOUSE_BUTTON:
		case XINE_EVENT_INPUT_MOUSE_MOVE:
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
			g_pPlutoLogger->Write(LV_STATUS, "Stream info audio channels %d", xine_get_stream_info(xineStream->m_pStream, XINE_STREAM_INFO_MAX_AUDIO_CHANNEL));
			break;

        default:
            g_pPlutoLogger->Write(LV_STATUS, "Got unprocessed Xine playback event: %d", event->type);
            break;
    }
}

bool XineSlaveWrapper::setXineStreamDebugging(int m_istreamID, bool newValue)
{
    XineStream *xineStream = getStreamForId(m_istreamID, "Trying to set debugging level for and invalid stream: (%d)");

    if ( xineStream == NULL )
        return false;

    if ( newValue )
    {
        xine_set_param(xineStream->m_pStream, XINE_PARAM_VERBOSITY, XINE_VERBOSITY_DEBUG);
    }
    else
    {
        xine_set_param(xineStream->m_pStream, XINE_PARAM_VERBOSITY, XINE_VERBOSITY_NONE);
    }

    return true;
}


void XineSlaveWrapper::resume()
{
  if (m_pXineVideo != NULL) {
    int count = 50;

    while( --count &&
          !xine_port_send_gui_data(m_pXineVideo, XINE_GUI_SEND_DRAWABLE_CHANGED, (void *) windows[m_iCurrentWindow]) )
        xine_usec_sleep(20000);

    if( !count ) {
      printf("RESUME video driver failed. make sure we have a patched xine-lib with XV2 driver\n");
      printf("  and no other application is using the XVideo port.\n");
      return;
    }

    xine_port_send_gui_data(m_pXineVideo, XINE_GUI_SEND_VIDEOWIN_VISIBLE, (void *) 1);
  }
}

void *XineSlaveWrapper::eventProcessingLoop(void *arguments)
{
    XineStream *pStream = (XineStream*)arguments;

    Bool checkResult;

	Display *pDisplay = XOpenDisplay(getenv("DISPLAY"));

    XEvent  event;
    while ( ! pStream->m_pOwner->m_bExitThread )
    {
        if ( pStream->m_bIsRendering )
		{
			do
			{
				XLockDisplay(pStream->m_pOwner->XServerDisplay);
	//			g_pPlutoLogger->Write(LV_STATUS, "Locking display!");
				//XLockDisplay(pDisplay);
	//			g_pPlutoLogger->Write(LV_STATUS, "Getting next event!");
				// checkResult = XWindowEvent(pDisplay, pStream->m_pOwner->windows[pStream->m_pOwner->m_iCurrentWindow], INPUT_MOTION, &event);
				checkResult = XCheckWindowEvent(pStream->m_pOwner->XServerDisplay, pStream->m_pOwner->windows[pStream->m_pOwner->m_iCurrentWindow], INPUT_MOTION, &event);
	//			g_pPlutoLogger->Write(LV_STATUS, "Unlocking display!");
				//XUnlockDisplay(pDisplay);
	//			g_pPlutoLogger->Write(LV_STATUS, "Done!");
				XUnlockDisplay(pStream->m_pOwner->XServerDisplay);

				if ( checkResult == True )
					pStream->m_pOwner->XServerEventProcessor(pStream, event);

			} while ( checkResult == True );
		}
		usleep(1000);
    }

	XCloseDisplay(pDisplay);
    return NULL;
}

int XineSlaveWrapper::XServerEventProcessor(XineStream *pStream, XEvent &event)
{
    Atom  XA_DELETE_WINDOW;

    switch(event.type)
    {
        case ClientMessage:
        {
            XA_DELETE_WINDOW = XInternAtom(XServerDisplay, "WM_DELETE_WINDOW", False);

            if((unsigned)event.xclient.data.l[0] == XA_DELETE_WINDOW)
                pStream->m_bIsRendering = false;
            break;
        }

        case MotionNotify:
        {
            XMotionEvent *mevent = (XMotionEvent *) &event;
            int x, y;

            if(pStream->m_pOwner->translate_point(mevent->x, mevent->y, &x, &y)) {
                xine_event_t       xineEvent;
                xine_input_data_t  xineInput;

                xineEvent.type        = XINE_EVENT_INPUT_MOUSE_MOVE;
                xineEvent.stream      = pStream->m_pStream;
                xineEvent.data        = &xineInput;
                xineEvent.data_length = sizeof(xineInput);
                gettimeofday(&xineEvent.tv, NULL);

                xineInput.button      = 0;
                xineInput.x           = x;
                xineInput.y           = y;
                xine_event_send(pStream->m_pStream, &xineEvent);
            }
            break;
        }

		case ButtonPress:
		{
			XButtonEvent *bevent = (XButtonEvent *) &event;
			xine_input_data_t xineInputData;
			xine_event_t xineEvent;

			x11_rectangle_t   rect;

			g_pPlutoLogger->Write(LV_STATUS, "Xine player: mouse button event: mx=%d my=%d", bevent->x, bevent->y);

			rect.x = bevent->x;
			rect.y = bevent->y;
			rect.w = 0;
			rect.h = 0;

			xine_gui_send_vo_data (pStream->m_pStream, XINE_GUI_SEND_TRANSLATE_GUI_TO_VIDEO, (void*)&rect);
			g_pPlutoLogger->Write(LV_STATUS, "Xine player: mouse button event after translation: mx=%d my=%d", rect.x, rect.y);

			xineEvent.stream = pStream->m_pStream;
  			xineEvent.type = XINE_EVENT_INPUT_MOUSE_BUTTON;
			xineEvent.data = &xineInputData;
    		xineEvent.data_length = sizeof(xineInputData);

			xineInputData.button = bevent->button;
    		xineInputData.x = rect.x;
    		xineInputData.y = rect.y;

			gettimeofday(&xineEvent.tv, NULL);
 			XLockDisplay(bevent->display);
			xine_event_send(pStream->m_pStream, &xineEvent);
			XUnlockDisplay(bevent->display);

			break;
		}

        case KeyPress:
        {
            XKeyEvent  kevent;
            KeySym     ksym;
            char       kbuf[256];
            int        len;

            kevent = event.xkey;

            XLockDisplay(kevent.display);
            len = XLookupString(&kevent, kbuf, sizeof(kbuf), &ksym, NULL);
            XUnlockDisplay(kevent.display);
            // g_pPlutoLogger->Write(LV_STATUS, "Key (%d), \"%s\" - %d", len, kbuf, ksym);

            xine_event_t       xineEvent;
            // xine_input_data_t  xineInput;

            switch ( ksym )
            {
                case XK_Up: xineEvent.type = XINE_EVENT_INPUT_UP; break;
                case XK_Down: xineEvent.type = XINE_EVENT_INPUT_DOWN; break;
                case XK_Left: xineEvent.type = XINE_EVENT_INPUT_LEFT; break;
                case XK_Right: xineEvent.type = XINE_EVENT_INPUT_RIGHT; break;
                case XK_Return: xineEvent.type = XINE_EVENT_INPUT_SELECT; break;
                default:
                    xineEvent.type = 0;
            }
            xineEvent.stream      = pStream->m_pStream;
            xineEvent.data        = NULL;
            xineEvent.data_length = 0;
            gettimeofday(&xineEvent.tv, NULL);
            xine_event_send(pStream->m_pStream, &xineEvent);
			break;
        }

        case Expose:
        {
            XExposeEvent *exposeEvent = (XExposeEvent *)&event;
			// g_pPlutoLogger->Write(LV_STATUS, "Expose with count %d", exposeEvent->count);

            if( exposeEvent->count != 0 )
                break;

            if (pStream->m_pOwner->m_pXineVideo)
                xine_port_send_gui_data(pStream->m_pOwner->m_pXineVideo, XINE_GUI_SEND_EXPOSE_EVENT, exposeEvent);

            break;
        }

        case ConfigureNotify:
        {
            XConfigureEvent *cev = (XConfigureEvent *) &event;
            Window           tmp_win;

            pStream->m_iImgWidth    = cev->width;
            pStream->m_iImgHeight   = cev->height;

            if((cev->x == 0) && (cev->y == 0))
            {
                XLockDisplay(cev->display);
                XTranslateCoordinates(cev->display, cev->window, DefaultRootWindow(cev->display), 0, 0, &pStream->m_iImgXPos, &pStream->m_iImgYPos, &tmp_win);
                XUnlockDisplay(cev->display);
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

int XineSlaveWrapper::translate_point(int gui_x, int gui_y, int *video_x, int *video_y)
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

  if(m_pXineVideo && xine_port_send_gui_data(m_pXineVideo, XINE_GUI_SEND_TRANSLATE_GUI_TO_VIDEO, (void*)&rect) != -1) {
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
void XineSlaveWrapper::changePlaybackSpeed(int iStreamID, PlayBackSpeedType desiredSpeed)
{
    XineStream *pStream;

    if ( (pStream = getStreamForId(iStreamID, "Can't set the speed of a non existent stream (%d)!")) == NULL)
        return;

    int xineSpeed = XINE_SPEED_PAUSE;
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
            xineSpeed = XINE_SPEED_FAST_2;
            break;
        case PLAYBACK_FF_4:
            xineSpeed = XINE_SPEED_FAST_4;
            break;

        default:
            g_pPlutoLogger->Write(LV_WARNING, "Don't know how to handle speed: %d", desiredSpeed);
            break;
    }

    if ( (xineSpeed == XINE_SPEED_PAUSE && desiredSpeed == 0) || xineSpeed != XINE_SPEED_PAUSE)
        xine_set_param(pStream->m_pStream, XINE_PARAM_SPEED, xineSpeed);
}

XineSlaveWrapper::PlayBackSpeedType XineSlaveWrapper::getPlaybackStream(int iStreamID)
{
	XineStream *pStream;

	if ( (pStream = getStreamForId(iStreamID, "Can't get the speed of a non existent stream (%d)!")) == NULL )
		return PLAYBACK_STOP;

	int currentSpeed;
	switch ( (currentSpeed = xine_get_param(pStream->m_pStream, XINE_PARAM_SPEED)) )
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
			g_pPlutoLogger->Write(LV_WARNING, "Can't translate current Xine speed %d. Assuming normal playback speed!", currentSpeed);
			return PLAYBACK_FF_1;
		}
	}
}

/**
    \fn XineSlaveWrapper::stopMedia(int iStreamID)
 */
void XineSlaveWrapper::stopMedia(int iStreamID)
{
    XineStream *pStream = getStreamForId(iStreamID, "Can't stop the playback of a non existent stream (%d)!");

    if ( pStream == NULL )
		return;

	// stop the event thread first
	if ( pStream->eventLoop )
	{
		g_pPlutoLogger->Write(LV_STATUS, "Stopping event thread.");
		m_bExitThread = true;

		pthread_join(pStream->eventLoop, NULL);
		g_pPlutoLogger->Write(LV_STATUS, "Done.");
	}

	if ( pStream->m_pStreamEventQueue )
	{
		g_pPlutoLogger->Write(LV_STATUS, "Disposing the event queue");
		xine_event_dispose_queue(pStream->m_pStreamEventQueue);
	}

	if ( pStream->m_pStream )
	{
		g_pPlutoLogger->Write(LV_STATUS, "Calling xine_stop for stream with id: %d", iStreamID);
		xine_stop(pStream->m_pStream);

		g_pPlutoLogger->Write(LV_STATUS, "Calling xine_close for stream with id: %d", iStreamID);
		xine_close(pStream->m_pStream);

		g_pPlutoLogger->Write(LV_STATUS, "Calling xine_dispose for stream with id: %d", iStreamID);
		xine_dispose(pStream->m_pStream);
	}

	g_pPlutoLogger->Write(LV_STATUS, "Going to call a %p and v %p",m_pXineAudio,m_pXineVideo);

	if ( m_pXineAudio )
	{
		g_pPlutoLogger->Write(LV_STATUS, "Calling xine_close_audio_driver for stream with id: %d");
		xine_close_audio_driver(m_pXine, m_pXineAudio);
		m_pXineAudio = NULL;
	}

	if ( m_pXineVideo )
	{
		g_pPlutoLogger->Write(LV_STATUS, "Calling xine_close_video_driver for stream with id: %d");
		xine_close_video_driver(m_pXine, m_pXineVideo);
		m_pXineVideo = NULL;
	}

	g_pPlutoLogger->Write(LV_STATUS, "deleting pstream");
	delete pStream;

	// ignore the return since we know for sure that it was the same stream
	setStreamForId(iStreamID, NULL);

	g_pPlutoLogger->Write(LV_STATUS, "Cleanup completed");
}

/**
    \fn XineSlaveWrapper::restartMediaStream(int iStreamID)
 */
void XineSlaveWrapper::restartMediaStream(int iStreamID)
{
    changePlaybackSpeed(iStreamID, PLAYBACK_FF_1);
}

/**
    \fn XineSlaveWrapper::pauseMediaStream(int iStreamID)
 */
void XineSlaveWrapper::pauseMediaStream(int iStreamID)
{
    int stoppedTime, completeTime;

	if ( getPlaybackStream(iStreamID) == PLAYBACK_STOP )
		changePlaybackSpeed(iStreamID, PLAYBACK_FF_1);
	else
    	changePlaybackSpeed(iStreamID, PLAYBACK_STOP);

    getStreamPlaybackPosition(iStreamID, stoppedTime, completeTime);

    g_pPlutoLogger->Write(LV_STATUS, "Stream paused at time: %d from %d", stoppedTime, completeTime);
}

/**
 * \fn void XineSlaveWrapper::sendInputEvent(int eventID);
 */
void XineSlaveWrapper::sendInputEvent(int eventType)
{
	time_t startTime = time(NULL);
	XineStream *pStream;
	if ( (pStream = getStreamForId(1, "void XineSlaveWrapper::sendInputEvent(int eventType) getting one stream")) == NULL )
		return;

	g_pPlutoLogger->Write(LV_STATUS, "Sending %d event to the engine.", eventType);

	xine_event_t event;

    event.type        = eventType;
    event.stream      = pStream->m_pStream;
    event.data        = NULL;
    event.data_length = 0;
    gettimeofday(&event.tv, NULL);

	xine_event_send(pStream->m_pStream, &event);
	g_pPlutoLogger->Write(LV_STATUS, "XineSlaveWrapper::sendInputEvent() Sending input event with ID: %d took %d seconds", eventType, time(NULL) - startTime);
}

void XineSlaveWrapper::XineStream::setPlaybackSpeed(int speed)
{
/** @todo: Make this proper.
     changePlaybackSpeed(iS)
*/
}

string XineSlaveWrapper::getRenderingWindowName()
{
    return m_sWindowTitle;
}

void XineSlaveWrapper::selectNextButton(int iStreamID)
{
    XineStream *pStream = getStreamForId(iStreamID, "Can't navigate on an non existent m_pstream (%d)!");

    if (pStream == NULL)
        return;

    g_pPlutoLogger->Write(LV_STATUS, "Selecting next hot spot on the m_pstream %d", iStreamID);

    xine_event_t event;

    event.type        = XINE_EVENT_INPUT_UP;
    event.stream      = pStream->m_pStream;
    event.data        = NULL;
    event.data_length = 0;
    gettimeofday(&event.tv, NULL);

    xine_event_send(pStream->m_pStream, &event);
}

void XineSlaveWrapper::selectPrevButton(int iStreamID)
{
    XineStream *pStream = getStreamForId(iStreamID, "Can't navigate on an non existent m_pstream (%d)!");

    if (pStream == NULL)
        return;

    g_pPlutoLogger->Write(LV_STATUS, "Selecting next hot spot on the m_pstream %d", iStreamID);

    xine_event_t event;

    event.type        = XINE_EVENT_INPUT_DOWN;
    event.stream      = pStream->m_pStream;
    event.data        = NULL;
    event.data_length = 0;
    gettimeofday(&event.tv, NULL);

    xine_event_send(pStream->m_pStream, &event);
}

void XineSlaveWrapper::pushCurrentButton(int iStreamID)
{
    XineStream *pStream = getStreamForId(iStreamID, "Can't push the buttons on invalid m_pstream (%d)");

    if (pStream == NULL)
        return;

    g_pPlutoLogger->Write(LV_STATUS, "Selecting next hot spot on the m_pstream %d", iStreamID);

    xine_event_t event;

    event.type        = XINE_EVENT_INPUT_SELECT;
    event.stream      = pStream->m_pStream;
    event.data        = NULL;
    event.data_length = 0;
    gettimeofday(&event.tv, NULL);

    xine_event_send(pStream->m_pStream, &event);
}

void XineSlaveWrapper::setXinePlayerObject(Xine_Player *object)
{
    m_pAggregatorObject = object;
}

void XineSlaveWrapper::getScreenShot(int iStreamID, int iWidth, int iHeight, char *&pData, int &iDataSize, string &sFormat, string &sCMD_Result)
{
    XineStream *pStream = getStreamForId(iStreamID, "I need a valid stream to be able to make screenshots.");

    if ( pStream == NULL )
        return;

    // only make screenshots if it's a video stream.
    if ( ! pStream->m_bHasVideo )
    {
        iDataSize = 0;
        return;
    }

    sFormat = "JPG";
    g_pPlutoLogger->Write(LV_STATUS, "Making snapshot");
    make_snapshot(pStream->m_pStream, sFormat.c_str(), iWidth, iHeight, true, pData, iDataSize);
}

void XineSlaveWrapper::make_snapshot(xine_stream_t *pStream, string sFormat, int iWidth, int iHeight, bool bKeepAspect, char*&pData, int &iDataSize)
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

	int         imageWidth = 0, imageHeight = 0, imageRatio = 0, imageFormat = 0;
    uint8_t     *imageData = NULL;

	if ( pStream == NULL )
	{
		g_pPlutoLogger->Write(LV_WARNING, "XineSlaveWrapper::make_snapshot(): The xine_stream_t object passed as parameter was null. Can't get the screen shot");
		return;
	}

    g_pPlutoLogger->Write(LV_STATUS, "XineSlaveWrapper::make_snapshot(): Getting frame info");
    if ( ! xine_get_current_frame(pStream, &imageWidth, &imageHeight, &imageRatio, &imageFormat, NULL) )
	{
		g_pPlutoLogger->Write(LV_WARNING, "XineSlaveWrapper::make_snapshot(): Error getting frame info. Returning!");
		return;
	}
    g_pPlutoLogger->Write(LV_STATUS, "XineSlaveWrapper::make_snapshot(): Got %d %d %d 0x%x", imageWidth, imageHeight, imageRatio, imageFormat);

    imageData = (uint8_t*)malloc ((imageWidth+8) * (imageHeight+1) * 2);
    g_pPlutoLogger->Write(LV_STATUS, "XineSlaveWrapper::make_snapshot(): Getting frame data");
    if ( ! xine_get_current_frame (pStream, &imageWidth, &imageHeight, &imageRatio, &imageFormat, imageData) )
	{
		g_pPlutoLogger->Write(LV_WARNING, "XineSlaveWrapper::make_snapshot(): Error getting frame data. Returning!");
		return;
	}
    g_pPlutoLogger->Write(LV_STATUS, "XineSlaveWrapper::make_snapshot(): Got %d %d %d 0x%x", imageWidth, imageHeight, imageRatio, imageFormat);

    // we should have the image in YV12 format aici
    // if not then we try to convert it.
    if ( imageFormat == XINE_IMGFMT_YUY2)
    {
        uint8_t *yuy2Data = imageData;
        uint8_t *imageData = (uint8_t *)malloc (imageWidth * imageHeight * 2);

        yuy2toyv12 (
                imageData,
                imageData + imageWidth * imageHeight,
                imageData + imageWidth * imageHeight * 5 / 4, yuy2Data, imageWidth, imageHeight);

        free (yuy2Data);
    }

    g_pPlutoLogger->Write(LV_STATUS, "XineSlaveWrapper::make_snapshot(): Converted to YV12");
    // convert to RGB
    // keep the yv12Data array around to be able to delete it
    uint8_t *yv12Data = imageData;

    /** @brief this function will allocate the output parameter */
    imageData = yv12torgb (
                    imageData,
                    imageData + imageWidth * imageHeight,
                    imageData + imageWidth * imageHeight * 5 / 4,
                    imageWidth, imageHeight);
    g_pPlutoLogger->Write(LV_STATUS, "XineSlaveWrapper::make_snapshot(): Converted to RGB!");
    free(yv12Data);

    double outputRatio;
    // double currentRatio;

    g_pPlutoLogger->Write(LV_STATUS, "XineSlaveWrapper::make_snapshot(): Temp data was freed here!");
    switch (imageRatio)
    {
        case XINE_VO_ASPECT_ANAMORPHIC:  /* anamorphic     */
        case XINE_VO_ASPECT_PAN_SCAN:    /* we display pan&scan as widescreen */
            outputRatio = 16.0 /9.0;
            break;

        case XINE_VO_ASPECT_DVB:         /* 2.11:1 */
            outputRatio = 2.11/1.0;
            break;

        case XINE_VO_ASPECT_SQUARE:      /* square pels */
        case XINE_VO_ASPECT_DONT_TOUCH:  /* probably non-mpeg m_pstream => don't touch aspect ratio */
            outputRatio = (double) imageWidth / (double) imageHeight;
            break;

        default:
            g_pPlutoLogger->Write(LV_WARNING, "XineSlaveWrapper::make_snapshot(): Unknown aspect ratio for image (%d) using 4:3", imageRatio);

        case XINE_VO_ASPECT_4_3:         /* 4:3             */
            outputRatio = 4.0 / 3.0;
            break;
    }

    double f = outputRatio / ((double) imageWidth / (double) imageHeight);

    g_pPlutoLogger->Write(LV_STATUS, "XineSlaveWrapper::make_snapshot(): The ratio between the current ration and the output ratio is %f", f);
    double t_width, t_height;
//     if( !bKeepAspect )
//     {
    if (f >= 1.0) {
        t_width  = imageWidth * f;
        t_height = imageHeight;
    }
    else
    {
        t_width  = imageWidth;
        t_height = imageHeight / f;
    }

    t_width /= 2;
    t_height /= 2;

	g_pPlutoLogger->Write(LV_STATUS, "XineSlaveWrapper::make_snapshot(): Making jpeg from RGB");
	JpegEncoderDecoder jpegEncoder;

	jpegEncoder.encodeIntoBuffer((char *)imageData, imageWidth, imageHeight, 3, pData, iDataSize);

	g_pPlutoLogger->Write(LV_STATUS, "XineSlaveWrapper::make_snapshot(): final image size: %d", iDataSize);

	FILE * file;
    file = fopen("/tmp/file.jpg", "wb");
	g_pPlutoLogger->Write(LV_STATUS, "XineSlaveWrapper::make_snapshot(): temporary image filep %p", file);

	fwrite(pData, iDataSize, 1, file);
    fclose(file);

	g_pPlutoLogger->Write(LV_STATUS, "XineSlaveWrapper::make_snapshot(): At the end. Returning");
    return;
}

void XineSlaveWrapper::yuy2toyv12 (uint8_t *y, uint8_t *u, uint8_t *v, uint8_t *input, int width, int height)
{
    int    i, j, w2;

    w2 = width / 2;

    for (i = 0; i < height; i += 2)
    {
        for (j = 0; j < w2; j++)
        {
            /*
             * packed YUV 422 is: Y[i] U[i] Y[i+1] V[i]
             */
            *(y++) = *(input++);
            *(u++) = *(input++);
            *(y++) = *(input++);
            *(v++) = *(input++);
        }

        /*
         * down sampling
         */
        for (j = 0; j < w2; j++)
        {
            /*
             * skip every second line for U and V
             */
            *(y++) = *(input++);
            input++;
            *(y++) = *(input++);
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

uint8_t * XineSlaveWrapper::yv12torgb (uint8_t *src_y, uint8_t *src_u, uint8_t *src_v, int width, int height)
{
  int     i, j;

  int     y, u, v;
  int     r, g, b;

  int     sub_i_uv;
  int     sub_j_uv;

  int     uv_width, uv_height;

  uint8_t *rgb;

  uv_width  = width / 2;
  uv_height = height / 2;

  rgb = (uint8_t*)malloc (width * height * 3);
  if (!rgb)
      return NULL;

  for (i = 0; i < height; ++i) {
    /*
     * calculate u & v rows
     */
    sub_i_uv = ((i * uv_height) / height);

    for (j = 0; j < width; ++j) {
      /*
       * calculate u & v columns
       */
      sub_j_uv = ((j * uv_width) / width);

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

      y = src_y[(i * width) + j] - 16;
      u = src_u[(sub_i_uv * uv_width) + sub_j_uv] - 128;
      v = src_v[(sub_i_uv * uv_width) + sub_j_uv] - 128;

      r = (int)((1.1644 * y) + (1.5960 * v));
      g = (int)((1.1644 * y) - (0.3918 * u) - (0.8130 * v));
      b = (int)((1.1644 * y) + (2.0172 * u));

      clip_8_bit (r);
      clip_8_bit (g);
      clip_8_bit (b);

      rgb[(i * width + j) * 3 + 0] = r;
      rgb[(i * width + j) * 3 + 1] = g;
      rgb[(i * width + j) * 3 + 2] = b;
    }
  }

  return rgb;
}

XineSlaveWrapper::XineStream *XineSlaveWrapper::setStreamForId(int iStreamID, XineStream *pNewStream)
{
	XineStream *pXineStream = NULL;

	pXineStream = m_pSameStream;
	m_pSameStream = pNewStream;

	return pXineStream;
}

XineSlaveWrapper::XineStream *XineSlaveWrapper::getStreamForId(int iStreamID, string strMessageInvalid)
{
    /** @warning HACK: use the same m_pstream regardless of the StreamID; */

//     if ( m_pSameStream == NULL )
//         m_pSameStream = new XineStream();;

    if ( m_pSameStream == NULL )
        g_pPlutoLogger->Write(LV_STATUS, strMessageInvalid.c_str(), iStreamID);

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

void XineSlaveWrapper::selectMenu(int iStreamID, int iMenuType)
{
    XineStream *xineStream = getStreamForId(iStreamID, "Can't select menu for a nonexistent stream!");

    if ( xineStream == NULL )
        return;

    xine_event_t xine_event;

    xine_event.type        = iMenuType + XINE_EVENT_INPUT_MENU1;
    xine_event.data_length = 0;
    xine_event.data        = NULL;
    xine_event.stream      = xineStream->m_pStream;
    gettimeofday(&xine_event.tv, NULL);

    xine_event_send(xineStream->m_pStream, &xine_event);
}

void XineSlaveWrapper::playbackCompleted(int iStreamID,bool bWithErrors)
{
    g_pPlutoLogger->Write(LV_STATUS, "Fire playback completed event");
	if ( ! m_isSlimClient )
    	this->m_pAggregatorObject->EVENT_Playback_Completed(iStreamID,bWithErrors);
}

int XineSlaveWrapper::getStreamPlaybackPosition(int iStreamID, int &positionTime, int &totalTime)
{
    XineStream *xineStream = getStreamForId(iStreamID, "Can't get the position of a nonexistent stream!");

    if ( xineStream == NULL )
        return 0;

    int iPosStream = 0;
    int iPosTime = 0;
    int iLengthTime = 0;

    int count = 10;
    while( --count && ! xine_get_pos_length(xineStream->m_pStream, &iPosStream, &iPosTime, &iLengthTime) )
    {
          g_pPlutoLogger->Write(LV_STATUS, "Error reading stream position: %d", xine_get_error(xineStream->m_pStream));
          usleep(500);
    }

//     if ( xine_get_pos_length(xineStream->m_pStream, &iPosStream, &iPosTime, &iLengthTime) == 0 )
//     {
//         g_pPlutoLogger->Write(LV_STATUS, "Error getting the stream position");
//         return -1;
//     }

    g_pPlutoLogger->Write(LV_STATUS, "Got those numbers: %d, %d", iPosTime, iLengthTime);

    positionTime = iPosTime;
    totalTime = iLengthTime;
    return positionTime;
}

int XineSlaveWrapper::enableBroadcast(int iStreamID)
{
    XineStream *pStream = getStreamForId(iStreamID, "Can't broadcast a stream that is not available");

    if ( pStream == NULL )
        return 0;

    int portNumber = 7866;
    if( portNumber != xine_get_param(pStream->m_pStream, XINE_PARAM_BROADCASTER_PORT) )
    {
//         if( port && xine_get_param(pStream->m_pStream, XINE_PARAM_BROADCASTER_PORT) )
            xine_set_param(pStream->m_pStream, XINE_PARAM_BROADCASTER_PORT, portNumber);

        /* try up to ten times from port base. sometimes we have trouble
        * binding to the same port we just used.
        */
        for( int i = 0; i < 10; i++ )
        {
            xine_set_param(pStream->m_pStream, XINE_PARAM_BROADCASTER_PORT, ++portNumber);
            if( portNumber == xine_get_param(pStream->m_pStream, XINE_PARAM_BROADCASTER_PORT) )
                return portNumber;
        }
    }

    return 0;
}

void XineSlaveWrapper::simulateMouseClick(int X, int Y)
{
	XineStream *pStream;
	xine_input_data_t xineInputData;
	xine_event_t xineEvent;

	if ( (pStream = getStreamForId(1, "XineSlaveWrapper::simulateMouseClick() getting one stream")) == NULL )
		return;

	g_pPlutoLogger->Write(LV_STATUS, "XineSlaveWrapper::simulateMouseClick(): simulating mouse click: mx=%d my=%d", X, Y);

	xineEvent.stream = pStream->m_pStream;
	xineEvent.type = XINE_EVENT_INPUT_MOUSE_BUTTON;
	xineEvent.data = &xineInputData;
    xineEvent.data_length = sizeof(xineInputData);

	xineInputData.button = 1;
	xineInputData.x = X;
    xineInputData.y = Y;

	gettimeofday(&xineEvent.tv, NULL);
 	XLockDisplay(XServerDisplay);
	xine_event_send(pStream->m_pStream, &xineEvent);
	XUnlockDisplay(XServerDisplay);
}

void XineSlaveWrapper::simulateKeystroke(int plutoButton)
{
    Window oldWindow;
    int oldRevertBehaviour;

	XLockDisplay(XServerDisplay);
	XGetInputFocus( XServerDisplay, &oldWindow, &oldRevertBehaviour);
    XSetInputFocus( XServerDisplay, windows[m_iCurrentWindow], RevertToParent, CurrentTime );
    XTestFakeKeyEvent( XServerDisplay, XKeysymToKeycode(XServerDisplay, translatePlutoKeySymToXKeySym(plutoButton)), True, 0 );
    XTestFakeKeyEvent( XServerDisplay, XKeysymToKeycode(XServerDisplay, translatePlutoKeySymToXKeySym(plutoButton)), False, 0 );

	if ( oldWindow )
        XSetInputFocus( XServerDisplay, oldWindow, oldRevertBehaviour, CurrentTime );

    XFlush(XServerDisplay);
	XUnlockDisplay(XServerDisplay);
}

KeySym XineSlaveWrapper::translatePlutoKeySymToXKeySym(int plutoButton)
{
	switch ( plutoButton )
	{
		case BUTTON_Up_Arrow_CONST: return XK_Up;
		case BUTTON_Down_Arrow_CONST: return XK_Down;
		case BUTTON_Left_Arrow_CONST: return XK_Left;
		case BUTTON_Right_Arrow_CONST: return XK_Right;
		case BUTTON_Enter_CONST: return XK_Return;

		default:
			g_pPlutoLogger->Write(LV_WARNING, "Translating of the %d pluto key is not handled yet.", plutoButton);
			return 0;
	}
}

static const char *audio_out_types_strs[] = {
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

void XineSlaveWrapper::setOutputSpeakerArrangement(string strOutputSpeakerArrangement)
{
	xine_cfg_entry_t xineConfigEntry;

	g_pPlutoLogger->Write(LV_STATUS, "Setting the audio output speaker arrangement.");

	xine_config_register_enum (m_pXine,	"audio.output.speaker_arrangement", 1, (char **) audio_out_types_strs,
			"Speaker arrangement",
			NULL, 0, NULL, NULL);

	if ( xine_config_lookup_entry(m_pXine, "audio.output.speaker_arrangement", &xineConfigEntry) == 0 )
	{
		g_pPlutoLogger->Write(LV_STATUS, "Could not lookup the current Speaker Arrangement Value");
		return;
	}

	g_pPlutoLogger->Write(LV_STATUS, "Current value: %s", audio_out_types_strs[xineConfigEntry.num_value]);

	g_pPlutoLogger->Write(LV_STATUS, "Trying to set the value to: %s", strOutputSpeakerArrangement.c_str());
	int i = 0;
	while ( audio_out_types_strs[i] != NULL )
	{
		if ( strncmp(strOutputSpeakerArrangement.c_str(	), audio_out_types_strs[i], strlen(audio_out_types_strs[i])) == 0 )
		{
			xineConfigEntry.num_value = i;
			xine_config_update_entry(m_pXine, &xineConfigEntry);

			if ( xine_config_lookup_entry(m_pXine, "audio.output.speaker_arrangement", &xineConfigEntry) == 0 )
			{
				g_pPlutoLogger->Write(LV_STATUS, "Could not lookup the current Speaker Arrangement Value after update.");
				return;
			}

			g_pPlutoLogger->Write(LV_STATUS, "Value changed to: %s", audio_out_types_strs[xineConfigEntry.num_value]);
			return;
		}

		i++;
	}

	return;
}

int XineSlaveWrapper::getDeviceId()
{
	return m_pAggregatorObject->m_dwPK_Device;
}

bool XineSlaveWrapper::isSlimClient()
{
	return m_isSlimClient;
}

void XineSlaveWrapper::setSlimClient(bool isSlimClient)
{
	m_isSlimClient = isSlimClient;
}

