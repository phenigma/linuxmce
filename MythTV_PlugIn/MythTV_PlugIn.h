//<-dceag-d-b->
#ifndef MythTV_PlugIn_h
#define MythTV_PlugIn_h

//	DCE Implemenation for #36 MythTV Plug-In

#include "Gen_Devices/MythTV_PlugInBase.h"
//<-dceag-d-e->

#include "../Media_Plugin/Media_Plugin.h"
#include "Datagrid_Plugin/Datagrid_Plugin.h"

class Database_FakeEPG;
class Row_Listing;

class MythTvWrapper;

namespace DCE
{

    class MythTvStream : public MediaStream
    {
    public:
        class MythTV_PlugIn *m_pMythTV_PlugIn;

        MythTvStream(class MythTV_PlugIn *pMythTV_PlugIn,class MediaPluginInfo *pMediaPluginInfo, int PK_DesignObj_Remote, int PK_Users,enum SourceType sourceType,int StreamID)
            : MediaStream(pMediaPluginInfo,PK_DesignObj_Remote,PK_Users,sourceType,StreamID) { m_iCurrentShow=-1; m_pMythTV_PlugIn=pMythTV_PlugIn; }

            // Temporary hacks, just populate m_vectRow_Listing with all the shows, and make m_iCurrentShow point to the current one
            vector<Row_Listing *> m_vectRow_Listing;
            int m_iCurrentShow;
            virtual void UpdateDescriptions();

            virtual int GetType() { return MEDIASTREAM_TYPE_MYTHTV; }
    };

    //<-dceag-decl-b->!
    class MythTV_PlugIn : public MythTV_PlugIn_Command, public MediaPluginBase, public DataGridGeneratorPlugIn
    {
        //<-dceag-decl-e->
        friend class MythTvStream;
        // Private member variables

        MythTvWrapper *m_pMythWrapper;
//         MythTvEPGWrapper *m_pAllShowsDataGrid;

        // Private methods
    public:
        // Public member variables

        //<-dceag-const-b->
public:
		// Constructors/Destructor
		MythTV_PlugIn(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~MythTV_PlugIn();
		virtual bool Register();
//<-dceag-const-e->

    private:
//         Database_FakeEPG *m_pDatabase_FakeEPG;
        class Datagrid_Plugin *m_pDatagrid_Plugin;

    public:
        // Mandatory implementations
        virtual class MediaStream *CreateMediaStream(class MediaPluginInfo *pMediaPluginInfo,int PK_Device_Source,string Filename,int StreamID);
        virtual bool StartMedia(class MediaStream *pMediaStream);
        virtual bool StopMedia(class MediaStream *pMediaStream);
        virtual bool BroadcastMedia(class MediaStream *pMediaStream);

        // Datagrids
        class DataGridTable *CurrentShows(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage);
        class DataGridTable *AllShows(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, Message *pMessage);

        // custom helper methods
//         void ProcessWatchTvRequest(int channelId, QDateTime showStartTime);
//         ProgramInfo *getProgramInfo(int channelId, QDateTime insideTime);

        //<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class

			*****EVENT***** accessors inherited from base class

			*****COMMANDS***** we need to implement
	*/

/* 
	COMMAND: #65 - Jump Position In Playlist
	COMMENTS: Change channels.  +1 and -1 mean up and down 1 channel.
	PARAMETERS:
		#5 Value To Assign
			The track to go to.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1.
*/
	virtual void CMD_Jump_Position_In_Playlist(string sValue_To_Assign) { string sCMD_Result; CMD_Jump_Position_In_Playlist(sValue_To_Assign.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Jump_Position_In_Playlist(string sValue_To_Assign,string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #185 - Schedule Recording
	COMMENTS: This will schedule a recording.
	PARAMETERS:
		#68 ProgramID
			The program which will need to be recorded. (The format is defined by the device which created the original datagrid)
*/
	virtual void CMD_Schedule_Recording(string sProgramID) { string sCMD_Result; CMD_Schedule_Recording(sProgramID.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Schedule_Recording(string sProgramID,string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->
    };
//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
