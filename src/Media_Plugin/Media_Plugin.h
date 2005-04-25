
/**
 *
 * @file Media_Plugin.h
 * @brief header file for the  Media_Plugin, MediaPosition, MediaDevice, MediaHandlerBase classes
 */

//<-dceag-d-b->
#ifndef Media_Plugin_h
#define Media_Plugin_h

//	DCE Implemenation for #2 Media Plug-in

#include "Gen_Devices/Media_PluginBase.h"
//<-dceag-d-e->

#include <deque>

#include "DeviceData_Router.h"
#include "DCE/Logger.h"
#include "DCERouter.h"
#include "MediaHandlerInfo.h"
#include "pluto_main/Table_MediaType.h"
#include "pluto_main/Table_DeviceTemplate.h"
#include "pluto_main/Table_DeviceTemplate_MediaType.h"
#include "pluto_main/Table_DeviceTemplate_MediaType_DesignObj.h"
#include "pluto_main/Table_MediaType_DesignObj.h"
#include "Datagrid_Plugin/Datagrid_Plugin.h"
#include "MediaAttributes.h"

#include "../Orbiter/Floorplan.h"
#include "Orbiter_Plugin/FollowMe_Device.h"
#include "MediaStream.h"
#include "EntertainArea.h"

class Database_pluto_main;
class Database_pluto_media;
class Row_EntertainArea;
class MediaFile;

/**
 * A Media Handler is derived from the Media Handler abstract class.  When it registers, it passes in a MediaHandlerInfo pointer indicating
 * what type of media it can play.  It may register several times with different types of media and different capabilities.
 */

// WARN: MediaHandlerBase was moved below because it needed the definition of the EntertainArea object
namespace DCE
{


//<-dceag-decl-b->!
class Media_Plugin : public Media_Plugin_Command, public DataGridGeneratorPlugIn, public FloorplanInfoProvider, public FollowMe_Device
{
//<-dceag-decl-e->
    friend class MediaStream;
    friend class MediaAttributes;
//<-dceag-const-b->!
public:
        // Constructors/Destructor
        Media_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
        virtual ~Media_Plugin();
        virtual bool Register();
        virtual void ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage);
        virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->
    class MediaAttributes *m_pMediaAttributes;
	class Generic_NonPluto_Media *m_pGeneric_NonPluto_Media;
    class MediaHandlerInfo *m_pGenericMediaHandlerInfo;

    friend class MediaHandlerInfo;
    pluto_pthread_mutex_t m_MediaMutex; // Other classes may need this

private:
    // Private member variables
    class Orbiter_Plugin *m_pOrbiter_Plugin;
    int m_iStreamID;
    pthread_mutexattr_t m_MutexAttr;
    MapMediaStream m_mapMediaStream; // All active streams

	// mapping from job names to a pair or caller devices and the job running devices.
	map<string, pair<int, int> > m_mapRippingJobsToRippingDevices;

	Database_pluto_main *m_pDatabase_pluto_main;
    Database_pluto_media *m_pDatabase_pluto_media;
    class Datagrid_Plugin *m_pDatagrid_Plugin;

    class DataGridTable *CurrentMedia( string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage );
    class DataGridTable *CurrentMediaSections( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
    class DataGridTable *MediaSections( string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage );
    class DataGridTable *MediaAttrXref( string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage );
    class DataGridTable *MediaAttrCollections( string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage );
    class DataGridTable *MediaAttrFiles( string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage );
    class DataGridTable *MediaSearchAutoCompl( string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage );
    class DataGridTable *MediaItemAttr( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );

    class DataGridTable *AvailablePlaylists( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
    class DataGridTable *FloorplanMediaChoices( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
    class DataGridTable *ActiveMediaStreams( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );

protected:
    void PlayMediaByDeviceTemplate(int iPK_DeviceTemplate, int iPK_Device, int iPK_Device_Orbiter, EntertainArea *pEntertainArea, string &sCMD_Result);
    void PlayMediaByFileName(string sFilename, int iPK_Device, int iPK_Device_Orbiter, EntertainArea *pEntertainArea, string &sCMD_Result);
    bool EnsureCorrectMediaStreamForDevice(MediaHandlerInfo *pMediaHandlerInfo, EntertainArea *pEntertainArea, int iPK_Device);

	bool DiskDriveIsRipping(int iPK_Device);

    /**
     * Find a media plugin info object that will play the specified file.
     */
    MediaHandlerInfo *FindMediaHandlerInfoForFileName(EntertainArea *pEntertainArea, string sFileToPlay);
    /**
     * Find a media plugin info object that will play the specified media type.
     */
    MediaHandlerInfo *FindMediaHandlerInfoForMediaType(EntertainArea *pEntertainArea, int iPK_MediaType);

    /**
     * Given that media was playing on the Prior list of devices, and now is on the Current list of devices, send the appropriate on/off's
     */
	void HandleOnOffs(int PK_MediaType_Prior,int PK_MediaType_Current, map<int,MediaDevice *> *pmapMediaDevice_Prior,map<int,MediaDevice *> *pmapMediaDevice_Current);

    /**
     * Turn off the device and other devices in the pipe, but without turning off devices we are currently using
     */
	void TurnDeviceOff(int PK_Pipe,DeviceData_Router *pDeviceData_Router,map<int,MediaDevice *> *pmapMediaDevice_Current);

    /**
     * Find a media type specific for a file name
     */
    //     int FindMediaTypeForFile(string strFileName);
    // Private methods
	vector<MediaHandlerInfo *> m_vectMediaHandlerInfo;

public:
    // Public member variables
    map<int,EntertainArea *> m_mapEntertainAreas;  // Plug-ins may want this
    map<int,MediaDevice *> m_mapMediaDevice;

    EntertainArea *m_mapEntertainAreas_Find(int iPK_EntertainArea) { map<int,class EntertainArea *>::iterator it = m_mapEntertainAreas.find(iPK_EntertainArea); return it==m_mapEntertainAreas.end() ? NULL : (*it).second; }
    MediaDevice *m_mapMediaDevice_Find(int iPK_Device) { map<int,class MediaDevice *>::iterator it = m_mapMediaDevice.find(iPK_Device); return it==m_mapMediaDevice.end() ? NULL : (*it).second; }
    MediaStream *m_mapMediaStream_Find(int StreamID)  { MapMediaStream::iterator it = m_mapMediaStream.find(StreamID);  return it==m_mapMediaStream.end() ? NULL : (*it).second; }
    void m_mapMediaStream_Remove(int StreamID) { MapMediaStream::iterator it = m_mapMediaStream.find(StreamID); if( it!=m_mapMediaStream.end() ) m_mapMediaStream.erase(it); }

	Database_pluto_main *GetMainDatabaseConnection()
	{
		return m_pDatabase_pluto_main;
	}

    // A MediaPlugIn call call this function, passing in the type of device that it will handle.  This will automatically build MediaPlugInInfo's for all the
    // types that the device supports, using all the default values from the database.  A plug-in could also call the MediaHandlerInfo constructor that will
    // create and register a specific type of media.  This means it is not necessary to call RegisterMediaPlugin.  This allows the plug-in to change
    // some of the defaults.  The Plugin could also create a MediaHandlerInfo type manually for complete control over the parameters, and then call
    // RegisterMediaPlugin
    void RegisterMediaPlugin(class Command_Impl *pCommand_Impl,class MediaHandlerBase *pMediaHandlerBase,int iPK_MasterDeviceList,bool bUsesDCE)
    {
        Row_DeviceTemplate *pRow_DeviceTemplate = m_pDatabase_pluto_main->DeviceTemplate_get()->GetRow(iPK_MasterDeviceList);
        if( !pRow_DeviceTemplate )
        {
            g_pPlutoLogger->Write(LV_CRITICAL,"Invalid device template %d as plugin",iPK_MasterDeviceList);
            return;  // Nothing we can do
        }

        int iPKDevice = pCommand_Impl->m_dwPK_Device;
        int iPKDeviceTemplate = pRow_DeviceTemplate->PK_DeviceTemplate_get();
        string strDescription = pRow_DeviceTemplate->Description_get();

        g_pPlutoLogger->Write(LV_STATUS,"Registered media plug in #%d (Template %d) %s (adress %p, plugin base address %p)",iPKDevice,iPKDeviceTemplate,strDescription.c_str(), pCommand_Impl, pMediaHandlerBase);
        vector<Row_DeviceTemplate_MediaType *> vectRow_DeviceTemplate_MediaType;
        pRow_DeviceTemplate->DeviceTemplate_MediaType_FK_DeviceTemplate_getrows(&vectRow_DeviceTemplate_MediaType);
        for(size_t mt=0;mt<vectRow_DeviceTemplate_MediaType.size();++mt)
        {
            Row_DeviceTemplate_MediaType *pRow_DeviceTemplate_MediaType = vectRow_DeviceTemplate_MediaType[mt];
            MediaHandlerInfo *pMediaHandlerInfo =
                new MediaHandlerInfo(pMediaHandlerBase,pCommand_Impl,pRow_DeviceTemplate_MediaType->FK_MediaType_get(),
                    iPK_MasterDeviceList,pRow_DeviceTemplate_MediaType->CanSetPosition_get()==1,bUsesDCE);

			m_vectMediaHandlerInfo.push_back(pMediaHandlerInfo);

			// Find a default remote control for this.  If one is specified by the DeviceTemplate, use that, and then revert to one that matches the media type
            vector<Row_DeviceTemplate_MediaType_DesignObj *> vectRow_DeviceTemplate_MediaType_DesignObj;
            pRow_DeviceTemplate_MediaType->DeviceTemplate_MediaType_DesignObj_FK_DeviceTemplate_MediaType_getrows(&vectRow_DeviceTemplate_MediaType_DesignObj);
            if( vectRow_DeviceTemplate_MediaType_DesignObj.size() )
                pMediaHandlerInfo->m_iPK_DesignObj = vectRow_DeviceTemplate_MediaType_DesignObj[0]->FK_DesignObj_get();
            else
			{
				vector<Row_MediaType_DesignObj *> vectRow_MediaType_DesignObj;
				Row_MediaType *pRow_MediaType = pRow_DeviceTemplate_MediaType->FK_MediaType_getrow();
				if( !pRow_MediaType )
				{
					g_pPlutoLogger->Write(LV_CRITICAL,"db problem with media type: %d",pRow_DeviceTemplate_MediaType->FK_MediaType_get());
					continue;
				}
				pRow_MediaType->MediaType_DesignObj_FK_MediaType_getrows(&vectRow_MediaType_DesignObj);
	            if( vectRow_MediaType_DesignObj.size() )
				{
					Row_MediaType_DesignObj *pRow_MediaType_DesignObj = vectRow_MediaType_DesignObj[0];

g_pPlutoLogger->Write(LV_STATUS,"FOUND %d records for media type %d %p",(int) vectRow_MediaType_DesignObj.size(),pRow_DeviceTemplate_MediaType->FK_MediaType_get(),pRow_MediaType_DesignObj);
		            pMediaHandlerInfo->m_iPK_DesignObj = pRow_MediaType_DesignObj->FK_DesignObj_get();
				}
				else
	                pMediaHandlerInfo->m_iPK_DesignObj = 0;
			}
        }
    }

    void AddDeviceToEntertainArea(EntertainArea *pEntertainArea,Row_Device *pRow_Device);
    class EntertainArea *DetermineEntArea(int iPK_Device_Orbiter,int iPK_Device,int iPK_EntertainArea);
    class MediaStream *DetermineStreamOnOrbiter(int iPK_Device_Orbiter,bool bErrorIfNotFound=true)
    {
        class EntertainArea *pEntertainArea = DetermineEntArea(iPK_Device_Orbiter,0,0);
        if( (!pEntertainArea || !pEntertainArea->m_pMediaStream) && bErrorIfNotFound ) g_pPlutoLogger->Write(LV_CRITICAL,"No stream on orbiter: %d",iPK_Device_Orbiter);
        return pEntertainArea ? pEntertainArea->m_pMediaStream : NULL;
    }

    int DetermineUserOnOrbiter(int iPK_Device_Orbiter);

    /**
     * @brief Helper functions for the process of auto-creating entertainment areas
     */
	void DeleteEntertainArea(Row_EntertainArea *pRow_EntertainArea);
	void AddDefaultCommandsToEntArea(Row_EntertainArea *pRow_EntertainArea);
	void AddDevicesToEntArea(Row_EntertainArea *pRow_EntertainArea);
	// Returns 0 if a command group exists for this entarea/template.  Otherwise creates one and returns the id
		int FindCommandGroupByTemplate(Row_EntertainArea *pRow_EntertainArea,int PK_Template,string sDescription);
	void AddCommand(int PK_CommandGroup,int PK_Device,int PK_Command,int NumParms,...);
	// For each MD, all it's direct children go in the same room, and if it has an on-screen Orbiter, it's direct children too
	void PutMDsChildrenInRoom(DeviceData_Router *pDeviceData_Router);

    /**
     * @brief Required for plug-ins that render floorplans
     */
    virtual void GetFloorplanDeviceInfo(DeviceData_Router *pDeviceData_Router,EntertainArea *pEntertainArea,int iFloorplanObjectType,int &iPK_FloorplanObjectType_Color,int &Color,string &sDescription,string &OSD);

    bool StartMedia(MediaHandlerInfo *pMediaHandlerInfo, unsigned int PK_Device_Orbiter,EntertainArea *pEntertainArea,int PK_Device_Source,int PK_DesignObj_Remote,deque<MediaFile *> *dequeMediaFile,bool bResume,int iRepeat);
	bool StartMedia(MediaStream *pMediaStream,bool bNoChanges,int PK_MediaType_Prior, map<int,MediaDevice *> mapMediaDevice_Prior);

    /**
     * @brief More capable StartMedia. Does not need an actual device since it will search for it at the play time.
     *
     * It will also take care of moving the playlists when we use another device to play the media. It will also receive and actual device play list and not only
     *  a filename.
     */
    bool StartMediaOnPlugin(MediaHandlerInfo *pMediaHandlerInfo, EntertainArea *pEntertainArea);

    /**
     * @brief Whenever the state of the media changes, the plug-in should call this function so we can update all the orbiter's pictures, descriptions, etc.
     */
    void MediaInfoChanged(MediaStream *pMediaStream);

    // We need our own message queue so that if a message comes in we don't know how to handle, we first hand it off to the plug-in, and if
    // that doesn't handle it either, we send it to the actual media device
    bool ReceivedMessage(class Message *pMessage);

    // Our message interceptors
    /**
     * @brief EVENT_Media_Inserted event interceptor. A removable device should fire this event when it detected a new media inserted.
     */
    bool MediaInserted(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo);

    /**
     * @brief EVENT_Playback_Completed event interceptor. Called when the router finds an event of this type in the queue.
     */
    bool PlaybackCompleted( class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo);

    /**
     * @brief EVENT_Media_Followme event interceptor. Called when the router finds an event of this type in the queue.
     */
	bool MediaFollowMe( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );

	/**
	 * @brief EVENT_Ripping_Completed_CONST event interceptor.
	 */
	bool RippingCompleted( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );


	// Sometimes when MediaHanderBase::GetRenderDevices is called, only the top-level render devices (ie the media source)
	// is desired.  However, HandleOnOffs wants everything in the pipe, and the following function
	// can be used to add the other devices into the map
	void AddOtherDevicesInPipesToRenderDevices(map<int,MediaDevice *> *pmapMediaDevice);
	void AddOtherDevicesInPipes_Loop(DeviceData_Router *pDevice,map<int,MediaDevice *> *pmapMediaDevice);


	// Follow-me
	virtual void FollowMe_EnteredRoom(int iPK_Event, int iPK_Orbiter, int iPK_Users, int iPK_RoomOrEntArea, int iPK_RoomOrEntArea_Left);
	virtual void FollowMe_LeftRoom(int iPK_Event, int iPK_Orbiter, int iPK_Users, int iPK_RoomOrEntArea, int iPK_RoomOrEntArea_Left);

	void StreamEnded(MediaStream *pMediaStream,bool bSendOff=true,bool bDeleteStream=true);
	void MediaInEAEnded(EntertainArea *pEntertainArea);

	virtual bool SafeToReload();
//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class

			*****EVENT***** accessors inherited from base class
	void EVENT_Watching_Media(int iPK_Room);
	void EVENT_Stopped_Watching_Media(int iPK_Room);
	void EVENT_Listening_to_Media(int iPK_Room);
	void EVENT_Stopped_Listening_To_Medi(int iPK_Room);

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #43 - MH Play Media */
	/** The Orbiters send the play media command to the actual media handler. rnrnThe Orbiter can send anyone or a combination of parameters. rnrnIt's up to media handler to figure out how to handle it. The media handler must find out if the media is already pla */
		/** @param #2 PK_Device */
			/** The ID of the actual device to start playing. */
		/** @param #3 PK_DesignObj */
			/** The Remote Control to use for playing this media. */
		/** @param #13 Filename */
			/** The filename to play or a pipe delimited list of filenames. */
		/** @param #29 PK_MediaType */
			/** The ID of the media type descriptor (if it is TV, movie, audio, etc ..  ) */
		/** @param #44 PK_DeviceTemplate */
			/** The DeviceTemplate ID. */
		/** @param #45 PK_EntertainArea */
			/** The desired target area for the playback. If this is missing then the orbiter should decide the target based on his controlled area. */
		/** @param #116 Resume */
			/** If true, when this media finishes, resume whatever was playing previously.  Useful for making announcements and similar. */
		/** @param #117 Repeat */
			/** 0=default for media type, 1=loop, -1=do not loop */

	virtual void CMD_MH_Play_Media(int iPK_Device,string sPK_DesignObj,string sFilename,int iPK_MediaType,int iPK_DeviceTemplate,string sPK_EntertainArea,bool bResume,int iRepeat) { string sCMD_Result; CMD_MH_Play_Media(iPK_Device,sPK_DesignObj.c_str(),sFilename.c_str(),iPK_MediaType,iPK_DeviceTemplate,sPK_EntertainArea.c_str(),bResume,iRepeat,sCMD_Result,NULL);};
	virtual void CMD_MH_Play_Media(int iPK_Device,string sPK_DesignObj,string sFilename,int iPK_MediaType,int iPK_DeviceTemplate,string sPK_EntertainArea,bool bResume,int iRepeat,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #44 - MH Stop Media */
	/** Stop media. All parameters are optional. The Media handler must find out what media to stop. */
		/** @param #2 PK_Device */
			/** The specific device to stop media on. */
		/** @param #29 PK_MediaType */
			/** The type of media to stop. */
		/** @param #44 PK_DeviceTemplate */
			/** The type of device to stop the media on. */
		/** @param #45 PK_EntertainArea */
			/** This is the location on which we need to stop the media. This is optional. If not specified the orbiter will decide the location based on the controlled area. */

	virtual void CMD_MH_Stop_Media(int iPK_Device,int iPK_MediaType,int iPK_DeviceTemplate,string sPK_EntertainArea) { string sCMD_Result; CMD_MH_Stop_Media(iPK_Device,iPK_MediaType,iPK_DeviceTemplate,sPK_EntertainArea.c_str(),sCMD_Result,NULL);};
	virtual void CMD_MH_Stop_Media(int iPK_Device,int iPK_MediaType,int iPK_DeviceTemplate,string sPK_EntertainArea,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #65 - Jump Position In Playlist */
	/** This will allow an orbiter to change the current playing position in the playlist */
		/** @param #5 Value To Assign */
			/** The track to go to.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1. */

	virtual void CMD_Jump_Position_In_Playlist(string sValue_To_Assign) { string sCMD_Result; CMD_Jump_Position_In_Playlist(sValue_To_Assign.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Jump_Position_In_Playlist(string sValue_To_Assign,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #73 - MH Send Me To Remote */
	/** An Orbiter sends this command when it wants to go to the active remote control.  This device will send the sender of the command a 'goto' command with the current remote. */
		/** @param #93 Not Full Screen */
			/** Normally all Orbiters that are on screen displays (ie running on a media director) are always sent to a 'full screen'  application desktop as the remote whenever video is playing, since the user will have another remote control.  Setting this to 1 explici */

	virtual void CMD_MH_Send_Me_To_Remote(bool bNot_Full_Screen) { string sCMD_Result; CMD_MH_Send_Me_To_Remote(bNot_Full_Screen,sCMD_Result,NULL);};
	virtual void CMD_MH_Send_Me_To_Remote(bool bNot_Full_Screen,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #74 - Bind to Media Remote */
	/** When an orbiter goes to a media remote control screen, it fires this command so that the media plug-in knows it is sitting at a remote, and needs to be notified when the media changes or the cover art changes.  This should be in the onLoad commands of eve */
		/** @param #2 PK_Device */
			/** The device the orbiter is controlling. */
		/** @param #3 PK_DesignObj */
			/** The object where the remote displays the graphical image of the cover art.  It will get update object images when the cover art changes. */
		/** @param #8 On/Off */
			/** If 1, bind (the Orbiter is sitting at the remote screen).  If 0, the orbiter has left the remote screen, and does not need media changed commands. */
		/** @param #16 PK_DesignObj_CurrentScreen */
			/** The current screen. */
		/** @param #25 PK_Text */
			/** The text object that contains the media description.  This will get set whenever the media changes, such as changing discs or channels. */
		/** @param #39 Options */
			/** Miscellaneous options.  These are not pre-defined, but are specific to a remote and the plug-in.  For example, the PVR plug-in needs to know what tuning device is active. */
		/** @param #45 PK_EntertainArea */
			/** The entertainment area the orbiter is controlling. */
		/** @param #56 PK_Text_Timecode */
			/** If the remote wants time code information, the object is stored here.  This can include an optional |n at the end, where n is the number of seconds to update (ie how often to update the counter). */
		/** @param #62 PK_Text_SectionDesc */
			/** The text object for the section description.  The section is tracks on a cd, or shows on a tv channel. */
		/** @param #63 PK_Text_Synopsis */
			/** The text object for the synopsis, a full description.  Examples are a DVD synopsis, or a description of a tv show. */

	virtual void CMD_Bind_to_Media_Remote(int iPK_Device,string sPK_DesignObj,string sOnOff,string sPK_DesignObj_CurrentScreen,int iPK_Text,string sOptions,string sPK_EntertainArea,int iPK_Text_Timecode,int iPK_Text_SectionDesc,int iPK_Text_Synopsis) { string sCMD_Result; CMD_Bind_to_Media_Remote(iPK_Device,sPK_DesignObj.c_str(),sOnOff.c_str(),sPK_DesignObj_CurrentScreen.c_str(),iPK_Text,sOptions.c_str(),sPK_EntertainArea.c_str(),iPK_Text_Timecode,iPK_Text_SectionDesc,iPK_Text_Synopsis,sCMD_Result,NULL);};
	virtual void CMD_Bind_to_Media_Remote(int iPK_Device,string sPK_DesignObj,string sOnOff,string sPK_DesignObj_CurrentScreen,int iPK_Text,string sOptions,string sPK_EntertainArea,int iPK_Text_Timecode,int iPK_Text_SectionDesc,int iPK_Text_Synopsis,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #214 - Save playlist */
	/** This will instruct the device to save the currently playing list */
		/** @param #17 PK_Users */
			/** The user that will own the new playlist. Can be missing. It will pick the current user then. */
		/** @param #45 PK_EntertainArea */
			/** Which playlist to save. You can direct the command to save a specific entertainment area's playlist or you can leave it blank to pick the current entertainment area's playlist */
		/** @param #50 Name */
			/** It will use the this name when saving. If it is not specified it will either use the name of the loaded playlist in the database or it will generate a new one. */
		/** @param #77 Save as new */
			/** Save the playlist as a new playlist. This will override the default behaviour. (If this playlist was not loaded from the database it will be saved as new. If it was loaded it will be overridded). This will make it always save it as new. */

	virtual void CMD_Save_playlist(int iPK_Users,string sPK_EntertainArea,string sName,bool bSave_as_new) { string sCMD_Result; CMD_Save_playlist(iPK_Users,sPK_EntertainArea.c_str(),sName.c_str(),bSave_as_new,sCMD_Result,NULL);};
	virtual void CMD_Save_playlist(int iPK_Users,string sPK_EntertainArea,string sName,bool bSave_as_new,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #231 - Load Playlist */
	/** This will instruct the device to load the specific playlist. */
		/** @param #45 PK_EntertainArea */
			/** The entertainment area in which to load the  playlist. By defualt it will be the entertainment in which the current orbiter is running. */
		/** @param #78 EK_Playlist */
			/** The id of the playlist to load */

	virtual void CMD_Load_Playlist(string sPK_EntertainArea,int iEK_Playlist) { string sCMD_Result; CMD_Load_Playlist(sPK_EntertainArea.c_str(),iEK_Playlist,sCMD_Result,NULL);};
	virtual void CMD_Load_Playlist(string sPK_EntertainArea,int iEK_Playlist,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #241 - MH Move Media */
	/** Moves an existing media stream to the specified entertainment areas. */
		/** @param #41 StreamID */
			/** The ID of the media stream to move */
		/** @param #45 PK_EntertainArea */
			/** The entertainment area(s) to send the Media Stream to. */

	virtual void CMD_MH_Move_Media(int iStreamID,string sPK_EntertainArea) { string sCMD_Result; CMD_MH_Move_Media(iStreamID,sPK_EntertainArea.c_str(),sCMD_Result,NULL);};
	virtual void CMD_MH_Move_Media(int iStreamID,string sPK_EntertainArea,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #269 - Move Playlist entry Up */
	/** Moves a entry up in the current playlist. */
		/** @param #48 Value */
			/** The id of the entry that needs to be moved up. */

	virtual void CMD_Move_Playlist_entry_Up(int iValue) { string sCMD_Result; CMD_Move_Playlist_entry_Up(iValue,sCMD_Result,NULL);};
	virtual void CMD_Move_Playlist_entry_Up(int iValue,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #270 - Move Playlist entry Down */
	/** Moves a entry down in the current playlist. */
		/** @param #48 Value */
			/** The id of the entry that needs to be moved down in the playlist. */

	virtual void CMD_Move_Playlist_entry_Down(int iValue) { string sCMD_Result; CMD_Move_Playlist_entry_Down(iValue,sCMD_Result,NULL);};
	virtual void CMD_Move_Playlist_entry_Down(int iValue,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #271 - Remove playlist entry. */
	/** Removes an entry from the playlist. */
		/** @param #48 Value */
			/** The Id of the entry that needs to be removed from the playlist. */

	virtual void CMD_Remove_playlist_entry(int iValue) { string sCMD_Result; CMD_Remove_playlist_entry(iValue,sCMD_Result,NULL);};
	virtual void CMD_Remove_playlist_entry(int iValue,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #331 - Get EntAreas For Device */
	/** Returns the EntArea(s) that a given device is in. */
		/** @param #2 PK_Device */
			/** The Device */
		/** @param #9 Text */
			/** A comma delimted list of EntAreas */

	virtual void CMD_Get_EntAreas_For_Device(int iPK_Device,string *sText) { string sCMD_Result; CMD_Get_EntAreas_For_Device(iPK_Device,sText,sCMD_Result,NULL);};
	virtual void CMD_Get_EntAreas_For_Device(int iPK_Device,string *sText,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #337 - Rip Disk */
	/** This will try to RIP a DVD to the HDD. */
		/** @param #17 PK_Users */
			/** The user who needs this rip in his private area. */
		/** @param #50 Name */
			/** The target disk name. */

	virtual void CMD_Rip_Disk(int iPK_Users,string sName) { string sCMD_Result; CMD_Rip_Disk(iPK_Users,sName.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Rip_Disk(int iPK_Users,string sName,string &sCMD_Result,Message *pMessage);


//<-dceag-h-e->
};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
//<-dceag-const2-b->!
