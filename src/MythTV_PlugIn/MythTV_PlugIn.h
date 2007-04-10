/**
 *
 * @file MythTV_PlugIn.h
 * @brief header file for the  MythTvStream, MythTV_PlugIn classes
 *
 */

//<-dceag-d-b->
#ifndef MythTV_PlugIn_h
#define MythTV_PlugIn_h

//	DCE Implemenation for #36 MythTV Plug-In

#include "Gen_Devices/MythTV_PlugInBase.h"
//<-dceag-d-e->

#include "../Media_Plugin/Media_Plugin.h"
#include "../Media_Plugin/MediaHandlerBase.h"
#include "Datagrid_Plugin/Datagrid_Plugin.h"
#include "General_Info_Plugin/General_Info_Plugin.h"

#include "MythTvMediaStream.h"

#include "AlarmManager.h"

class Database_FakeEPG;
class Row_Listing;
class EPGGrid;
class MythTvWrapper;
class MySqlHelper;

namespace DCE
{
    using namespace std;

	class MythChannel
	{
	public:
		int m_dwID,m_dwChanNum,m_dwSource;
		string m_sShortName,m_sLongName;
		char *m_pPic;
		size_t m_Pic_size;
		DataGridCell *m_pCell;  // A temporary pointer only valid while created a grid
		char m_cAddedAlready;  // Also temporary since the same channel can appear multiple times in a list.  Values are 'N', 'Y', 'F' for favorite program
		bool m_bFavorite;

		MythChannel(int dwID,int dwChanNum,int dwSource, string sShortName,string sLongName,char *pPic, size_t Pic_size)
		{
			m_dwID=dwID;
			m_dwChanNum=dwChanNum;
			m_dwSource=dwSource;
			m_sShortName=sShortName;
			m_sLongName=sLongName;
			m_pPic=pPic;
			m_Pic_size=Pic_size;
			m_pCell=NULL;
		}

		~MythChannel()
		{
			delete m_pPic;
			m_pPic=NULL;
			m_Pic_size=0;
		}
	};

	typedef list<MythChannel *> ListMythChannel;

	class ScanJob
	{
	public:
		Row_Device *m_pRow_Device_CaptureCard,*m_pRow_Device_Tuner;
		int m_iPK_Orbiter,m_iPercentCompletion;
		string m_sScanJob,m_sStatus;
		bool m_bActive;
		ScanJob(Row_Device *pRow_Device_CaptureCard,Row_Device *pRow_Device_Tuner,int iPK_Orbiter,string sScanJob)
		{
			m_pRow_Device_Tuner=pRow_Device_Tuner;
			m_pRow_Device_CaptureCard=pRow_Device_CaptureCard;
			m_iPK_Orbiter=iPK_Orbiter;
			m_sScanJob=sScanJob;
			m_bActive=false;
		}
	};

    //<-dceag-decl-b->!
    class MythTV_PlugIn : public MythTV_PlugIn_Command, public MediaHandlerBase, public AlarmEvent, public DataGridGeneratorPlugIn
    {
	//<-dceag-decl-e->
        // friend class MythTvStream;
		MySqlHelper *m_pMySqlHelper_Myth;
		map<int, int> m_mapMythInputsToDevices;
		EPGGrid *m_pEPGGrid;

		/** Private member variables */
        map<int, int> m_mapDevicesToStreams;
		map<int,list_int> m_mapDevicesToSources; // First int is a device (like a tv tuner, sat box, etc.), second is a list of video sources.  First int is '0' for all myth systems

		map< int, ScanJob * > m_mapPendingScans; // Map tuner device, capture card to string of scan script,percent complete

        MythTvWrapper *m_pMythWrapper;

		class AlarmManager *m_pAlarmManager;

		int m_dwPK_File_LastCheckedForNewRecordings;
		bool m_bBookmarksNeedRefreshing;
		bool m_bFillDbRunning,  // true if the fill process is currently active
			m_bNeedToRunFillDb; // if we get more changes, while a fill is active, we'll set this so when the fill stops we know to restart it
        // MythTvEPGWrapper *m_pAllShowsDataGrid;

        /** Private methods */
    public:
        /** Public member variables */
        int m_dwTargetDevice;
		
		// Map of PK_User to bookmarked channel,series,program, where bookmark is a comma delimited list
		// populated by RefreshBookmarks
		map<int,string> m_mapChannelBookmarks,m_mapSeriesBookmarks,m_mapProgramBookmarks;
		map<int,int> m_mapUserFavoriteChannels; // Map the user id to the number of favorite channels he has


        //<-dceag-const-b->
public:
		// Constructors/Destructor
		MythTV_PlugIn(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~MythTV_PlugIn();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

		virtual void PrepareToDelete();

		virtual void BuildAttachedInfraredTargetsMap();
    private:
        // Database_FakeEPG *m_pDatabase_FakeEPG;
        class Datagrid_Plugin *m_pDatagrid_Plugin;
		class General_Info_Plugin *m_pGeneral_Info_Plugin;
		map<int,MythChannel *> m_mapMythChannel;  // A Channel ID to channel info
		MythChannel *m_mapMythChannel_Find(int chanid) { map<int,class MythChannel *>::iterator it = m_mapMythChannel.find(chanid); return it==m_mapMythChannel.end() ? NULL : (*it).second; }
		map<int,ListMythChannel> m_map_listMythChannel; // User ID to a sorted list of that users channels with favorites first

        // finds the nearest (up to 30 minutes rounding) previous date since the currentTime
//         QDateTime findCurrentStartTime();

    public:
        /** Mandatory implementations */
        virtual class MediaStream *CreateMediaStream(class MediaHandlerInfo *pMediaHandlerInfo, int iPK_MediaProvider, vector<class EntertainArea *> &vectEntertainArea, MediaDevice *pMediaDevice, int iPK_Users, deque<MediaFile *> *dequeFilenames,int StreamID);

        virtual bool StartMedia(class MediaStream *pMediaStream,string &sError);

        virtual bool StopMedia(class MediaStream *pMediaStream);

		MythTvMediaStream* ConvertToMythMediaStream(MediaStream *pMediaStream, string callerIdMessage = "");

        /** Datagrids */
    	class DataGridTable *CurrentShows(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign, class Message *pMessage);

        /** All Shows available in the MythTv database */
    	class DataGridTable *AllShows(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, Message *pMessage);

        /** The interceptor for the MediaInfoChangedEvent from the playing device */
    	bool MediaInfoChanged( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );

		/** The interceptor to monitor progress on channel scans */
		bool ScanningProgress( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );

		/** What tv options are available in the area -- this is a temporary hack until we get a proper source for this */
		class DataGridTable *TvProviders(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage);
		class DataGridTable *FavoriteChannels( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
		class DataGridTable *FavoriteShows( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
		class DataGridTable *ThumbnailableAttributes( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );

		// Set a value in mythconverg's setting table.  If hostname=*, all known hosts will be set.  If it's empty, hostname will be NULL
		void UpdateMythSetting(string value,string data,string hostname,bool bOnlyIfNotExisting=false);

		void AlarmCallback(int id, void* param);
		void CheckForNewRecordings();

		bool NewBookmarks( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );
		void RefreshBookmarks();
		void BuildChannelList(); // Build a list of all channels with their icons and store as MythChannel objects in m_mapMythChannel, and store sorted versions in m_map_listMythChannel
		void PurgeChannelList(); // Purge the list of channels, freeing all memory used

		void StartScanningScript(Row_Device *pRow_Device_Tuner,Row_Device *pRow_Device_CaptureCard,int iPK_Orbiter,string sScanningScript);
		void StartScanJob(ScanJob *pScanJob);
		void CheckForTvFormatAndProvider( int iPK_Device );

		virtual bool ReportPendingTasks(PendingTaskList *pPendingTaskList=NULL);

		void SetPaths();
		void RunBackendStarter();
		void StartFillDatabase();

        //<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_PK_Device();
	int DATA_Get_Priority();
	bool DATA_Get_Dont_Auto_Configure();

			*****EVENT***** accessors inherited from base class
	void EVENT_Error_Occured(string sError_Message);
	void EVENT_PVR_Rec_Sched_Conflict();

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
		/** @param #198 PK_Orbiter */
			/** If specified, this is the orbiter to notify of the progress if this results in scanning for channels */

	virtual void CMD_Sync_Providers_and_Cards(int iPK_Orbiter) { string sCMD_Result; CMD_Sync_Providers_and_Cards(iPK_Orbiter,sCMD_Result,NULL);};
	virtual void CMD_Sync_Providers_and_Cards(int iPK_Orbiter,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #846 - Make Thumbnail */
	/** Thumbnail the current frame */
		/** @param #13 Filename */
			/** Can be a fully qualified filename, or a !F+number, or !A+number for an attribute */
		/** @param #19 Data */
			/** The picture */

	virtual void CMD_Make_Thumbnail(string sFilename,char *pData,int iData_Size) { string sCMD_Result; CMD_Make_Thumbnail(sFilename.c_str(),pData,iData_Size,sCMD_Result,NULL);};
	virtual void CMD_Make_Thumbnail(string sFilename,char *pData,int iData_Size,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #882 - Abort Task */
	/** Abort a pending task */
		/** @param #248 Parameter ID */
			/** The ID of the task to abort */

	virtual void CMD_Abort_Task(int iParameter_ID) { string sCMD_Result; CMD_Abort_Task(iParameter_ID,sCMD_Result,NULL);};
	virtual void CMD_Abort_Task(int iParameter_ID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #910 - Reporting EPG Status */
	/** Reporting the status of an EPG update */
		/** @param #9 Text */
			/** Any messages about this */
		/** @param #40 IsSuccessful */
			/** true if the process succeeded */
		/** @param #257 Task */
			/** The type of EPG task: channel (retrieving channels), guide (retrieving guide) */

	virtual void CMD_Reporting_EPG_Status(string sText,bool bIsSuccessful,string sTask) { string sCMD_Result; CMD_Reporting_EPG_Status(sText.c_str(),bIsSuccessful,sTask.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Reporting_EPG_Status(string sText,bool bIsSuccessful,string sTask,string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->
    };
//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
//<-dceag-const2-b->!
