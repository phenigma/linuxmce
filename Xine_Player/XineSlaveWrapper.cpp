//
// C++ Implementation: Xine Player DCE Plugin (the Xine lib slave controller)
//
// Description:
//
//
// Author: Toader Mihai Claudiu mtoader@gmail.com, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "DCE/Logger.h"
#include "XineSlaveWrapper.h"
#include "Xine_Player.h"

#include <math.h>

#include <png.h>

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

// Define this here to avoid some dependency.
typedef struct
{
  uint32_t  flags;
  uint32_t  functions;
  uint32_t  decorations;
  int32_t   input_mode;
  uint32_t  status;
} MWMHints;


enum
{
    PLAYBACK_REW_32     = -32000,
    PLAYBACK_REW_16     = -16000,
    PLAYBACK_REW_8      =  -8000,
    PLAYBACK_REW_4      =  -4000,
    PLAYBACK_REW_2      =  -2000,
    PLAYBACK_REW_1      =  -1000,
    PLAYBACK_REW_1_2    =   -500,
    PLAYBACK_REW_1_4    =   -250,

    PLAYBACK_STOP       =      0,

    PLAYBACK_FF_1_4     =    250,
    PLAYBACK_FF_1_2     =    500,
    PLAYBACK_FF_1       =   1000,
    PLAYBACK_FF_2       =   2000,
    PLAYBACK_FF_4       =   4000,
    PLAYBACK_FF_8       =   8000,
    PLAYBACK_FF_16      =  16000,
    PLAYBACK_FF_32      =  32000,
} PlayBackSpeedEnum;

XineSlaveWrapper::XineSlaveWrapper()
    : m_strWindowTitle("xine-slave window (controlled by the DCE Xine Player device)"),
      m_strConfigFile("/usr/local/pluto/bin/xine-slave.conf"),

      XServerDisplay(NULL),
      m_strXineVideoDriverName("xv"),
      m_strXineAudioDriverName("alsa"),

      m_pSameStream(NULL)
{
    currentWindow = 0;
}

XineSlaveWrapper::~XineSlaveWrapper()
{
    closeWindow();
}


static const char noCursorDataDescription[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


/*!
    \fn bool XineSlaveWrapper::createWindow()
 */
bool XineSlaveWrapper::createWindow()
{
    XColor      black; //, dummy;
    XSizeHints      sizeHints;
    MWMHints        wmHints;

    long        propertyValues[1];
    int         completionEvent;
    int         xpos, ypos, width, height;

    double          res_h, res_v;

    if(!XInitThreads())
    {
            g_pPlutoLogger->Write(LV_WARNING, "XInitThreads() failed!");
            return false;
    }

    if((XServerDisplay = XOpenDisplay(getenv("DISPLAY"))) == NULL) {
        // WARN: This is a potential security threat (if hte DISPLAY is wayy too big what happends ?
        g_pPlutoLogger->Write(LV_WARNING, "Could not open X DISPLAY from: %s", getenv("DISPLAY"));
        return false;
    }

    currentScreen   = XDefaultScreen(XServerDisplay);
    xpos            = 10;
    ypos            = 20;
    width           = 720;
    height          = 540;

    XLockDisplay(XServerDisplay);
    windows[0] = XCreateSimpleWindow(XServerDisplay, XDefaultRootWindow(XServerDisplay),
                    xpos, ypos, width, height,
                    1, 0, 0);
    windows[1] = XCreateSimpleWindow(XServerDisplay, XDefaultRootWindow(XServerDisplay),
                    0, 0, (DisplayWidth(XServerDisplay, currentScreen)), DisplayHeight(XServerDisplay, currentScreen),
                    0, 0, 0);

    XSelectInput(XServerDisplay, windows[0],
                        (ExposureMask | ButtonPressMask | KeyPressMask | ButtonMotionMask |
                         StructureNotifyMask | PropertyChangeMask | PointerMotionMask));

    XSelectInput(XServerDisplay, windows[1],
                        (ExposureMask | ButtonPressMask | KeyPressMask | ButtonMotionMask |
                         StructureNotifyMask | PropertyChangeMask | PointerMotionMask));

    XSetStandardProperties(XServerDisplay, windows[0], m_strWindowTitle.c_str(), m_strWindowTitle.c_str(), None, NULL, 0, 0);
    XSetStandardProperties(XServerDisplay, windows[1], m_strWindowTitle.c_str(), m_strWindowTitle.c_str(), None, NULL, 0, 0);

    sizeHints.win_gravity = StaticGravity;
    sizeHints.flags       = PPosition | PSize | PWinGravity;

    XSetWMNormalHints(XServerDisplay, windows[0], &sizeHints);
    XSetWMNormalHints(XServerDisplay, windows[1], &sizeHints);

    Atom XA_DELETE_WINDOW     = XInternAtom(XServerDisplay, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(XServerDisplay, windows[0], &XA_DELETE_WINDOW, 1);
    XSetWMProtocols(XServerDisplay, windows[1], &XA_DELETE_WINDOW, 1);

    Atom XA_NO_BORDER         = XInternAtom(XServerDisplay, "_MOTIF_WM_HINTS", False);
    wmHints.flags       = (1L << 1); // MWM_HINTS_DECORATIONS
    wmHints.decorations = 0;

    XChangeProperty(XServerDisplay, windows[1], XA_NO_BORDER, XA_NO_BORDER, 32,
                PropModeReplace, (unsigned char *) &wmHints,
                5 /* PROP_MWM_HINTS_ELEMENTS */);

    Atom XA_WIN_LAYER    = XInternAtom(XServerDisplay, "_NET_WM_STATE", False);
    propertyValues[0] = 10;
    XChangeProperty(XServerDisplay, windows[1], XA_WIN_LAYER, XA_CARDINAL, 32, PropModeReplace, (unsigned char *)propertyValues, 1);


    XA_WIN_LAYER = XInternAtom(XServerDisplay, "_NET_WM_STATE", False);
    propertyValues[0] = 12;
    XChangeProperty(XServerDisplay, windows[1], XA_WIN_LAYER, XA_CARDINAL, 32, PropModeReplace, (unsigned char *)propertyValues, 1);

    if(XShmQueryExtension(XServerDisplay) == True)
            completionEvent = XShmGetEventBase(XServerDisplay) + ShmCompletion;
    else
            completionEvent = -1;

    noCursor  = XCreateBitmapFromData(XServerDisplay, (DefaultRootWindow(XServerDisplay)), noCursorDataDescription, 8, 8);
    cursors[0] = XCreateFontCursor(XServerDisplay, XC_left_ptr);
    cursors[1] = XCreatePixmapCursor(XServerDisplay, noCursor, noCursor, &black, &black, 0, 0);
    XFreePixmap(XServerDisplay, noCursor);

    XDefineCursor(XServerDisplay, windows[currentWindow], cursors[currentWindow]);
    XMapRaised(XServerDisplay, windows[currentWindow]);

    res_h = (DisplayWidth(XServerDisplay, currentScreen) * 1000 / DisplayWidthMM(XServerDisplay, currentScreen));
    res_v = (DisplayHeight(XServerDisplay, currentScreen) * 1000 / DisplayHeightMM(XServerDisplay, currentScreen));


    m_dPixelAspect = res_v / res_h;

    g_pPlutoLogger->Write(LV_STATUS, "XServer aspect %f", m_dPixelAspect);

    if(fabs(m_dPixelAspect - 1.0) < 0.01)
        m_dPixelAspect = 1.0;

    XSync(XServerDisplay, False);
    XUnlockDisplay(XServerDisplay);

    return true;
}

/**
 *
 *  \fn XineSlaveWrapper::playStream(string fileName, int streamID, int mediaPosition, int requestingObject);
 */
void XineSlaveWrapper::playStream(string fileName, int streamID, int mediaPosition, int requestingObject)
{
    g_pPlutoLogger->Write(LV_STATUS, "Got a play media command for %s", fileName.c_str());

    bool isNewStream = false;
    XineStream *xineStream = getStreamForId(streamID, ""); // HACK: This should be changed to behave properly

    if ( xineStream == NULL )
    {
        isNewStream = true;
        xineStream = new XineStream();
        m_pSameStream = xineStream; // HACK: this code should not be here
        xineStream->m_pOwner = this;
        xineStream->streamID = streamID;
    }
    else
    {
        xine_close(xineStream->stream);
        g_pPlutoLogger->Write(LV_STATUS, "Closed previous stream!");
        xineStream->isRendering = false;
        xineStream->streamID = streamID;
    }

    m_x11Visual.display           = XServerDisplay;
    m_x11Visual.screen            = currentScreen;
    m_x11Visual.d                 = windows[currentWindow];
    m_x11Visual.dest_size_cb      = &destinationSizeCallback;
    m_x11Visual.frame_output_cb   = &frameOutputCallback;
    m_x11Visual.user_data         = xineStream;

    if( isNewStream && (m_pXineVideo = xine_open_video_driver(m_pXine, m_strXineVideoDriverName.c_str(), XINE_VISUAL_TYPE_X11, (void *) &m_x11Visual)) == NULL)
    {
        g_pPlutoLogger->Write(LV_WARNING, "I'm unable to initialize Xine's '%s' video driver. Giving up.", m_strXineVideoDriverName.c_str());
        return;
    }

    if ( isNewStream &&  (m_pXineAudio = xine_open_audio_driver(m_pXine, m_strXineAudioDriverName.c_str(), NULL)) == NULL )
    {
        g_pPlutoLogger->Write(LV_WARNING, "I'm unable to initialize Xine's '%s' audio driver.", m_strXineAudioDriverName.c_str());
        return;
    }

    if ( isNewStream && (xineStream->stream = xine_stream_new(m_pXine, m_pXineAudio, m_pXineVideo)) == NULL )
    {
        g_pPlutoLogger->Write(LV_WARNING, "Could not create stream (wanted to play: %s)", fileName.c_str());
        delete xineStream;
        return; // TODO make it return false.
    }

    xineStream->requestingObject = requestingObject;

    if (isNewStream )
        m_mapStreams[streamID] = xineStream; // TODO: check for leaks (if someone will send me multiple playStream with the same stream ID it will leak memory)

    if ( isNewStream )
    {
        xineStream->streamEventQueue = xine_event_new_queue(xineStream->stream);
        xine_event_create_listener_thread(xineStream->streamEventQueue, xineEventListener, xineStream);
    }

    xine_port_send_gui_data(m_pXineVideo, XINE_GUI_SEND_VIDEOWIN_VISIBLE, (void *) 0);
    xine_port_send_gui_data(m_pXineVideo, XINE_GUI_SEND_DRAWABLE_CHANGED, (void *) windows[1 - currentWindow]);


    if( xine_open(xineStream->stream, fileName.c_str()))
    {
        g_pPlutoLogger->Write(LV_STATUS, "Opened..... %s", fileName.c_str());

        xine_post_wire_audio_port( xine_get_audio_source(xineStream->stream), m_pXineAudio);

         if ( xineStream->imgWidth == 0)
             xineStream->imgWidth++;

         if ( xineStream->imgHeight == 0)
             xineStream->imgHeight++;

        g_pPlutoLogger->Write(LV_STATUS, "Got image dimensions: %dx%d", xineStream->imgWidth, xineStream->imgWidth );

        xine_port_send_gui_data(m_pXineVideo, XINE_GUI_SEND_DRAWABLE_CHANGED, (void *) windows[currentWindow]);
        xine_port_send_gui_data(m_pXineVideo, XINE_GUI_SEND_VIDEOWIN_VISIBLE, (void *) 1);

        xineStream->isRendering = true;

        if ( isNewStream )
            pthread_create(&xineStream->eventLoop, NULL, eventProcessingLoop, xineStream);

        if ( xine_play(xineStream->stream, 0, 0) )
        {
            g_pPlutoLogger->Write(LV_STATUS, "Playing... %s", fileName.c_str());
        }
    }
}

bool XineSlaveWrapper::closeWindow()
{
    if ( XServerDisplay != NULL )
    {
        XLockDisplay(XServerDisplay);

        XUnmapWindow(XServerDisplay,  windows[currentWindow]);

        XFreeCursor(XServerDisplay, cursors[0]);
        XFreeCursor(XServerDisplay, cursors[1]);

        XDestroyWindow(XServerDisplay,  windows[0]);
        XDestroyWindow(XServerDisplay,  windows[1]);

        XUnlockDisplay(XServerDisplay);

        XCloseDisplay (XServerDisplay);

        XServerDisplay = NULL;
    }

    return true;
}

bool XineSlaveWrapper::createXineLibConnection()
{
    m_pXine = xine_new();
    xine_config_load(m_pXine, m_strConfigFile.c_str());
    xine_init(m_pXine);

    return true;
}


/**
 * xine Callbacks.
 */

void XineSlaveWrapper::destinationSizeCallback(void *data, int video_width, int video_height, double video_pixel_aspect,
                                               int *dest_width, int *dest_height,
                                               double *dest_pixel_aspect)
{
    XineStream *stream = (XineStream*)data;

//     if( ! stream->isRendering )
//         g_pPlutoLogger->Write(LV_STATUS, "Destination size callback called (not rendering)!");

    *dest_width        = stream->imgWidth + (stream->isRendering ? 0 : 1);
    *dest_height       = stream->imgHeight + (stream->isRendering ? 0 : 1);
    *dest_pixel_aspect = stream->m_pOwner->m_dPixelAspect;
}

void XineSlaveWrapper::frameOutputCallback(void *data, int video_width, int video_height, double video_pixel_aspect,
                                          int *dest_x, int *dest_y, int *dest_width, int *dest_height,
                                          double *dest_pixel_aspect,
                                          int *win_x, int *win_y)
{
    XineStream *stream = (XineStream*)data;

//     if( ! stream->isRendering)
//         g_pPlutoLogger->Write(LV_STATUS, "Framer Output callback called (not rendering).");

    *dest_x            = 0;
    *dest_y            = 0;
    *win_x             = stream->imgXPos;
    *win_y             = stream->imgYPos;
    *dest_width        = stream->imgWidth + (stream->isRendering ? 0 : 1);
    *dest_height       = stream->imgHeight + (stream->isRendering ? 0 : 1);
    *dest_pixel_aspect = stream->m_pOwner->m_dPixelAspect;
}


/*!
    \fn XineSlaveWrapper::xineEventListener(void *userData)
 */
void XineSlaveWrapper::xineEventListener(void *userData, const xine_event_t *event)
{
    XineStream *xineStream = (XineStream *)userData;

    switch(event->type)
    {
        case XINE_EVENT_UI_PLAYBACK_FINISHED:
            //report_mrl_and_title(NULL);
            // clear_tracks();

            // menu_snapshots = end_menu = 0;
/*            if (repeat)
            {
                xine_play(stream, 0, 0);
            }
            else if (mrl[mrl_n+1] != NULL)
                play_mrl(mrl[++mrl_n]);
            else
                send_event(XINE_SE_PLAYBACK_FINISHED, "");*/
            g_pPlutoLogger->Write(LV_WARNING, "Playback finished for stream: %d", xineStream->streamID);
            xineStream->isRendering = false;
            break;

        case XINE_EVENT_PROGRESS:
/*          xine_progress_data_t *pevent = (xine_progress_data_t *) event->data;

            send_event(XINE_SE_PROGRESS, "%s [%d%%]",  pevent->description, pevent->percent);       */
            g_pPlutoLogger->Write(LV_WARNING, "Playback is running along.");
            break;

        case XINE_EVENT_UI_NUM_BUTTONS:
            {
                int iButtons = ((xine_ui_data_t *)event->data)->num_buttons;

                if ( xineStream->m_pOwner->m_pAggregatorObject )
                    xineStream->m_pOwner->m_pAggregatorObject->FireMenuOnScreen(xineStream->requestingObject, xineStream->streamID, iButtons != 0);
            }
            break;

        case XINE_EVENT_UI_SET_TITLE:
            /*      {
                char volume[256];
                int dummy;
                xine_ui_data_t *data = (xine_ui_data_t *) event->data;
                if(sscanf(data->str, "DVD Navigator: Menu, %s", volume)==1 ||
                    sscanf(data->str, "DVD Menu, %s", volume)==1 ||
                    sscanf(data->str, "DVD Root Menu, %s", volume)==1)
                {
                    if( !cur_mrl || !strchr(cur_mrl,TITLE_DELIMITER) )
                    send_event(XINE_SE_DVD_TITLE, volume);
                }
                if(data->str && sscanf(data->str,"Title %d, Chapter %d",&dummy,&dvd_chapter) == 2) {
                    send_event(XINE_SE_CHAPTER, "%d", dvd_chapter);
                } else {
                    dvd_chapter = 0;
                }
                }*/
            g_pPlutoLogger->Write(LV_STATUS, "Event ui set title");
            break;

        case XINE_EVENT_INPUT_MOUSE_MOVE:
            // ignored at the moment;
            break;

        default:
            g_pPlutoLogger->Write(LV_STATUS, "Got unprocessed Xine playback event: %d", event->type);
            break;
    }
}

bool XineSlaveWrapper::setXineStreamDebugging(int streamID, bool newValue)
{
    XineStream *xineStream = getStreamForId(streamID, "Trying to set debugging level for and invalid stream: (%d)");

    if ( xineStream == NULL )
        return false;

    if ( newValue )
    {
        xine_set_param(xineStream->stream, XINE_PARAM_VERBOSITY, XINE_VERBOSITY_DEBUG);
    }
    else
    {
        xine_set_param(xineStream->stream, XINE_PARAM_VERBOSITY, XINE_VERBOSITY_NONE);
    }

    return true;
}


void XineSlaveWrapper::resume()
{
  if (m_pXineVideo != NULL) {
    int count = 50;

    while( --count &&
          !xine_port_send_gui_data(m_pXineVideo, XINE_GUI_SEND_DRAWABLE_CHANGED, (void *) windows[currentWindow]) )
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
    XineStream *stream = (XineStream*)arguments;
    Bool checkResult;

    XEvent  event;
    while ( true )
    {
        checkResult = XCheckWindowEvent(stream->m_pOwner->XServerDisplay, stream->m_pOwner->windows[stream->m_pOwner->currentWindow], ~0, &event);

        if ( checkResult == True )
            stream->m_pOwner->XServerEventProcessor(stream, event);

//         if ( stream->playbackSpeed != )
        usleep(200);
    }

    return NULL;
}

int XineSlaveWrapper::XServerEventProcessor(XineStream *stream, XEvent &event)
{
    Atom  XA_DELETE_WINDOW;

    switch(event.type)
    {
        case ClientMessage:
        {
            XA_DELETE_WINDOW = XInternAtom(XServerDisplay, "WM_DELETE_WINDOW", False);

            if((unsigned)event.xclient.data.l[0] == XA_DELETE_WINDOW)
                stream->isRendering = false;
            break;
        }

        case MotionNotify:
        {
            XMotionEvent *mevent = (XMotionEvent *) &event;
            int x, y;

            if(stream->m_pOwner->translate_point(mevent->x, mevent->y, &x, &y)) {
                xine_event_t       xineEvent;
                xine_input_data_t  xineInput;

                xineEvent.type        = XINE_EVENT_INPUT_MOUSE_MOVE;
                xineEvent.stream      = stream->stream;
                xineEvent.data        = &xineInput;
                xineEvent.data_length = sizeof(xineInput);
                gettimeofday(&xineEvent.tv, NULL);

                xineInput.button      = 0;
                xineInput.x           = x;
                xineInput.y           = y;
                xine_event_send(stream->stream, &xineEvent);
            }
            break;
        }

        case Expose:
        {
            XExposeEvent *exposeEvent = (XExposeEvent *)&event;
            if(exposeEvent->count != 0)
                break;

            if (stream->m_pOwner->m_pXineVideo)
                xine_port_send_gui_data(stream->m_pOwner->m_pXineVideo, XINE_GUI_SEND_EXPOSE_EVENT, exposeEvent);

            break;
        }

        case ConfigureNotify:
        {
            XConfigureEvent *cev = (XConfigureEvent *) &event;
            Window           tmp_win;

            stream->imgWidth    = cev->width;
            stream->imgHeight   = cev->height;

            if((cev->x == 0) && (cev->y == 0))
            {
                XTranslateCoordinates(cev->display, cev->window, DefaultRootWindow(cev->display), 0, 0, &stream->imgXPos, &stream->imgYPos, &tmp_win);
            }
            else
            {
                stream->imgXPos = cev->x;
                stream->imgYPos = cev->y;
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
    /* driver implements gui->video coordinate space translation, use it */
    *video_x = rect.x;
    *video_y = rect.y;
    return 1;
  }

  return 1;
  /* Driver cannot convert gui->video space, fall back to old code... */

//   if(XGetGeometry(XServerDisplay, windows[currentWindow], &rootwin, &xwin, &ywin, &wwin, &hwin, &bwin, &dwin) == BadDrawable)
//     return 0;

  /* Scale co-ordinate to image dimensions. */
/*  height_scale = (float) m_imageHeight / (float) hwin;
  width_scale  = (float) m_imageWidth / (float) wwin;
  aspect       = (float) m_imageWidth / (float) m_imageHeight;
  if(((float) wwin / (float) hwin) < aspect) {
    scale      = width_scale;
    xf         = (float) gui_x * scale;
    yf         = (float) gui_y * scale;
    hwin       = (int) (hwin * scale);*/
    /* FIXME: The 1.25 should really come from the NAV packets. */
/*    *video_x   = (int) (xf * 1.25 / aspect);
    *video_y   = (int) (yf - ((hwin - m_imageHeight) / 2));
  }
  else {
    scale    = height_scale;
    xf       = (float) gui_x * scale;
    yf       = (float) gui_y * scale;
    wwin     = (int) (wwin * scale);*/
    /* FIXME: The 1.25 should really come from the NAV packets. */
//     *video_x = (int) ((xf - ((wwin - m_imageWidth) / 2)) * 1.25 / aspect);
//     *video_y = (int) yf;
//   }
//
//   return 1;
}



/*!
    \fn XineSlaveWrapper::changePlaybackSpeed(int iStreamID, int iMediaPlaybackSpeed)
 */
void XineSlaveWrapper::changePlaybackSpeed(int iStreamID, int iMediaPlaybackSpeed)
{
    XineStream *stream = getStreamForId(iStreamID, "Can't set the speed of a non existent stream (%d)!");

    if ( stream == NULL)
        return;

    int xineSpeed = XINE_SPEED_PAUSE;
    switch ( iMediaPlaybackSpeed )
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
            g_pPlutoLogger->Write(LV_WARNING, "Don't know how to handle speed: %d", iMediaPlaybackSpeed);
            break;
    }

    if ( (xineSpeed == XINE_SPEED_PAUSE && iMediaPlaybackSpeed == 0) || xineSpeed != XINE_SPEED_PAUSE)
        xine_set_param(stream->stream, XINE_PARAM_SPEED, xineSpeed);
}



/*!
    \fn XineSlaveWrapper::stopMedia(int iStreamID)
 */
void XineSlaveWrapper::stopMedia(int iStreamID)
{
    XineStream *stream = getStreamForId(iStreamID, "Can't stop the playback of a non existent stream (%d)!");

    if ( stream == NULL )
        return;

    xine_stop(stream->stream);
    //xine_close(stream->stream);
}


/*!
    \fn XineSlaveWrapper::restartMediaStream(int iStreamID)
 */
void XineSlaveWrapper::restartMediaStream(int iStreamID)
{
/*    XineStream *stream;

    stream = m_mapStreams[iStreamID];

    if ( stream == NULL )
    {
        g_pPlutoLogger->Write(LV_WARNING, "Can't restart a non existent media stream!");
        return;
    }

    stream->setPlaybackSpeed(PLAYBACK_FF_1);*/

    changePlaybackSpeed(iStreamID, PLAYBACK_FF_1);
}


/*!
    \fn XineSlaveWrapper::pauseMediaStream(int iStreamID)
 */
void XineSlaveWrapper::pauseMediaStream(int iStreamID)
{
//     XineStream *stream;
//
//     stream = m_mapStreams[iStreamID];
//
//     if ( stream == NULL )
//     {
//         g_pPlutoLogger->Write(LV_WARNING, "Can't pause a non existent media stream!");
//         return;
//     }
//
//     stream->setPlaybackSpeed(PLAYBACK_STOP);

    changePlaybackSpeed(iStreamID, PLAYBACK_STOP);
}


void XineSlaveWrapper::XineStream::setPlaybackSpeed(int speed)
{
    // TODO: Make this proper.
//     changePlaybackSpeed(iS)
}

string XineSlaveWrapper::getRenderingWindowName()
{
    return m_strWindowTitle;
}

void XineSlaveWrapper::selectNextButton(int iStreamID)
{
    XineStream *stream = getStreamForId(iStreamID, "Can't navigate on an non existent stream (%d)!");

    if (stream == NULL)
        return;

    g_pPlutoLogger->Write(LV_STATUS, "Selecting next hot spot on the stream %d", iStreamID);

    xine_event_t event;

    event.type        = XINE_EVENT_INPUT_UP;
    event.stream      = stream->stream;
    event.data        = NULL;
    event.data_length = 0;
    gettimeofday(&event.tv, NULL);

    xine_event_send(stream->stream, &event);
}

void XineSlaveWrapper::selectPrevButton(int iStreamID)
{
    XineStream *stream = getStreamForId(iStreamID, "Can't navigate on an non existent stream (%d)!");

    if (stream == NULL)
        return;

    g_pPlutoLogger->Write(LV_STATUS, "Selecting next hot spot on the stream %d", iStreamID);

    xine_event_t event;

    event.type        = XINE_EVENT_INPUT_DOWN;
    event.stream      = stream->stream;
    event.data        = NULL;
    event.data_length = 0;
    gettimeofday(&event.tv, NULL);

    xine_event_send(stream->stream, &event);
}

void XineSlaveWrapper::pushCurrentButton(int iStreamID)
{
    XineStream *stream = getStreamForId(iStreamID, "Can't push the buttons on invalid stream (%d)");

    if (stream == NULL)
        return;

    g_pPlutoLogger->Write(LV_STATUS, "Selecting next hot spot on the stream %d", iStreamID);

    xine_event_t event;

    event.type        = XINE_EVENT_INPUT_SELECT;
    event.stream      = stream->stream;
    event.data        = NULL;
    event.data_length = 0;
    gettimeofday(&event.tv, NULL);

    xine_event_send(stream->stream, &event);
}

void XineSlaveWrapper::setXinePlayerObject(Xine_Player *object)
{
    m_pAggregatorObject = object;
}

void XineSlaveWrapper::getScreenShot(int iStreamID, int iWidth, int iHeight, char *&pData, int &iDataSize, string &sFormat, string &sCMD_Result)
{
    XineStream *stream;

    if ( iStreamID != 0 )
    {
        stream = m_mapStreams[iStreamID];

        if ( stream == NULL )
        {
            g_pPlutoLogger->Write(LV_WARNING, "I need a valid stream to be able to make screenshots.");
            return;
        }
    }
    else
    {
        if ( m_mapStreams.size() == 0 )
        {
            g_pPlutoLogger->Write(LV_WARNING, "No streams are defined. Ignoring command!");
            return;
        }

        stream = m_mapStreams.begin()->second;
    }

    sFormat = "JPG";
    g_pPlutoLogger->Write(LV_STATUS, "Making snapshot");
    make_snapshot(stream->stream, sFormat.c_str(), iWidth, iHeight, true, pData, iDataSize);
}

void XineSlaveWrapper::make_snapshot(xine_stream_t *stream, string sFormat, int iWidth, int iHeight, bool bKeepAspect, char*&pData, int &iDataSize)
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

    int         imageWidth, imageHeight, imageRatio, imageFormat;
    uint8_t     *imageData;

    g_pPlutoLogger->Write(LV_STATUS, "Getting frame info");
    xine_get_current_frame(stream, &imageWidth, &imageHeight, &imageRatio, &imageFormat, NULL);
    g_pPlutoLogger->Write(LV_STATUS, "Got %d %d %d %d", imageWidth, imageHeight, imageRatio, imageFormat);

    imageData = (uint8_t*)malloc ((imageWidth+8) * (imageHeight+1) * 2);

    g_pPlutoLogger->Write(LV_STATUS, "Getting frame data");
    xine_get_current_frame (stream, &imageWidth, &imageHeight, &imageRatio, &imageFormat, imageData);
    g_pPlutoLogger->Write(LV_STATUS, "Got %d %d %d %d", imageWidth, imageHeight, imageRatio, imageFormat);

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

    g_pPlutoLogger->Write(LV_STATUS, "Converted to YV12");
    // convert to RGB
    // keep the yv12Data array around to be able to delete it
    uint8_t *yv12Data = imageData;

    // this function will allocate the output parameter
    imageData = yv12torgb (
                    imageData,
                    imageData + imageWidth * imageHeight,
                    imageData + imageWidth * imageHeight * 5 / 4,
                    imageWidth, imageHeight);
    g_pPlutoLogger->Write(LV_STATUS, "Converted to RGB!");
    free(yv12Data);


    double outputRatio;
    double currentRatio;


    g_pPlutoLogger->Write(LV_STATUS, "Temp data was freed here!");
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
        case XINE_VO_ASPECT_DONT_TOUCH:  /* probably non-mpeg stream => don't touch aspect ratio */
            outputRatio = (double) imageWidth / (double) imageHeight;
            break;

        default:
            g_pPlutoLogger->Write(LV_WARNING, "Unknown aspect ratio for image (%d) using 4:3", imageRatio);

        case XINE_VO_ASPECT_4_3:         /* 4:3             */
            outputRatio = 4.0 / 3.0;
            break;
    }

    double f = outputRatio / ((double) imageWidth / (double) imageHeight);

    g_pPlutoLogger->Write(LV_STATUS, "The report between the current ration and the output ratio is %f", f);
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

    FILE * file;
    file = fopen("/tmp/file.png", "wb");

    int BitsPerColor;
    png_bytepp image_rows;

    image_rows = new png_bytep[imageHeight];
    for (int i = 0; i < imageHeight; i++)
        image_rows[i] = (unsigned char *) imageData + i * imageWidth * 3;

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop png_info = png_create_info_struct(png_ptr);

    png_init_io(png_ptr, file);

    png_set_filter(png_ptr, 0, PNG_FILTER_NONE);
    png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);

    BitsPerColor = 8;

    png_set_IHDR(
            png_ptr,
            png_info,
        imageWidth, imageHeight, 8,
        PNG_COLOR_TYPE_RGB,
        PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(png_ptr, png_info);
    png_write_image(png_ptr, image_rows);
    png_write_end(png_ptr, png_info);

    delete [] image_rows;
    png_destroy_write_struct(&png_ptr, &png_info);
    fclose(file);

    int fileDescriptor = open("/tmp/file.png", O_RDONLY);

    if ( fileDescriptor == -1 )
    {
        g_pPlutoLogger->Write(LV_WARNING, "Couldn't open the temp file /tmp/file.png. The error was: %s", strerror(errno));
        return;
    }

    struct stat fileStat;
    fstat(fileDescriptor, &fileStat);

    pData = new char[fileStat.st_size];
    iDataSize = fileStat.st_size;

    read(fileDescriptor, pData, fileStat.st_size);
    close(fileDescriptor);

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

    for (j = 0; j < w2; j++) {
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

      r = (1.1644 * y) + (1.5960 * v);
      g = (1.1644 * y) - (0.3918 * u) - (0.8130 * v);
      b = (1.1644 * y) + (2.0172 * u);

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

XineSlaveWrapper::XineStream *XineSlaveWrapper::getStreamForId(int iStreamID, string strMessageInvalid)
{
    // HACK: use the same stream regardless of the StreamID;

//     if ( m_pSameStream == NULL )
//         m_pSameStream = new XineStream();;

    return m_pSameStream;

/*    XineStream *stream;

    stream = m_mapStreams[iStreamID];

    if ( stream == NULL )
    {
        g_pPlutoLogger->Write(LV_WARNING, strMessageInvalid.c_str(), iStreamID);
        return NULL;
    }

    return stream;*/
}

// void XineSlaveWrapper::closePlaybackStream(int iStreamID)
// {
//     XineStream *stream;
//
//     stream = m_mapStreams[iStreamID];
//
//     if ( stream == NULL )
//     {
//         g_pPlutoLogger->Write(LV_WARNING, "Can't close and non existent stream!");
//         return;
//     }
// //     xine_close(stream->stream);
// }

void XineSlaveWrapper::selectMenu(int iStreamID, int iMenuType)
{
    XineStream *xineStream = getStreamForId(iStreamID, "Can't select menu for a nonexistent stream!");
    if ( xineStream == NULL )
        return;

    xine_event_t xine_event;

    xine_event.type        = XINE_EVENT_INPUT_MENU3;
    xine_event.data_length = 0;
    xine_event.data        = NULL;
    xine_event.stream      = xineStream->stream;
    gettimeofday(&xine_event.tv, NULL);

    xine_event_send(xineStream->stream, &xine_event);

}
