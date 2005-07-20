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

class MythTvWrapper;

namespace DCE
{
    using namespace std;

    //<-dceag-decl-b->!
    class MythTV_PlugIn : public MythTV_PlugIn_Command, public MediaHandlerBase // , public MediaHandlerBase
    {
	//<-dceag-decl-e->
        // friend class MythTvStream;

		map<int, int> m_mapMythInputsToDevices;

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
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage);
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

        virtual bool StartMedia(class MediaStream *pMediaStream);

        virtual bool StopMedia(class MediaStream *pMediaStream);

		MythTvMediaStream* ConvertToMythMediaStream(MediaStream *pMediaStream, string callerIdMessage = "");

        /** Datagrids */
    	class DataGridTable *CurrentShows(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign, class Message *pMessage);

        /** All Shows available in the MythTv database */
    	class DataGridTable *AllShows(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, Message *pMessage);

        /** All current shows on all channels (This grid is suitable for mobile phones.) */
    	class DataGridTable *AllShowsForMobiles(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, Message *pMessage);

        /** The interceptor for the MediaInfoChangedEvent from the playing device */
    	bool MediaInfoChanged( class Socket *pSocket, class Message *pMessage, class DeviceData_Router *pDeviceFrom, class DeviceData_Router *pDeviceTo );

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


//<-dceag-h-e->
    };
//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
//<-dceag-const2-b->!
