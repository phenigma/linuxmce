/**
 *
 * @file MediaPluginInfo.h
 * @brief documentation
 * @todo ask
 *
 */

#ifndef MediaPluginInfo_h
#define MediaPluginInfo_h

#include "DCE/Logger.h"
#include <deque>

class MediaFile;
void operator+= (deque<MediaFile *> &dTarget, deque<MediaFile *> &dAdditional);

namespace DCE
{
    enum SourceType {
        st_RemovableMedia,  /** The media is coming from a removable disc */
        st_Storage,         /** The media is stored on the core */
        st_Broadcast,       /** Transmission from one DCE to another */
        st_Transmission     /** The media is coming from an external transmission, like Radio or Internet Radio */
    };

#define MEDIASTREAM_TYPE_GENERIC        0
#define MEDIASTREAM_TYPE_CD             1
#define MEDIASTREAM_TYPE_DVD            2

    // Specific to plugins
#define MEDIASTREAM_TYPE_MYTHTV         100


    /** @brief
     *
     * A Media Plugin is derived from the MediaPluginBase abstract class.  When it registers, it passes in a MediaPluginInfo pointer
     * indicating what type of media it can play.  It may register several times with different types of media and different capabilities.
     * So there exists 1 MediaPluginInfo for each combination of MediaPlugin and MediaType (ie: xine/audio, xine/video, myth/tv, etc.)
     */
    class MediaPluginInfo
    {
    public:
        int m_PK_MediaType;

        bool    m_bUsesRemovableMedia,      /** True means it can play from a removable drive.
                                                For example with MediaType DVD, true means it can play a DVD disc. */

        m_bCanStoreOnServer,        /** True means it can play stored media from the server */

        m_bCanBroadcastInHouse,     /** Means it can be broadcast within the house, using one of the protocols
                                        in the send/receive lists below  */

        m_bIsExternalTransmission,      /** True means it is an external transmissions, like Radio.
                                            Not related to internal broadcasting between devices */

        m_bCanJumpPosition;         /** True means the device can save its position, and return to it, or have another device
                                    continue the stream where this left off  */

        int m_iPK_DesignObj;            /** What remote control to use for this type of media */

        list<class MediaDevice *> m_listMediaDevice;    /** Which Media Devices it is representing.  This can be empty.
                                            Then the media handler will give preference to a plug-in that specifically represents a device */

        list<class EntertainArea *> m_listEntertainArea;    /** Which entertainment areas it can play this media on.
                                                                Normally this would be all the entertainment areas corresponding to m_listMediaDevice.
                                                                But in whole house audio it can be more complex. */

        list<string> m_listExtensions;  /** What file extensions it can play for this type of media (ie mp3, wav, vob, etc.) */

        list<int> m_PK_Broadcast_Send;  /** What broadcast formats it supports to send */

        list<int> m_PK_Broadcast_Receive;   /** What broadcast formats it supports to receive */

        bool m_bUsesDevices;            /** If the devices it represents are DCE, or 'dumb' devices, like infrared a/v gear */

        class MediaPluginBase *m_pMediaPluginBase;  /** The actual handler */

        class Command_Impl *m_pCommand_Impl;  /** The same handler, but we need the v-table for the Command_Impl so we can pass messages
                                                  into it's received dcemessage */

        /** @brief A constructor for a plug-in that wants to specify all this stuff manually */

    MediaPluginInfo(class MediaPluginBase *pMediaPluginBase,class Command_Impl *pCommand_Impl)
    { m_pMediaPluginBase=pMediaPluginBase; m_pCommand_Impl=pCommand_Impl; m_iPK_DesignObj=0; };

        /** @brief
     * This will perform some standard tasks commonly needed to setup a plug-in.  First, if PK_DeviceTemplate is specified,
         * it will add all devices of that type to this plug-in.  If the media type has associated extensions, and none were already added,
     * it will add the ones from the database.
         * It assigns all the default values for bUsesRemovableMedia, m_bCanStoreOnServer, m_bCanBroadcastInHouse, m_bIsExternalTransmission.
         * If the plug-in isn't associated with any entertainment areas, it will add itself to all entertainment areas where it's devices
     * are located.  Then it registers
         * The calling plug-in can change values after the fact if it deviates from the defaults (ie doesn't support something, or uses different
     * file extensions)
         * If the plug-in wants to change the the entertainment areas, it should set bDontRegister to true, change these values, then register.
     * At registration the media handler associates all the plug-ins with their media types.  Broadcast formats can be set after registration.
    */

        MediaPluginInfo(class MediaPluginBase *pMediaPluginBase,class Command_Impl *pCommand_Impl,int PK_MediaType,int PK_DeviceTemplate,bool bCanJumpPosition,bool bUsesDevices,bool bDontRegister=false);
    };

    typedef list<MediaPluginInfo *> List_MediaPluginInfo;


    /** @brief
     * An instance of media, such as a DVD or a CD.  If 2 people start watching the same DVD individually, that will be 2 media streams,
     * because they are independent.
     * However, if 1 person starts watching and another joins in, or the media is broadcast, it becomes a single stream with multiple destinations.  In this case, all
     * the destinations are seeing the same thing, and the appropriate plug-in must concern itself with proper audio/video sync,
     * and ensure that if 1 person changes the stream, they all stay in sync.
     * It is possible to do this even with non-stored media, like Broadcast TV.
	 * Whatever media plugin first created the media is considered the 'responsible' plug-in.  A MediaStream can contain a 'playlist' or group
	 * of files or media clips.  However, all must be of the same type since there is only 1 MediaPluginInfo for a stream.  You cannot, therefore, mix
	 * audio and video clips in a playlist, or some stored audio (WAV, MP3, etc) with CD's or DVD's.
    */
    class MediaStream
    {
        int m_iStreamID; /** A unique number to identify this stream -- cannot change this */

    public:
        class MediaPluginInfo *m_pMediaPluginInfo; /** Which handler has primary responsibility for this media stream */

        class OH_Orbiter *m_pOH_Orbiter;    /** Which orbiter started this stream in the first place */

        map<int,class EntertainArea *> m_mapEntertainArea; /** The entertainment areas where this stream is playing */

        /**
         * As more 'play media' commands come in to this stream, it will add them to the queue so the user can save as a play list.
         * If it's a mounted media, like dvd, that won't happen
         */
        deque<MediaFile *>   m_dequeMediaFile;        /** The filenames we're playing */
        int             m_iDequeMediaFile_Pos;   /** The play position in the m_dequeFilename deque. */
        int             m_iPK_Playlist;          /** the ID of the playlist. nonZero if the playlist was loaded from database, zero otherwise. */
        string          m_sPlaylistName;       /** the name of the playlist which was loaded from the database. */

        // TODO: In the future check to see how to manage the file which is playing from which position.
        // For the moment we should just ignore it.
        int             m_iStoppedAtPosition;

        char *m_pPictureData;
        int m_iPictureSize;
        MediaDevice *m_pMediaDevice;      /** The source device */
        int m_iPK_MediaType;        /** The type of media */
        int m_iPK_DesignObj_Remote; /** What screen to use as the remote control */
        bool m_bPlaying;        /** True if the media is now playing */
        int m_iPK_Users;        /** Who started the media */
        bool m_bFollowMe;       /** True if the media is supposed to follow the above user */
		int m_iOrder;  /** This is used for the floorplans to order and color code the streams */

        enum SourceType m_eSourceType;  /** Where the media is coming from */

        string m_sMediaDescription;     /** Some text the plug-in populates to describe the media.  "The Patriot", "Beatles Anthology" are examples.
                        For TV or radio this is the channel */

        string m_sSectionDescription;   /** Describe where we are in the media, such as a song title, chapter, track, etc.
                        For TV or radio this is the name of the show */

        string m_sMediaSynopsis;    /** A description of what's playing, such as the tv show description */

        string m_sTimecode;     /** The handler may populate this with a timecode */

        class MediaPosition *m_pMediaPosition; /** Where we are in the media stream */

        /** @brief constructor*/
        MediaStream(class MediaPluginInfo *pMediaPluginInfo, MediaDevice *pMediaDevice, int PK_DesignObj_Remote, int PK_Users,enum SourceType sourceType,int iStreamID);

        /** @brief virtual destructor */
        virtual ~MediaStream();

        virtual int GetType() { return MEDIASTREAM_TYPE_GENERIC; }
        virtual void UpdateDescriptions() { m_sMediaDescription="Generic Media"; m_sSectionDescription=""; m_sMediaSynopsis=""; m_sTimecode=""; }
        int m_iStreamID_get() { return m_iStreamID; }

        void SetPicture(char *pPictureData,int iPictureSize) { delete[] m_pPictureData; m_pPictureData=pPictureData; m_iPictureSize=iPictureSize; }

        virtual string GetFilenameToPlay( string defaultFileName = "" );
        virtual void SetPlaylistPosition(int position);
        virtual void ChangePositionInPlaylist(int iHowMuch);
        virtual void DumpPlaylist();
        virtual void ClearPlaylist();
        virtual bool HaveMoreInQueue();
    };

    typedef map<int,MediaStream *> MapMediaStream;

    class BoundRemote
    {
    public:
        class OH_Orbiter *m_pOH_Orbiter;
        class EntertainArea *m_pEntertainArea;
        class Media_Plugin *m_pMedia_Plugin;
        string m_sPK_DesignObj_Remote,m_sPK_DesignObj_GraphicImage,m_sOptions;
        int m_iPK_Text_Description,m_iPK_Text_Section,m_iPK_Text_Synopsis,m_iPK_Text_Timecode;

        BoundRemote(class Media_Plugin *pMedia_Plugin) { m_iPK_Text_Description=m_iPK_Text_Timecode=0; m_pMedia_Plugin=pMedia_Plugin;}
        void UpdateOrbiter(MediaStream *pMediaStream);
    };

    typedef map<int,BoundRemote *> MapBoundRemote;
}

#endif
