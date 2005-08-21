
/**
 *
 * @file Media_Plugin.h
 * @brief header file for the  Media_Plugin, MediaDevice, MediaHandlerBase classes
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
#include "MediaHandlerBase.h"
#include "pluto_main/Table_MediaType.h"
#include "pluto_main/Table_DeviceTemplate.h"
#include "pluto_main/Table_DeviceTemplate_MediaType.h"
#include "pluto_main/Table_DeviceTemplate_MediaType_DesignObj.h"
#include "pluto_main/Table_MediaType_DesignObj.h"

#include "pluto_media/Define_AttributeType.h"
#include "Datagrid_Plugin/Datagrid_Plugin.h"
#include "MediaAttributes.h"
#include "Gen_Devices/AllCommandsRequests.h"

#include "../Orbiter/Floorplan.h"
#include "Orbiter_Plugin/FollowMe_Device.h"
#include "MediaStream.h"
#include "EntertainArea.h"
#include "Orbiter/OrbiterFileBrowser.h"
#include "Orbiter_Plugin/OH_Orbiter.h"

class Database_pluto_main;
class Database_pluto_media;
class Row_EntertainArea;
class MediaFile;

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
	map<string, class RippingJob *> m_mapRippingJobs;
	map<int,string> m_mapMediaType_2_Directory;
	map<int,bool> m_mapMediaType_Bookmarkable;

	// Keep track of which users,media types want to be prompted to resume.  Valid char values are 'N' (never resume), 'A' (always resume), 'P' or anything else (prompt)
	map< pair<int,int>,char > m_mapPromptResume;

	// When deciding what remote control to use, we will look in these 4 maps in this order,
	// first for a specific remote for this orbiter/source device/mediatype combination, then for this
	// orbiter/media type, then for the source device/media type, lastly any remote for the media type
	map< pair<int,pair<int,int> >, class RemoteControlSet *> m_mapOrbiter_DeviceTemplate_MediaType_RemoteControl;
	map< pair<int,int>, class RemoteControlSet *> m_mapOrbiter_MediaType_RemoteControl;
	map< pair<int,int>, class RemoteControlSet *> m_mapDeviceTemplate_MediaType_RemoteControl;
	map< int, class RemoteControlSet *> m_mapMediaType_RemoteControl;


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
    class DataGridTable *DevicesPipes( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
	class DataGridTable *MediaAttrCurStream( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
	void AddMediaSectionToDataGrid(DataGridTable *pDataGrid,int &iRow,deque<MediaSection *> &dequeMediaSection,string sPreface);
	void AddMediaTitlesToDataGrid(DataGridTable *pDataGrid,int &iRow,deque<MediaTitle *> &dequeMediaTitle,string sPreface);
	void DevicesPipes_Loop(int PK_Orbiter,DeviceData_Router *pDevice,DataGridTable *&pDataGrid,int &iRow,int PK_Command_Input=0,int PK_Command_Output=0,vector<int> *p_vectDevice=NULL);

    class DataGridTable *AvailablePlaylists( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
    class DataGridTable *FloorplanMediaChoices( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
    class DataGridTable *ActiveMediaStreams( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );

    class DataGridTable *DVDSubtitles( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
    class DataGridTable *DVDAudioTracks( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
    class DataGridTable *DVDAngles( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
    class DataGridTable *Bookmarks( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
	class DataGridTable *BookmarksByMediaType( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );

	void PopulateRemoteControlMaps();

protected:
    void PlayMediaByDeviceTemplate(int iPK_DeviceTemplate, int iPK_Device, int iPK_Device_Orbiter, EntertainArea *pEntertainArea, string &sCMD_Result);
    void PlayMediaByFileName(string sFilename, int iPK_Device, int iPK_Device_Orbiter, EntertainArea *pEntertainArea, string &sCMD_Result);
    bool EnsureCorrectMediaStreamForDevice(MediaHandlerInfo *pMediaHandlerInfo, EntertainArea *pEntertainArea, int iPK_Device);

	bool DiskDriveIsRipping(int iPK_Device);
	bool HandleDeviceOnOffEvent(MediaDevice *pMediaDevice,bool bIsOn);

	RemoteControlSet *PickRemoteControlMap(int PK_Orbiter,int iPK_DeviceTemplate,int PK_MediaType)
	{
		// We're going to look in 4 places, from most specific to least specific
		if( PK_Orbiter )
		{
			// The first 2 maps will see if the user directly specified a remote control to use for the given remote
			map< pair<int,pair<int,int> >, class RemoteControlSet *>::iterator it1;
			pair<int,pair<int,int> > p1 = make_pair< int,pair<int,int> > (
				PK_Orbiter,
				make_pair< int,int > ( iPK_DeviceTemplate, PK_MediaType ) );
			if( (it1=m_mapOrbiter_DeviceTemplate_MediaType_RemoteControl.find(p1))!=m_mapOrbiter_DeviceTemplate_MediaType_RemoteControl.end() )
				return it1->second;

			map< pair<int,int>, class RemoteControlSet *>::iterator it2;
			pair<int,int> p2 = make_pair< int,int > (
				PK_Orbiter,
				PK_MediaType );
			if( (it2=m_mapOrbiter_MediaType_RemoteControl.find(p2))!=m_mapOrbiter_MediaType_RemoteControl.end() )
				return it2->second;
		}

		map< pair<int,int>, class RemoteControlSet *>::iterator it3;
		pair<int,int> p3 = make_pair< int,int > (
			iPK_DeviceTemplate,
			PK_MediaType );

		if( (it3=m_mapDeviceTemplate_MediaType_RemoteControl.find(p3))!=m_mapDeviceTemplate_MediaType_RemoteControl.end() )
			return it3->second;

		map< int, class RemoteControlSet *>::iterator it4;
		if( (it4=m_mapMediaType_RemoteControl.find(PK_MediaType))!=m_mapMediaType_RemoteControl.end() )
			return it4->second;

		return NULL;
	}

	/**
	 * Sometimes we just want to override the description with a
	 * status like 'pause', '2x', etc.
	 */
	void OverrideNowPlaying(MediaStream *pMediaStream,string sNowPlaying);

	/**
	 * Parse media identification that came in CDDB Format 
	 */

	void Parse_CDDB_Media_ID(MediaStream *pMediaStream,string sValue);
	void Parse_Misc_Media_ID(MediaStream *pMediaStream,string sValue);

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
	void TurnDeviceOff(int PK_Pipe,DeviceData_Router *pDeviceData_Router,map<int,MediaDevice *> *pmapMediaDevice_Current,vector<int> *p_vectDevice=NULL);

    /**
     * Add the file to the pluto_media database
     */
	void AddFileToDatabase(MediaFile *pMediaFile,int PK_MediaType);
	void AddRippedDiscToDatabase(RippingJob *pRippingJob);
	void AddDiscAttributesToFile(int PK_File,int PK_Disc,int Track);

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
	bool m_mapMediaType_Bookmarkable_Find(int iPK_MediaType) { map<int,bool>::iterator it=m_mapMediaType_Bookmarkable.find(iPK_MediaType); return it==m_mapMediaType_Bookmarkable.end() ? false : (*it).second; }
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
		vector<int> vectPK_MasterDeviceList;
		vectPK_MasterDeviceList.push_back(iPK_MasterDeviceList);
		RegisterMediaPlugin(pCommand_Impl,pMediaHandlerBase,vectPK_MasterDeviceList,bUsesDCE);
	}

    void RegisterMediaPlugin(class Command_Impl *pCommand_Impl,class MediaHandlerBase *pMediaHandlerBase,vector<int> &vectPK_MasterDeviceList,bool bUsesDCE);

    void AddDeviceToEntertainArea(EntertainArea *pEntertainArea,Row_Device *pRow_Device);
	void DetermineEntArea( int iPK_Device_Orbiter, int iPK_Device, string sPK_EntertainArea, vector<EntertainArea *> &vectEntertainArea );
    class MediaStream *DetermineStreamOnOrbiter(int iPK_Device_Orbiter,bool bErrorIfNotFound=true)
    {
        vector<class EntertainArea *> vectEntertainArea;
		DetermineEntArea(iPK_Device_Orbiter,0,"",vectEntertainArea);
        if( (vectEntertainArea.size()!=1 || !vectEntertainArea[0]->m_pMediaStream) && bErrorIfNotFound ) g_pPlutoLogger->Write(LV_CRITICAL,"No stream on orbiter: %d",iPK_Device_Orbiter);
        return vectEntertainArea.size()==1 ? vectEntertainArea[0]->m_pMediaStream : NULL;
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
	void GetMediaHandlersForEA(int iPK_MediaType,int iPK_MediaProvider,vector<EntertainArea *> &vectEntertainArea, vector< pair< MediaHandlerInfo *,vector<EntertainArea *> > > &vectEA_to_MediaHandler);
	MediaDevice *GetMediaDeviceForEA(int iPK_MediaType,EntertainArea *pEntertainArea);

	/**
     * @brief Save the last positions so we can resume
     */
	void SaveLastPlaylistPosition(MediaStream *pMediaStream);
	void SaveLastFilePosition(MediaStream *pMediaStream);
	void SaveLastDiscPosition(MediaStream *pMediaStream);
	void RestoreMediaResumePreferences();
	void SaveMediaResumePreferences();
	int CheckForAutoResume(MediaStream *pMediaStream);

	/**
     * @brief Required for plug-ins that render floorplans
     */
    virtual void GetFloorplanDeviceInfo(DeviceData_Router *pDeviceData_Router,EntertainArea *pEntertainArea,int iFloorplanObjectType,int &iPK_FloorplanObjectType_Color,int &Color,string &sDescription,string &OSD,int &PK_DesignObj_Toolbar);

	// Get a list of all the screens an orbiter will use to show the user a file picker
	OrbiterFileBrowser_Collection *CreateOrbiterFileList(OH_Orbiter *pOH_Orbiter)
	{
		OrbiterFileBrowser_Collection *pOrbiterFileBrowser_Collection = new OrbiterFileBrowser_Collection();

		for(map<int,string>::iterator it=m_mapMediaType_2_Directory.begin();it!=m_mapMediaType_2_Directory.end();++it)
		{
			RemoteControlSet *pRemoteControlSet = 
				PickRemoteControlMap(pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,0,it->first);

			if( pRemoteControlSet )
			{
				OrbiterFileBrowser_Entry *pOrbiterFileBrowser_Entry = new OrbiterFileBrowser_Entry();
				pOrbiterFileBrowser_Entry->m_DesignObj=pRemoteControlSet->m_iPK_DesignObj_FileList;
				pOrbiterFileBrowser_Entry->m_DesignObj_Popup=pRemoteControlSet->m_iPK_DesignObj_FileList_Popup;
				pOrbiterFileBrowser_Entry->m_MediaType=it->first;
				pOrbiterFileBrowser_Entry->m_sFilename=m_mapMediaType_2_Directory[it->first];
				pOrbiterFileBrowser_Collection->m_mapOrbiterFileBrowser[it->first]=pOrbiterFileBrowser_Entry;
			}
		}
		return pOrbiterFileBrowser_Collection;
	}

	// This version is called by MH_Play_Media.  It may result in multiple handlers and multiple streams
	// if there isn't 1 handler that can do it all.  If p_vectMediaStream is passed it will have a list of all the streams that were created as a result
	void StartMedia( int iPK_MediaType, int iPK_MediaProvider, unsigned int iPK_Device_Orbiter, vector<EntertainArea *> &vectEntertainArea, int iPK_Device, deque<MediaFile *> *dequeMediaFile, bool bResume, int iRepeat, string sStartingPosition, vector<MediaStream *> *p_vectMediaStream=NULL);

	// This creates a single media stream for a given media handler and starts playing it by calling the next StartMedia, or returns NULL if it cannot create the stream
    MediaStream *StartMedia(MediaHandlerInfo *pMediaHandlerInfo, int iPK_MediaProvider, unsigned int PK_Device_Orbiter,vector<EntertainArea *> &vectEntertainArea,int PK_Device_Source,deque<MediaFile *> *dequeMediaFile,bool bResume,int iRepeat, string sStartingPosition, int iPK_Playlist=0);

	// This is the final stage of 'StartMedia' that starts playing the given stream.  This is also called when the stream changes, or is moved, and needs to be restarted
	bool StartMedia(MediaStream *pMediaStream);

    /**
     * @brief More capable StartMedia. Does not need an actual device since it will search for it at the play time.
     *
     * It will also take care of moving the playlists when we use another device to play the media. It will also receive and actual device play list and not only
     *  a filename.
     */
    bool StartMediaOnPlugin(MediaHandlerInfo *pMediaHandlerInfo, EntertainArea *pEntertainArea);

    /**
     * @brief Whenever the state of the media changes, the plug-in should call this function so we can update all the orbiter's pictures, descriptions, etc.
	 * If the content of the media changed, such as when a new CD is identified and all the track lists need updating, set bRefreshScreen=true
     */
    void MediaInfoChanged(MediaStream *pMediaStream, bool bRefreshScreen=false );

    // We need our own message queue so that if a message comes in we don't know how to handle, we first hand it off to the plug-in, and if
    // that doesn't handle it either, we send it to the actual media device
    bool ReceivedMessage(class Message *pMessage);

    // Our message interceptors
    /**
     * @brief EVENT_Media_Inserted event interceptor. A removable device should fire this event when it detected a new media inserted.
     */
    bool MediaInserted(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo);

    /**
     * @brief EVENT_Media_Identified event interceptor, fired when a media identification device identifies media
     */
    bool MediaIdentified(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo);

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
	bool RippingProgress( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );
	void UpdateRippingStatus(RippingJob *pRippingJob,string sText,string sValue);

	/**
	 * @brief EVENT_Device_On_Off_CONST event interceptor, when some equipment was turned on or off manually
	 */
	bool DeviceOnOff( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );

	/**
	 * @brief EVENT_AV_Input_Changed_CONST event interceptor, when some equipment was has the input changed manually
	 */
	bool AvInputChanged( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );

	/**
	 * @brief EVENT_MediaDescriptionChanged_CONST event interceptor, when some equipment wants to change the description for the media it's playing
	 */
	bool MediaDescriptionChanged( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );


	// Sometimes when MediaHanderBase::GetRenderDevices is called, only the top-level render devices (ie the media source)
	// is desired.  However, HandleOnOffs wants everything in the pipe, and the following function
	// can be used to add the other devices into the map
	void AddOtherDevicesInPipesToRenderDevices(int PK_Pipe, map<int,MediaDevice *> *pmapMediaDevice);
	void AddOtherDevicesInPipes_Loop(int PK_Pipe, DeviceData_Router *pDevice,map<int,MediaDevice *> *pmapMediaDevice,vector<int> *p_vectDevice=NULL);


	// Follow-me
	virtual void FollowMe_EnteredRoom(int iPK_Event, int iPK_Orbiter, int iPK_Users, int iPK_RoomOrEntArea, int iPK_RoomOrEntArea_Left);
	virtual void FollowMe_LeftRoom(int iPK_Event, int iPK_Orbiter, int iPK_Users, int iPK_RoomOrEntArea, int iPK_RoomOrEntArea_Left);

	// If it ended because we're starting another stream, bSendOff==false so we don't turn everything off
	// If the new stream we're starting has bResume true, then bDeleteStream==false so we don't delete the stream since we'll be resuming it
	// If a new stream is replacing an old one, pMediaStream_Replacement is set to the new stream so we know if we should
	// fire the watching/listening events again.
	void StreamEnded(MediaStream *pMediaStream,bool bSendOff=true,bool bDeleteStream=true,MediaStream *pMediaStream_Replacement=NULL,vector<EntertainArea *> *p_vectEntertainArea=NULL);
	void MediaInEAEnded(EntertainArea *pEntertainArea,bool bFireEvent=true);

	virtual bool SafeToReload(string *sPendingTasks=NULL);

	// Given pMediaDevice, this function will find out if that device is presently being used by any media streams
	// and return a 1 if it's involved as the source, and 2 if the destination, or 0 if there's no involvement.
	// If it returns non-zero, pMediaStream will be set to the stream.  If it's a source, but not the direct source (ie fed by another)
	// pMediaDevice_Source will be the direct source.  The same is true with pMediaDevice_Dest
	// If it's involved in the destination, pEntertainArea will also be set
	int DetermineInvolvement(MediaDevice *pMediaDevice, MediaDevice *&pMediaDevice_Source,MediaDevice *&pMediaDevice_Dest,
		EntertainArea *&pEntertainArea,MediaStream *&pMediaStream);

	// This sends the set now playing command to an orbiter.  If pMessage is passed, it adds the command without sending it
	void SetNowPlaying( int dwPK_Device, string sNowPlaying, MediaStream *pMediaStream, bool bRefreshScreen, Message *pMessage=NULL )
	{
		string sRemotes;
		if( pMediaStream && pMediaStream->m_pRemoteControlSet )
			sRemotes = StringUtils::itos(pMediaStream->m_pRemoteControlSet->m_iPK_DesignObj_Remote) + ","
				+ StringUtils::itos(pMediaStream->m_pRemoteControlSet->m_iPK_DesignObj_Remote_Popup) + ","
				+ StringUtils::itos(pMediaStream->m_pRemoteControlSet->m_iPK_DesignObj_FileList) + ","
				+ StringUtils::itos(pMediaStream->m_pRemoteControlSet->m_iPK_DesignObj_FileList_Popup) + ","
				+ StringUtils::itos(pMediaStream->m_iPK_DesignObj_RemoteOSD);

		int PK_Device_Source=0,iDequeMediaFile=0;
		string sFilename;
		if( pMediaStream )
		{
			PK_Device_Source = pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device;
			iDequeMediaFile = pMediaStream->m_iDequeMediaFile_Pos;

			if( pMediaStream->m_iPK_MediaType==MEDIATYPE_pluto_CD_CONST )
			{
				sFilename = m_pMediaAttributes->GetAttributeName(pMediaStream->m_mapPK_Attribute[ATTRIBUTETYPE_Performer_CONST]);
				if( sFilename.size() )
					sFilename += "/"; // We got a performer

				sFilename += m_pMediaAttributes->GetAttributeName(pMediaStream->m_mapPK_Attribute[ATTRIBUTETYPE_Album_CONST]);
			}
			else if( pMediaStream->m_iPK_MediaType==MEDIATYPE_pluto_DVD_CONST )
				sFilename = pMediaStream->m_sMediaDescription;
		}

		if( StringUtils::StartsWith(sFilename,"<%=") )
			sFilename="";  // It's just a stock message--the user will have to pick

		DCE::CMD_Set_Now_Playing CMD_Set_Now_Playing( m_dwPK_Device, dwPK_Device, PK_Device_Source,
			sRemotes, sNowPlaying, sFilename, iDequeMediaFile, bRefreshScreen );
		if( pMessage )
			pMessage->m_vectExtraMessages.push_back(CMD_Set_Now_Playing.m_pMessage);
		else
			SendCommand( CMD_Set_Now_Playing );
	}

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

	virtual void CMD_MH_Play_Media(int iPK_Device,string sFilename,int iPK_MediaType,int iPK_DeviceTemplate,string sPK_EntertainArea,bool bResume,int iRepeat) { string sCMD_Result; CMD_MH_Play_Media(iPK_Device,sFilename.c_str(),iPK_MediaType,iPK_DeviceTemplate,sPK_EntertainArea.c_str(),bResume,iRepeat,sCMD_Result,NULL);};
	virtual void CMD_MH_Play_Media(int iPK_Device,string sFilename,int iPK_MediaType,int iPK_DeviceTemplate,string sPK_EntertainArea,bool bResume,int iRepeat,string &sCMD_Result,Message *pMessage);


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
			/** The target disk name, or for cd's, a comma-delimited list of names for each track. */
		/** @param #121 Tracks */
			/** For CD's, this must be a comma-delimted list of tracks (1 based) to rip. */
		/** @param #131 EK_Disc */
			/** The ID of the disc to rip */

	virtual void CMD_Rip_Disk(int iPK_Users,string sName,string sTracks,int iEK_Disc) { string sCMD_Result; CMD_Rip_Disk(iPK_Users,sName.c_str(),sTracks.c_str(),iEK_Disc,sCMD_Result,NULL);};
	virtual void CMD_Rip_Disk(int iPK_Users,string sName,string sTracks,int iEK_Disc,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #372 - MH Set Volume */
	/** Set the volume */
		/** @param #45 PK_EntertainArea */
			/** The Entertainment Area(s) */
		/** @param #76 Level */
			/** The level as an abolute value from 0-100, or a relative value like -1 or +1. */

	virtual void CMD_MH_Set_Volume(string sPK_EntertainArea,string sLevel) { string sCMD_Result; CMD_MH_Set_Volume(sPK_EntertainArea.c_str(),sLevel.c_str(),sCMD_Result,NULL);};
	virtual void CMD_MH_Set_Volume(string sPK_EntertainArea,string sLevel,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #388 - Set Media Private */
	/** Indicate if the media in an entertainment area is private or not. */
		/** @param #45 PK_EntertainArea */
			/** The entertainment area */
		/** @param #119 True/False */
			/** True if the media in this area is public (the default), false if it's not */

	virtual void CMD_Set_Media_Private(string sPK_EntertainArea,bool bTrueFalse) { string sCMD_Result; CMD_Set_Media_Private(sPK_EntertainArea.c_str(),bTrueFalse,sCMD_Result,NULL);};
	virtual void CMD_Set_Media_Private(string sPK_EntertainArea,bool bTrueFalse,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #391 - Add Media Attribute */
	/** Changes the media attribute */
		/** @param #5 Value To Assign */
			/** The new value of the attribute */
		/** @param #41 StreamID */
			/** The ID of the stream */
		/** @param #121 Tracks */
			/** If empty, the attribute is for the disc.  If specified, it is for this track number */
		/** @param #122 EK_AttributeType */
			/** The type of attribute to set */

	virtual void CMD_Add_Media_Attribute(string sValue_To_Assign,int iStreamID,string sTracks,int iEK_AttributeType) { string sCMD_Result; CMD_Add_Media_Attribute(sValue_To_Assign.c_str(),iStreamID,sTracks.c_str(),iEK_AttributeType,sCMD_Result,NULL);};
	virtual void CMD_Add_Media_Attribute(string sValue_To_Assign,int iStreamID,string sTracks,int iEK_AttributeType,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #392 - Set Media Attribute Text */
	/** Adds a new attribute */
		/** @param #5 Value To Assign */
			/** The new value.  If it's a name, LastName^Firstname format */
		/** @param #123 EK_Attribute */
			/** The attribute */

	virtual void CMD_Set_Media_Attribute_Text(string sValue_To_Assign,int iEK_Attribute) { string sCMD_Result; CMD_Set_Media_Attribute_Text(sValue_To_Assign.c_str(),iEK_Attribute,sCMD_Result,NULL);};
	virtual void CMD_Set_Media_Attribute_Text(string sValue_To_Assign,int iEK_Attribute,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #393 - Get Attribute */
	/** Returns the attribute text for the given ID */
		/** @param #9 Text */
			/** The value */
		/** @param #123 EK_Attribute */
			/** The attribute */

	virtual void CMD_Get_Attribute(int iEK_Attribute,string *sText) { string sCMD_Result; CMD_Get_Attribute(iEK_Attribute,sText,sCMD_Result,NULL);};
	virtual void CMD_Get_Attribute(int iEK_Attribute,string *sText,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #409 - Save Bookmark */
	/** Save the current position as a bookmark */
		/** @param #45 PK_EntertainArea */
			/** The entertainment area with the media */

	virtual void CMD_Save_Bookmark(string sPK_EntertainArea) { string sCMD_Result; CMD_Save_Bookmark(sPK_EntertainArea.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Save_Bookmark(string sPK_EntertainArea,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #410 - Delete Bookmark */
	/** Delete a bookmark */
		/** @param #129 EK_Bookmark */
			/** The bookmark to delete */

	virtual void CMD_Delete_Bookmark(int iEK_Bookmark) { string sCMD_Result; CMD_Delete_Bookmark(iEK_Bookmark,sCMD_Result,NULL);};
	virtual void CMD_Delete_Bookmark(int iEK_Bookmark,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #411 - Rename Bookmark */
	/** Rename a bookmark */
		/** @param #5 Value To Assign */
			/** The new name */
		/** @param #17 PK_Users */
			/** The user, if this is a private bookmark */
		/** @param #129 EK_Bookmark */
			/** The bookmark */

	virtual void CMD_Rename_Bookmark(string sValue_To_Assign,int iPK_Users,int iEK_Bookmark) { string sCMD_Result; CMD_Rename_Bookmark(sValue_To_Assign.c_str(),iPK_Users,iEK_Bookmark,sCMD_Result,NULL);};
	virtual void CMD_Rename_Bookmark(string sValue_To_Assign,int iPK_Users,int iEK_Bookmark,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #412 - Set Media Position */
	/** Position the current media to a bookmark */
		/** @param #41 StreamID */
			/** The stream to set */
		/** @param #42 MediaPosition */
			/** The media position.  When MediaPlugin gets this, it will be a bookmark ID, when a media player gets it, the string */

	virtual void CMD_Set_Media_Position(int iStreamID,string sMediaPosition) { string sCMD_Result; CMD_Set_Media_Position(iStreamID,sMediaPosition.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Set_Media_Position(int iStreamID,string sMediaPosition,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #417 - Set Auto Resume Options */
	/** Specify if the user should be prompted to resume the given media type or not */
		/** @param #5 Value To Assign */
			/** Valid values are: NEVER, ALWAYS, PROMPT */
		/** @param #17 PK_Users */
			/** The user to set */
		/** @param #29 PK_MediaType */
			/** The media type */

	virtual void CMD_Set_Auto_Resume_Options(string sValue_To_Assign,int iPK_Users,int iPK_MediaType) { string sCMD_Result; CMD_Set_Auto_Resume_Options(sValue_To_Assign.c_str(),iPK_Users,iPK_MediaType,sCMD_Result,NULL);};
	virtual void CMD_Set_Auto_Resume_Options(string sValue_To_Assign,int iPK_Users,int iPK_MediaType,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #689 - Update Time Code */
	/** Updates the current running time for a media stream. */
		/** @param #41 StreamID */
			/** The Stream to update */
		/** @param #102 Time */
			/** The current time.  If there is both a section time and total time, they should be \t delimited, like 1:03\t60:30 */
		/** @param #132 Total */
			/** The total time.   If there is both a section time and total time, they should be \t delimited, like 1:03\t60:30 */

	virtual void CMD_Update_Time_Code(int iStreamID,string sTime,string sTotal) { string sCMD_Result; CMD_Update_Time_Code(iStreamID,sTime.c_str(),sTotal.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Update_Time_Code(int iStreamID,string sTime,string sTotal,string &sCMD_Result,Message *pMessage);


//<-dceag-h-e->
};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
//<-dceag-const2-b->!
