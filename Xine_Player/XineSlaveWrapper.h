//
// C++ Interface: %{MODULE}
//
// Description:
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef XINESLAVEWRAPPER_H
#define XINESLAVEWRAPPER_H

#include <string>
#include <map>

#include <xine.h>
#include <xine/xineutils.h>
#include <xine/video_out.h>
#include <xine/audio_out.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/extensions/XShm.h>
#include <X11/extensions/xf86vmode.h>
#include <X11/cursorfont.h>

using namespace std;
using namespace DCE;

namespace DCE
{
class Xine_Player;
}
/**
This class will handle all the low level work related to the xine-lib control.

@author mtoader@gmail.com
*/
class XineSlaveWrapper
{
    class XineStream
    {
        int                 streamID;
        int                 requestingObject;

        bool                isRendering;
        int                 imgWidth, imgHeight;
        int                 imgXPos, imgYPos;
        int                 playbackSpeed;

        pthread_t           eventLoop;
        xine_stream_t       *stream;
        xine_event_queue_t  *streamEventQueue;

        XineSlaveWrapper    *m_pOwner;

        void setPlaybackSpeed(int speed);
        friend class XineSlaveWrapper;
    };

    friend class DCE::Xine_Player;

    Xine_Player *m_pAggregatorObject;

    string m_strWindowTitle;
    string m_strConfigFile;

    Display     *XServerDisplay;
    Window      windows[2];
    Cursor      cursors[2];
    Pixmap      noCursor;

    int         currentScreen;
    int         currentWindow;

    string      m_strXineVideoDriverName;
    string      m_strXineAudioDriverName;

    x11_visual_t        m_x11Visual;
    xine_t              *m_pXine;
    xine_video_port_t   *m_pXineVideo;
    xine_audio_port_t   *m_pXineAudio;

    map<int, XineStream *> m_mapStreams;

    // HACK:
    XineStream *m_pSameStream;

    // Image related data for a stream
    double  m_dPixelAspect;

    static void *eventProcessingLoop(void *arguments);

    int translate_point(int gui_x, int gui_y, int *video_x, int *video_y);
    bool closeWindow();

    void resume();
    static void destinationSizeCallback(void *data,
                                        int video_width, int video_height,
                                        double video_pixel_aspect,
                                        int *dest_width, int *dest_height,
                                        double *dest_pixel_aspect);

    static void frameOutputCallback(void *data,
                                    int video_width, int video_height,
                                    double video_pixel_aspect,
                                    int *dest_x, int *dest_y,
                                    int *dest_width, int *dest_height,
                                    double *dest_pixel_aspect,
                                    int *win_x, int *win_y);

    int XServerEventProcessor(XineStream *stream, XEvent &event);
    XineStream *getStreamForId(int iStreamID, string strMessageInvalid);

    void     yuy2toyv12 (uint8_t *y, uint8_t *u, uint8_t *v, uint8_t *input, int width, int height);
    uint8_t *yv12torgb (uint8_t *src_y, uint8_t *src_u, uint8_t *src_v, int width, int height);

    void make_snapshot(xine_stream_t *stream, string sFormat, int iWidth, int iHeight, bool bKeepAspect, char*&pData, int &iDataSize);
public:
    XineSlaveWrapper();

    ~XineSlaveWrapper();
    bool createWindow();

    bool createXineLibConnection();
    bool setXineStreamDebugging(int streamID, bool newValue);
    void playStream(string fileName, int streamID, int mediaPosition, int requestingObject);
    static void xineEventListener(void *streamObject, const xine_event_t *event);
    void changePlaybackSpeed(int iStreamID, int iMediaPlaybackSpeed);
    void stopMedia(int iStreamID);
    void restartMediaStream(int iStreamID);
    void pauseMediaStream(int iStreamID);

    void selectNextButton(int iStreamID);
    void selectPrevButton(int iStreamID);
    void pushCurrentButton(int iStreamID);

    string getRenderingWindowName();

    void setXinePlayerObject(Xine_Player *object);

    void getScreenShot(int iStreamID, int iWidth, int iHeight, char *&pData, int &iDataSize, string &sFormat, string &sCMD_Result);
    void selectMenu(int iStreamID, int iMenuType);
};

#endif

