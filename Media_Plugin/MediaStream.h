#ifndef MediaStream_h
#define MediaStream_h

#include <map>
#include <deque>

#include "MediaAttributes.h"
#include "MediaHandlerInfo.h"
#include "DeviceData_Router.h"


using namespace std;

namespace DCE
{

    /** @brief
     * An instance of media, such as a DVD or a CD.  If 2 people start watching the same DVD individually, that will be 2 media streams,
     * because they are independent.
     * However, if 1 person starts watching and another joins in, or the media is broadcast, it becomes a single stream with multiple destinations.  In this case, all
     * the destinations are seeing the same thing, and the appropriate plug-in must concern itself with proper audio/video sync,
     * and ensure that if 1 person changes the stream, they all stay in sync.
     * It is possible to do this even with non-stored media, like Broadcast TV.
     * Whatever media plugin first created the media is considered the 'responsible' plug-in.  A MediaStream can contain a 'playlist' or group
     * of files or media clips.  However, all must be of the same type since there is only 1 MediaHandlerInfo for a stream.  You cannot, therefore, mix
     * audio and video clips in a playlist, or some stored audio (WAV, MP3, etc) with CD's or DVD's.
    */
    class MediaStream
    {
        int m_iStreamID; /** A unique number to identify this stream -- cannot change this */

    public:
        class MediaHandlerInfo *m_pMediaHandlerInfo; /** Which handler has primary responsibility for this media stream */

		class OH_Orbiter *m_pOH_Orbiter_StartedMedia;    	   /** Which orbiter started this stream in the first place */
		class OH_Orbiter *m_pOH_Orbiter_OSD;    	   /** Which orbiter is the on-sceren display */

        map<int, class EntertainArea *> m_mapEntertainArea; /** The entertainment areas where this stream is playing */

        /**
         * As more 'play media' commands come in to this stream, it will add them to the queue so the user can save as a play list.
         * If it's a mounted media, like dvd, that won't happen
         */
        deque<MediaFile *>  m_dequeMediaFile;        /** The filenames we're playing */
        unsigned int		m_iDequeMediaFile_Pos;   /** The play position in the m_dequeFilename deque. */
        int					m_iPK_Playlist;          /** the ID of the playlist. nonZero if the playlist was loaded from database, zero otherwise. */
        string				m_sPlaylistName;       	 /** the name of the playlist which was loaded from the database. */

		DeviceData_Router	*m_pDeviceData_Router_Source;      /** The device which is the source of this media stream. */
		int 			 m_iPK_MediaType;        	/** The type of media in this stream. */

		// TODO: Ask why do i need to put class in front here ?
		class MediaPosition 	*m_pMediaPosition; 			/** Where we are in the media stream */

		char 		*m_pPictureData;
        size_t 		 m_iPictureSize;

        int 	m_iPK_DesignObj_Remote; 		/** What screen to use as the remote control */
        int 	m_iPK_DesignObj_RemoteOSD; 		/** What screen to use as the remote control for the on-screen orbiter */

		bool 	m_bPlaying;        				/** True if the media is now playing */

		int 	m_iPK_Users;        			/** Who started the media */
        bool 	m_bFollowMe;       				/** True if the media is supposed to follow the above user */

		bool 	m_bIsMovable; 					/** < bCanMove - This if this media stream can be moved in a move media command */
		int 	m_iOrder;  						/** This is used for the floorplans to order and color code the streams */

        enum SourceType m_eSourceType;  /** Where the media is coming from */

        string m_sMediaDescription;     /** Some text the plug-in populates to describe the media.  "The Patriot", "Beatles Anthology" are examples.
												For TV or radio this is the channel */
        string m_sSectionDescription;   /** Describe where we are in the media, such as a song title, chapter, track, etc.
                        						For TV or radio this is the name of the show */
        string m_sMediaSynopsis;    	/** A description of what's playing, such as the tv show description */
        string m_sTimecode;    			/** The handler may populate this with a timecode */


        /** @brief constructor*/
        MediaStream(class MediaHandlerInfo *pMediaHandlerInfo, DeviceData_Router *pDeviceData_Router, int PK_DesignObj_Remote, int PK_Users,enum SourceType sourceType,int iStreamID);

        /** @brief virtual destructor */
        virtual ~MediaStream();

        virtual int GetType() { return MEDIASTREAM_TYPE_GENERIC; }
        virtual void UpdateDescriptions() { m_sMediaDescription="Generic Media"; m_sSectionDescription=""; m_sMediaSynopsis=""; m_sTimecode=""; }
        int m_iStreamID_get() { return m_iStreamID; }

        void SetPicture(char *pPictureData,int iPictureSize) { delete[] m_pPictureData; m_pPictureData=pPictureData; m_iPictureSize=iPictureSize; }

		virtual bool isMovable();
		virtual void setIsMovable(bool bIsMovable = true);

        virtual string GetFilenameToPlay( string defaultFileName = "" );
        virtual void SetPlaylistPosition(int position);
        virtual void ChangePositionInPlaylist(int iHowMuch);
        virtual void DumpPlaylist();
        virtual void ClearPlaylist();
        virtual bool HaveMoreInQueue();

		/**
		 * @brief The stream will change his internal state to match the new location specificated.
		 * @return true if the specification was undestood by the MediaStream, false otherwise.
		 */
		virtual bool ProcessJumpPosition(string sJumpSpecification);
    };

    typedef map<int,MediaStream *> MapMediaStream;
}

#endif
