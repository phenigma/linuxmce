/**
 *
 * @file XineSlaveWrapper.h
 * @brief header file for XineSlaveWrapper class
 *
 */

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
#include <X11/extensions/XTest.h>
#include <X11/extensions/xf86vmode.h>
#include <X11/cursorfont.h>

using namespace std;
using namespace DCE;

namespace DCE
{
class Xine_Player;  /**< to be able to use it in declarations */
}
/**
* @brief This class will handle all the low level work related to the xine-lib control.
*/
class XineSlaveWrapper
{
    typedef enum _playbackSpeedEnum
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
	} PlayBackSpeedType;

	class XineStream
    {
        int                 m_iStreamID;
        int                 m_iRequestingObject;

        bool                m_bIsRendering;
        int                 m_iImgWidth, m_iImgHeight;
        int                 m_iImgXPos, m_iImgYPos;
        int                 m_iPlaybackSpeed;

        bool                m_bHasVideo;
        pthread_t           eventLoop;
        xine_stream_t       *m_pStream;
        xine_event_queue_t  *m_pStreamEventQueue;

        XineSlaveWrapper    *m_pOwner;  /** < the Owner */

        /**
        * @brief This will change the playback speed of the current stream
        */
        void setPlaybackSpeed(int speed);
        friend class XineSlaveWrapper;
    };

    friend class DCE::Xine_Player;

    Xine_Player *m_pAggregatorObject;  /** < points the Xine Player */

    string m_sWindowTitle;
    string m_sConfigFile;

	bool 		m_bExitThread;
    Display     *XServerDisplay;
    Window      windows[2];
    Cursor      cursors[2];
    Pixmap      noCursor;

    int         m_iCurrentScreen;
    int         m_iCurrentWindow;

    string      m_sXineVideoDriverName;
    string      m_sXineAudioDriverName;

    x11_visual_t        m_x11Visual;
    xine_t              *m_pXine;
    xine_video_port_t   *m_pXineVideo;
    xine_audio_port_t   *m_pXineAudio;
    xine_post_t         *m_pXineVisualizationPlugin;

    map<int, XineStream *> m_mapStreams;

    /** @warning HACK: */
    XineStream *m_pSameStream;

    double  m_dPixelAspect; /** <  Image related data for a stream */

    /**
     * @brief Hepler function that will preprocess events from XServer.
     */

    static void *eventProcessingLoop(void *arguments);

    /**
     * @brief Will convert a point to coordinates relative to the playback window.
     */

    int translate_point(int gui_x, int gui_y, int *video_x, int *video_y);

    /**
     * @brief This will close the playback window.
     */

    bool closeWindow();

    /**
     * @brief This will resume playback. Not used at this moment.
     */

    void resume();

    /**
     * @brief callbacks required by the xine engine for video playback.
     */

    static void destinationSizeCallback(void *data,
                                        int video_width, int video_height,
                                        double video_pixel_aspect,
                                        int *dest_width, int *dest_height,
                                        double *dest_pixel_aspect);

    /**
     * @see destinationSizeCallback
     */

    static void frameOutputCallback(void *data,
                                    int video_width, int video_height,
                                    double video_pixel_aspect,
                                    int *dest_x, int *dest_y,
                                    int *dest_width, int *dest_height,
                                    double *dest_pixel_aspect,
                                    int *win_x, int *win_y);

    /**
     * @brief memeber function that will do the actual XServer event processing.
     */

    int XServerEventProcessor(XineStream *stream, XEvent &event);

    /**
     * @brief find a stream by ID. Currently this will ignore the ID and always return the current stream.
     */

    XineStream *getStreamForId(int iStreamID, string strMessageInvalid);

	/**
     * @brief map a stream to a stream ID. It will return the previous stream if there was one.
     */
    XineStream *setStreamForId(int iStreamID, XineStream *pNewStream);

    /**
     * @brief conversion function used in the screen shot feature.
     */

    void     yuy2toyv12 (uint8_t *y, uint8_t *u, uint8_t *v, uint8_t *input, int width, int height);

    /**
     * @see yuy2toyv12
     */

    uint8_t *yv12torgb (uint8_t *src_y, uint8_t *src_u, uint8_t *src_v, int width, int height);

    /**
     * @brief Take a screenshot of the currenly playing stream.
     */

    void make_snapshot(xine_stream_t *stream, string sFormat, int iWidth, int iHeight, bool bKeepAspect, char*&pData, int &iDataSize);

	/**
	 * @brief Translate a PlutoKey definition into a XKeySym definition
	 */
	 KeySym translatePlutoKeySymToXKeySym(int plutoButton);

	 bool XineSlaveWrapper::loadXineConfiguration();
public:
    /**
     * @brief constructor
     */
    XineSlaveWrapper();

    /**
     * @brief destructor
     */

    ~XineSlaveWrapper();

    /**
     * @brief create the playback window
     */

    bool createWindow();

    /**
     * @brief create xine lib objects.
     */

    bool createXineLibConnection();

    /**
     * @brief set debugging level.
     */

    bool setXineStreamDebugging(int streamID, bool newValue);

    /**
     * @brief play the stream represented by the fileName.
     */

    bool playStream(string fileName, int streamID, int mediaPosition, int requestingObject);

    /**
     * @brief callback function to listen for events from the xine lib.
     */

    static void xineEventListener(void *streamObject, const xine_event_t *event);

    /**
     * @brief change the stream playback speed.
     */

    void changePlaybackSpeed(int iStreamID, PlayBackSpeedType newSpeed);

	/**
	 * @brief get current playback speed
	 */
	PlayBackSpeedType getPlaybackStream(int iStreamID);

    /**
     * @brief stop the stream playback.
     */

    void stopMedia(int iStreamID);

    /**
     * @brief restart the stream playback.
     */

    void restartMediaStream(int iStreamID);

    /**
     * @brief pause stream playback.
     */

    void pauseMediaStream(int iStreamID);

	/**
	 * @brief send a Xine event to the engine
	 */
	void sendInputEvent(int eventID);

    /**
     * @brief move the button down in a DVD menu.
     */
    void selectNextButton(int iStreamID);

    /**
     * @brief move the button up in a DVD menu.
     */

    void selectPrevButton(int iStreamID);

    /**
     * @brief select the currently selected button in a DVD menu.
     */

    void pushCurrentButton(int iStreamID);

    /**
     * @brief the playback window name (the X property WM_NAME).
     */

    string getRenderingWindowName();

    /**
     * @brief set the owner object of this object.
     */

    void setXinePlayerObject(Xine_Player *object);

    /**
     * @brief public interface to allow taking of a screenshot of the current image.
     */

    void getScreenShot(int iStreamID, int iWidth, int iHeight, char *&pData, int &iDataSize, string &sFormat, string &sCMD_Result);

    /**
     * @brief in a DVD playback this will go to the respective menu (RootMenu, etc.)
     */

    void selectMenu(int iStreamID, int iMenuType);

	/**
	 * @brief simlate a mouse click on the current active window.
	 */
	void simulateMouseClick(int X, int Y);

	/**
	 * @brief simlate a mouse click on the current active window.
	 */
	void simulateKeystroke(int plutoButton);

    /**
     * @brief Fired by a xineStream object when the stream has completed playback
     */
    void playbackCompleted(int iStreamID,bool bWithErrors);

    /**
     * @brief Find current position for stream.
     */
    int getStreamPlaybackPosition(int iStreamID, int &positionTime, int &totalTime);

    /**
     * @brief Enable broadcasting on this stream
     */
    int enableBroadcast(int iStreamID);

	/**
	 * @brief Enable Sound data passthru for this player.
	 */
	void setOutputSpeakerArrangement(string strOutputSpeakerArrangement);

	/**
	 *
	 */
	int getDeviceId();
};

#endif

