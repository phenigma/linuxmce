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

#include "MythTvMediaStream.h"

class Database_FakeEPG;
class Row_Listing;
class EPGGrid;
class MythTvWrapper;
class MySqlHelper;

namespace DCE
{
    using namespace std;

    //<-dceag-decl-b->!
    class MythTV_PlugIn : public MythTV_PlugIn_Command, public MediaHandlerBase, public DataGridGeneratorPlugIn
    {
	//<-dceag-decl-e->
        // friend class MythTvStream;
		MySqlHelper *m_pMySqlHelper_Myth;
		map<int, int> m_mapMythInputsToDevices;
		EPGGrid *m_pEPGGrid;
		DeviceData_Router *m_pMythBackend_ProxyDevice;

		/** Private member variables */
        map<int, int> m_mapDevicesToStreams;

        MythTvWrapper *m_pMythWrapper;
        // MythTvEPGWrapper *m_pAllShowsDataGrid;

        /** Private methods */
    public:
        /** Public member variables */
        int m_dwTargetDevice;

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

		virtual void BuildAttachedInfraredTargetsMap();
    private:
        // Database_FakeEPG *m_pDatabase_FakeEPG;
        class Datagrid_Plugin *m_pDatagrid_Plugin;
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

        /** All current shows on all channels (This grid is suitable for mobile phones.) */
    	class DataGridTable *AllShowsForMobiles(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, Message *pMessage);

        /** The interceptor for the MediaInfoChangedEvent from the playing device */
    	bool MediaInfoChanged( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );

		/** What tv options are available in the area -- this is a temporary hack until we get a proper source for this */
		class DataGridTable *TvProviders(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage);

        //<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	int DATA_Get_Priority();

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


//<-dceag-h-e->
    };
//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
//<-dceag-const2-b->!
