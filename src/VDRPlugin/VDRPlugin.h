/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
//<-dceag-d-b->
#ifndef VDRPlugin_h
#define VDRPlugin_h

//	DCE Implemenation for #1704 VDR-Plugin

#include "Gen_Devices/VDRPluginBase.h"
//<-dceag-d-e->

#include "DCE/PlainClientSocket.h"

#include "../Media_Plugin/Media_Plugin.h"
#include "../Media_Plugin/MediaStream.h"
#include "../Media_Plugin/MediaHandlerBase.h"
#include "VDRMediaStream.h"
class Row_Bookmark;

class VDRSource
{
public:
	VDRSource(int dwID,string sDescription) : m_dwID(dwID), m_sDescription(sDescription) {}

	int m_dwID;
	string m_sDescription;
};

typedef map<int,int> MapBookmark;  // Map of PK_Users to PK_Bookmarks 
class VDRChannel
{
public:
	int m_dwID,m_dwChanNum;
	VDRSource *m_pVDRSource;
	string m_sShortName,m_sLongName;
	char *m_pPic;
	size_t m_Pic_size;
	MapBookmark m_mapBookmark;  // Map of PK_Users to PK_Bookmarks who have bookmarked this
	DataGridCell *m_pCell;  // A temporary pointer only valid while created a grid

	VDRChannel(int dwID,int dwChanNum,VDRSource *pVDRSource, string sShortName,string sLongName,char *pPic, size_t Pic_size)
	{
		m_pVDRSource=pVDRSource;
		m_dwID=dwID;
		m_dwChanNum=dwChanNum;
		m_sShortName=sShortName;
		m_sLongName=sLongName;
		m_pPic=pPic;
		m_Pic_size=Pic_size;
		m_pCell=NULL;
	}

	~VDRChannel()
	{
		delete m_pPic;
		m_pPic=NULL;
		m_Pic_size=0;
	}
};

// A particular airing of a show (ie a channel & start time)
class VDRProgramInstance
{
public:
	time_t m_tStartTime,m_tStopTime;
	VDREpisode *m_pVDREpisode;
};

// A program is a unique episode of a series, or a unique showing if there's not a series
class VDREpisode
{
public:
	VDRSeries *m_pVDRSeries;  // The series, like the Simpsons, or if this is a one-time show, it points to the description like "Batman 2"
	string m_sSubTitle; // Bart learns to drive
	list<VDRPerformer *> m_listVDRPerformer;
};

// The series info i.e. "The Simpsons", "Batman 2"
class VDRSeries
{
public:
	string m_sDescription; // i.e. "The Simpsons", "Batman 2"
};

// A performer
class VDRPerformer
{
public:
	string m_sName;
	list<VDREpisode *> m_listVDREpisode;
};

typedef list<VDRChannel *> ListVDRChannel;



//<-dceag-decl-b->!
namespace DCE
{
	class VDRPlugin : public VDRPlugin_Command, public MediaHandlerBase, public DataGridGeneratorPlugIn
	{
//<-dceag-decl-e->
		// Private member variables
		ListVDRChannel m_ListVDRChannel; // The channels in the right order for display
		map<string,VDRSource *> m_mapVDRSource;
		VDRSource *m_mapVDRSource_Find(string sSourceID) { map<string,VDRSource *>::iterator it = m_mapVDRSource.find(sSourceID); return it==m_mapVDRSource.end() ? NULL : (*it).second; }
		string m_sVDRIp;

		class Orbiter_Plugin *m_pOrbiter_Plugin;
		class Datagrid_Plugin *m_pDatagrid_Plugin;
	    pluto_pthread_mutex_t m_VDRMutex; // Protect the maps
		pthread_cond_t m_VDRCond; /** < condition for the messages in the queue */

		// Private methods
public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		VDRPlugin(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~VDRPlugin();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

		void PurgeChannelList();
		void BuildChannelList();
		VDRSource *GetNewSource(string sSource);

		virtual void PrepareToDelete();
		// Datagrids
		class DataGridTable *CurrentShows(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, Message *pMessage);
		class DataGridTable *AllShows(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, Message *pMessage);
		class DataGridTable *FavoriteChannels(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, Message *pMessage);
		class DataGridTable *FavoriteShows(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, Message *pMessage);
		class DataGridTable *OtherShowtimes(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, Message *pMessage);

		// Utilities
		/*
		bool GetVdrAndEpgFromOrbiter(int PK_Device,class MediaDevice *&pMediaDevice_VDR,VDREPG::EPG *&pEPG,VDRMediaStream *&pVDRMediaStream);
		VDREPG::Event *GetStartingEvent(VDREPG::EPG *pEPG,int PK_Users);
		VDREPG::Event *GetEventForBookmark(VDREPG::EPG *pEPG,Row_Bookmark *pRow_Bookmark,int &iPriority_Bookmark);
		*/

//<-dceag-const2-b->!
		/**
		* @brief
		*/
		virtual class MediaStream *CreateMediaStream( class MediaHandlerInfo *pMediaHandlerInfo, int iPK_MediaProvider, vector<class EntertainArea *> &vectEntertainArea, MediaDevice *pMediaDevice, int iPK_Users, deque<MediaFile *> *dequeFilenames, int StreamID );

		/**
		* @brief Start media playback
		*/
		virtual bool StartMedia( class MediaStream *pMediaStream,string &sError );

		/**
		* @brief Stop media playback
		*/
		virtual bool StopMedia( class MediaStream *pMediaStream );

		virtual MediaDevice *FindMediaDeviceForEntertainArea(EntertainArea *pEntertainArea);


//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	int DATA_Get_Priority();

			*****EVENT***** accessors inherited from base class

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #65 - Jump Position In Playlist */
	/** Change channels.  +1 and -1 mean up and down 1 channel. */
		/** @param #5 Value To Assign */
			/** The track to go to.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1. */

	virtual void CMD_Jump_Position_In_Playlist(string sValue_To_Assign) { string sCMD_Result; CMD_Jump_Position_In_Playlist(sValue_To_Assign.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Jump_Position_In_Playlist(string sValue_To_Assign,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #185 - Schedule Recording */
	/** This will schedule a recording. */
		/** @param #68 ProgramID */
			/** The program which will need to be recorded. (The format is defined by the device which created the original datagrid) */

	virtual void CMD_Schedule_Recording(string sProgramID) { string sCMD_Result; CMD_Schedule_Recording(sProgramID.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Schedule_Recording(string sProgramID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #409 - Save Bookmark */
	/** Save the current channel or program as a bookmark.  Text should have CHAN: or PROG: in there */
		/** @param #39 Options */
			/** For TV, CHAN: or PROG: indicating if it's the channel or program to bookmark */
		/** @param #45 PK_EntertainArea */
			/** The entertainment area with the media */

	virtual void CMD_Save_Bookmark(string sOptions,string sPK_EntertainArea) { string sCMD_Result; CMD_Save_Bookmark(sOptions.c_str(),sPK_EntertainArea.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Save_Bookmark(string sOptions,string sPK_EntertainArea,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #698 - Get Extended Media Data */
	/** Returns extra data about the given media, such as the title, airtime, whether it's currently scheduled to record, etc. */
		/** @param #3 PK_DesignObj */
			/** If specified the sender will be sent a goto-screen with this screen.  If not the sender will be sent a refresh */
		/** @param #68 ProgramID */
			/** If specified, the program to retrive info on.  If not specified, assumed to be the currently playing media */

	virtual void CMD_Get_Extended_Media_Data(string sPK_DesignObj,string sProgramID) { string sCMD_Result; CMD_Get_Extended_Media_Data(sPK_DesignObj.c_str(),sProgramID.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Get_Extended_Media_Data(string sPK_DesignObj,string sProgramID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #764 - Set Active Menu */
	/** Indicate which menu is active, options are:
live, nonlive, osd */
		/** @param #9 Text */
			/** The menu currently active */

	virtual void CMD_Set_Active_Menu(string sText) { string sCMD_Result; CMD_Set_Active_Menu(sText.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Set_Active_Menu(string sText,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #824 - Sync Providers and Cards */
	/** Synchronize settings for pvr cards and provders */

	virtual void CMD_Sync_Providers_and_Cards() { string sCMD_Result; CMD_Sync_Providers_and_Cards(sCMD_Result,NULL);};
	virtual void CMD_Sync_Providers_and_Cards(string &sCMD_Result,Message *pMessage);


//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
