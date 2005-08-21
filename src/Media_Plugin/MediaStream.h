#ifndef MediaStream_h
#define MediaStream_h

#include <map>
#include <deque>

#include "MediaAttributes.h"
#include "MediaHandlerInfo.h"
#include "DeviceData_Router.h"

#include "MediaFile.h"
#include "MediaTitle.h"
#include "MediaSection.h"

using namespace std;

class Row_MediaType_DesignObj;
class Row_DeviceTemplate_MediaType_DesignObj;

namespace DCE
{

	class RemoteControlSet 
	{
	public:
		int 	m_iPK_DesignObj_Remote; 		/** What screen to use as the remote control */
        int 	m_iPK_DesignObj_Remote_Popup; 		/** What screen to use as the popup remote control */
        int 	m_iPK_DesignObj_FileList; 			/** What screen to use as the file list */
        int 	m_iPK_DesignObj_FileList_Popup; 	/** What screen to use as the file list popup */
		RemoteControlSet(Row_MediaType_DesignObj *);
		RemoteControlSet(Row_DeviceTemplate_MediaType_DesignObj *);
	};


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

        map<int, class EntertainArea *> m_mapEntertainArea; /** The entertainment areas where this stream is playing */
		map<int,int> m_mapPK_Attribute;  /** An external media identification script may set attributes here, PK_AttributeType,Section=PK_Attribute */
        int m_mapPK_Attribute_Find(int PK_AttributeType) { map<int,int>::iterator it = m_mapPK_Attribute.find(PK_AttributeType); return it==m_mapPK_Attribute.end() ? 0 : (*it).second; }

        /**
         * As more 'play media' commands come in to this stream, it will add them to the queue so the user can save as a play list.
         * If it's a mounted media, like dvd, that won't happen
         */
        deque<MediaFile *>  m_dequeMediaFile;        /** The filenames we're playing */
        deque<MediaTitle *>  m_dequeMediaTitle;      /** The titles, if this is something like a DVD */
        deque<MediaSection *>  m_dequeMediaSection;      /** The titles, if this is something like a DVD */
        unsigned int		m_iDequeMediaFile_Pos;   /** The play position in the m_dequeFilename deque. */
        int					m_iPK_Playlist;          /** the ID of the playlist. nonZero if the playlist was loaded from database, zero otherwise. */
        string				m_sPlaylistName;       	 /** the name of the playlist which was loaded from the database. */
		int m_discid;  /** A unique number to identify the disc inserted, if this is from a removable disc (CD/DVD) */
		
		/** When this media is identified, the priority of the identifying module is stored here so if another
		module also identifies it, we can take the one with the highest priority */
		int m_IdentifiedPriority; 
		int m_dwPK_Disc;  /** 0 if this isn't a removable disc media */

		MediaDevice		*m_pMediaDevice_Source;      /** The device which is the source of this media stream. */
		int 			 m_iPK_MediaType;        	/** The type of media in this stream. */
		int				 m_iPK_MediaProvider;		/** The media provider in this stream. */

		char 		*m_pPictureData;
        size_t 		 m_iPictureSize;

        int 	m_iPK_DesignObj_RemoteOSD; 		/** What screen to use as the remote control for the on-screen orbiter */

		RemoteControlSet *m_pRemoteControlSet;  /** The remote controls to use for this stream */

		bool 	m_bPlaying;        				/** True if the media is now playing */

		int 	m_iPK_Users;        			/** Who started the media */
        bool 	m_bFollowMe;       				/** True if the media is supposed to follow the above user */

		bool 	m_bIsMovable; 					/** < bCanMove - This if this media stream can be moved in a move media command */
		int 	m_iOrder;  						/** This is used for the floorplans to order and color code the streams */
		int		m_iRepeat;						/** Indicates if this media should repeat */
		bool 	m_bResume; 						/** < True if this media was started with the 'resume' flag, meaning when it ends, resume what was playing previously */

		bool	m_bStopped;						/** Set when we stop the stream, so that if we later get a playback completed we know we already stopped it and don't need to do anything more */

		time_t	m_tTime;						/** When the user started this media */
		time_t	m_tTime_Parked;					/** When we parked this media -- ie the user left an area with follow me on, so we stop the media, and leave it 'pending' for a while so he can enter another area */

        enum SourceType m_eSourceType;  /** Where the media is coming from */

        string m_sMediaDescription;     /** Some text the plug-in populates to describe the media.  "The Patriot", "Beatles Anthology" are examples.
												For TV or radio this is the channel */
        string m_sSectionDescription;   /** Describe where we are in the media, such as a song title, chapter, track, etc.
                        						For TV or radio this is the name of the show */
        string m_sMediaSynopsis;    	/** A description of what's playing, such as the tv show description */
        string m_sTimecode;    			/** The handler may populate this with a timecode */
        string m_sTotalTime;    		/** The handler may populate this with the total time */
		string m_sLastPosition;			/** The handler may populate this with a seekable position showing the current location.  I should do this in StopMedia if possible so the media can be resumed */
		string m_sStartPosition;		/** The position to start the stream at. */


        /** @brief constructor*/
        MediaStream(class MediaHandlerInfo *pMediaHandlerInfo, int iPK_MediaProvider, MediaDevice *pMediaDevice, int PK_Users,enum SourceType sourceType,int iStreamID);

        /** @brief virtual destructor */
        virtual ~MediaStream();

        virtual int GetType() { return MEDIASTREAM_TYPE_GENERIC; }
        virtual void UpdateDescriptions() { m_sMediaDescription="Generic Media"; m_sSectionDescription=""; m_sMediaSynopsis=""; m_sTimecode=""; }
        int m_iStreamID_get() { return m_iStreamID; }

        void SetPicture(char *pPictureData,int iPictureSize) { delete[] m_pPictureData; m_pPictureData=pPictureData; m_iPictureSize=iPictureSize; }
		string GetAllOSD();

		virtual bool isMovable();
		virtual void setIsMovable(bool bIsMovable = true);

        virtual string GetFilenameToPlay( string defaultFileName = "" );
        virtual void SetPlaylistPosition(int position);
        virtual void ChangePositionInPlaylist(int iHowMuch);
        virtual void DumpPlaylist();
        virtual void ClearPlaylist();

		virtual bool CanPlayMore();
		virtual bool ContainsVideo();

		// True if the orbiter is an on screen display for this stream.  If ppEntertainArea is not NULL
		// it will be set to the entertainment area for the OSD
		virtual bool OrbiterIsOSD(int PK_Orbiter,EntertainArea **ppEntertainArea=NULL);

		virtual void GetRenderDevices(map<int, MediaDevice *> *pmapMediaDevices);

		virtual int SpecialOsdScreen(); // Returns 0 if this stream has a special object for OSD

		virtual void DeleteEntryFromPlaylist(int position);
		virtual void MoveEntryInPlaylist(int position, int displacement);
		/**
		 * @brief The stream will change his internal state to match the new location specificated.
		 * @return true if the specification was undestood by the MediaStream, false otherwise.
		 */
		virtual bool ProcessJumpPosition(string sJumpSpecification);
	
		/**
		 * @brief Goes through the attributes (song title, group, etc.) and determines the
		 * appropriate description for the stream and each file in it
		 */
		virtual void UpdateDescriptionsFromAttributes();

		/**
		* return a comma delimited list of all active destination ent areas
		* except the one passed in, or all areas if the parameter is null
		*/
		string GetEntAreasWithout(EntertainArea *pEntertainArea);
		string GetEntAreasWithout(map<int,class EntertainArea *> *p_mapEntertainArea);
	};

    typedef map<int,MediaStream *> MapMediaStream;
}

#endif
