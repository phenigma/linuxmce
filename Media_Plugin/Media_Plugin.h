//<-dceag-d-b->
#ifndef Media_Plugin_h
#define Media_Plugin_h

//  DCE Implemenation for #2 Media Plug-in

#include "Gen_Devices/Media_PluginBase.h"
//<-dceag-d-e->

#include "DeviceData_Router.h"
#include "DCE/Logger.h"
#include "DCERouter/DCERouter.h"
#include "MediaPluginInfo.h"
#include "pluto_main/Table_DeviceTemplate.h"
#include "pluto_main/Table_DeviceTemplate_MediaType.h"
#include "pluto_main/Table_DeviceTemplate_MediaType_DesignObj.h"
#include "Datagrid_Plugin/Datagrid_Plugin.h"
class Database_pluto_main;

// A Media Handler is derived from the Media Handler abstract class.  When it registers, it passes
// in a MediaPluginInfo pointer indicating what type of media it can play.  It may register several times
// with different types of media and different capabilities.

// All media plugins must implement this class
namespace DCE
{

class MediaPluginBase
{
public:
    class Media_Plugin *m_pMedia_Plugin;
    MediaPluginBase() {}

    virtual class MediaStream *CreateMediaStream(class MediaPluginInfo *pMediaPluginInfo,int PK_Device_Source,string Filename,int StreamID)=0;
    virtual bool StartMedia(class MediaStream *pMediaStream)=0;
    virtual bool StopMedia(class MediaStream *pMediaStream)=0;
    virtual bool BroadcastMedia(class MediaStream *pMediaStream)=0;
};

// This adds media specific information for a device, extending the DeviceData_Router
class MediaDevice
{
public:
    MediaDevice(class Router *pRouter,class Row_Device *pRow_Device);       // This will store extra information about itself
    class DeviceData_Router *m_pDeviceData_Router;  // The device in the router
    map<int,int> m_mapMediaType_DesignObj; // A map of all the remotes for the various screens
    list<class EntertainArea *> m_listEntertainArea;
};



// An entertainment area is a cluster of devices.  Only 1
class EntertainArea
{
public:
    EntertainArea(int iPK_EntertainArea,bool bOnly1Stream) { m_iPK_EntertainArea=iPK_EntertainArea; m_bOnly1Stream=bOnly1Stream; m_pMediaStream=NULL; }
    int m_iPK_EntertainArea;
    class MediaStream *m_pMediaStream;   // The current media streams in this entertainment area
    map<int,class MediaDevice *> m_mapMediaDevice;  // All the media devices in the area
    MediaDevice *m_mapMediaDevice_Find(int PK_Device) {
        map<int,class MediaDevice *>::iterator it = m_mapMediaDevice.find(PK_Device);
        return it==m_mapMediaDevice.end() ? NULL : (*it).second;
    }
    MapBoundRemote m_mapBoundRemote;
    BoundRemote *m_mapBoundRemote_Find(int iPK_Orbiter) { map<int,class BoundRemote *>::iterator it = m_mapBoundRemote.find(iPK_Orbiter);   return it==m_mapBoundRemote.end() ? NULL : (*it).second; }
    void m_mapBoundRemote_Remove(int iPK_Orbiter) { MapBoundRemote::iterator it = m_mapBoundRemote.find(iPK_Orbiter); if( it!=m_mapBoundRemote.end() ) m_mapBoundRemote.erase(it); }

    bool m_bOnly1Stream; // If true, stop any other media streams in this area when a new one starts.  There can be only 1.  This is the default behavior

    map<int,List_MediaPluginInfo *> m_mapMediaPluginInfo_MediaType; // The key is the media type
    List_MediaPluginInfo *m_mapMediaPluginInfo_MediaType_Find(int PK_MediaType) {
        map<int,List_MediaPluginInfo *>::iterator it = m_mapMediaPluginInfo_MediaType.find(PK_MediaType);
        return it==m_mapMediaPluginInfo_MediaType.end() ? NULL : (*it).second;
    }

    map<string,List_MediaPluginInfo *> m_mapMediaPluginInfo_Extension; // The key is a file extension
    List_MediaPluginInfo *m_mapMediaPluginInfo_Extension_Find(string Extension) {
        map<string,List_MediaPluginInfo *>::iterator it = m_mapMediaPluginInfo_Extension.find(Extension);
        return it==m_mapMediaPluginInfo_Extension.end() ? NULL : (*it).second;
    }
};
typedef map<int,class EntertainArea *> MapEntertainArea;

// When a media device can save and restore a position, it must create a derived class that stores the position information.
// For a CD, this may be a track and timecount.  For a DVD it is more complicated, involving a lot of settings, like sub-title and audio, needed to restore this position/state
// GetID must return a unique string identifying the format.  This way media devices can see the format based on GetID, and cast MediaPosition to the correct type, handling it accordingly
class MediaPosition
{
public:
    virtual string GetID()=0;
};


//<-dceag-decl-b->!
class Media_Plugin : public Media_Plugin_Command, public DataGridGeneratorPlugIn
{
//<-dceag-decl-e->
    friend class MediaStream;
//<-dceag-const-b->
public:
        // Constructors/Destructor
        Media_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
        virtual ~Media_Plugin();
        virtual bool Register();
//<-dceag-const-e->
    friend class MediaPluginInfo;
    pluto_pthread_mutex_t m_MediaMutex; // Other classes may need this

private:
    // Private member variables
    class Orbiter_Plugin *m_pOrbiter_Plugin;
    int m_iStreamID;
    pthread_mutexattr_t m_MutexAttr;
    MapMediaStream m_mapMediaStream; // All active streams

    Database_pluto_main *m_pDatabase_pluto_main;
    class Datagrid_Plugin *m_pDatagrid_Plugin;
    class DataGridTable *CurrentMedia(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage);
    class DataGridTable *MediaSections(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage);

    // Private methods
public:
    // Public member variables
    map<int,EntertainArea *> m_mapEntertainAreas;  // Plug-ins may want this
    map<int,MediaDevice *> m_mapMediaDevice;

    EntertainArea *m_mapEntertainAreas_Find(int iPK_EntertainArea) { map<int,class EntertainArea *>::iterator it = m_mapEntertainAreas.find(iPK_EntertainArea); return it==m_mapEntertainAreas.end() ? NULL : (*it).second; }
    MediaDevice *m_mapMediaDevice_Find(int iPK_Device) { map<int,class MediaDevice *>::iterator it = m_mapMediaDevice.find(iPK_Device); return it==m_mapMediaDevice.end() ? NULL : (*it).second; }
    MediaStream *m_mapMediaStream_Find(int StreamID)  { MapMediaStream::iterator it = m_mapMediaStream.find(StreamID);  return it==m_mapMediaStream.end() ? NULL : (*it).second; }
    void m_mapMediaStream_Remove(int StreamID) { MapMediaStream::iterator it = m_mapMediaStream.find(StreamID); if( it!=m_mapMediaStream.end() ) m_mapMediaStream.erase(it); }

    // A MediaPlugIn call call this function, passing in the type of device that it will handle.  This will automatically build MediaPlugInInfo's for all the
    // types that the device supports, using all the default values from the database.  A plug-in could also call the MediaPluginInfo constructor that will
    // create and register a specific type of media.  This means it is not necessary to call RegisterMediaPlugin.  This allows the plug-in to change
    // some of the defaults.  The Plugin could also create a MediaPluginInfo type manually for complete control over the parameters, and then call
    // RegisterMediaPlugin
    void RegisterMediaPlugin(class Command_Impl *pCommand_Impl,class MediaPluginBase *pMediaPluginBase,int iPK_MasterDeviceList,bool bUsesDCE)
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

        g_pPlutoLogger->Write(LV_STATUS,"Registered media plug in #%d (Template %d) %s",iPKDevice,iPKDeviceTemplate,strDescription.c_str());
        vector<Row_DeviceTemplate_MediaType *> vectRow_DeviceTemplate_MediaType;
        pRow_DeviceTemplate->DeviceTemplate_MediaType_FK_DeviceTemplate_getrows(&vectRow_DeviceTemplate_MediaType);
        for(size_t mt=0;mt<vectRow_DeviceTemplate_MediaType.size();++mt)
        {
            Row_DeviceTemplate_MediaType *pRow_DeviceTemplate_MediaType = vectRow_DeviceTemplate_MediaType[mt];
            MediaPluginInfo *pMediaPluginInfo =
                new MediaPluginInfo(pMediaPluginBase,pCommand_Impl,pRow_DeviceTemplate_MediaType->FK_MediaType_get(),
                    iPK_MasterDeviceList,pRow_DeviceTemplate_MediaType->CanSetPosition_get()==1,bUsesDCE);

            vector<Row_DeviceTemplate_MediaType_DesignObj *> vectRow_DeviceTemplate_MediaType_DesignObj;
            pRow_DeviceTemplate_MediaType->DeviceTemplate_MediaType_DesignObj_FK_DeviceTemplate_MediaType_getrows(&vectRow_DeviceTemplate_MediaType_DesignObj);
            if( vectRow_DeviceTemplate_MediaType_DesignObj.size() )
                pMediaPluginInfo->m_iPK_DesignObj = vectRow_DeviceTemplate_MediaType_DesignObj[0]->FK_DesignObj_get();
            else
                pMediaPluginInfo->m_iPK_DesignObj = 0;
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

    bool StartMedia(MediaPluginInfo *pMediaPluginInfo,int PK_Device_Orbiter,EntertainArea *pEntertainArea,int PK_Device_Source,int PK_DesignObj_Remote,string Filename);

    // Whenever the state of the media changes, the plug-in should call this function so we can update all the orbiter's pictures, descriptions, etc.
    void MediaInfoChanged(MediaStream *pMediaStream);

    // We need our own message queue so that if a message comes in we don't know how to handle, we first hand it off to the plug-in, and if
    // that doesn't handle it either, we send it to the actual media device
    bool ReceivedMessage(class Message *pMessage);

    // Our message interceptors

    // We need to see all media inserted events so we can start the appropriate media devices
    bool MediaInserted(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo);

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
    COMMAND: #43 - MH Play Media
    COMMENTS: The Orbiters send the play media command to the actual media handler. rnrnThe Orbiter can send anyone or a combination of parameters. rnrnIt's up to media handler to figure out how to handle it. The media handler must find out if the media is already pla
    PARAMETERS:
        #2 PK_Device
            The ID of the actual device to start playing.
        #3 PK_DesignObj
            The Remote Control to use for playing this media.
        #13 Filename
            The filename to play or a pipe delimited list of filenames.
        #29 PK_MediaType
            The ID of the media type descriptor (if it is TV, movie, audio, etc ..  )
        #44 PK_DeviceTemplate
            The DeviceTemplate ID.
        #45 PK_EntertainArea
            The desired target area for the playback. If this is missing then the orbiter should decide the target based on his controlled area.
*/
    virtual void CMD_MH_Play_Media(int iPK_Device,string sPK_DesignObj,string sFilename,int iPK_MediaType,int iPK_DeviceTemplate,int iPK_EntertainArea) { string sCMD_Result; CMD_MH_Play_Media(iPK_Device,sPK_DesignObj.c_str(),sFilename.c_str(),iPK_MediaType,iPK_DeviceTemplate,iPK_EntertainArea,sCMD_Result,NULL);};
    virtual void CMD_MH_Play_Media(int iPK_Device,string sPK_DesignObj,string sFilename,int iPK_MediaType,int iPK_DeviceTemplate,int iPK_EntertainArea,string &sCMD_Result,Message *pMessage);

/*
    COMMAND: #44 - MH Stop Media
    COMMENTS: Stop media. All parameters are optional. The Media handler must find out what media to stop.
    PARAMETERS:
        #2 PK_Device
            The specific device to stop media on.
        #29 PK_MediaType
            The type of media to stop.
        #44 PK_DeviceTemplate
            The type of device to stop the media on.
        #45 PK_EntertainArea
            This is the location on which we need to stop the media. This is optional. If not specified the orbiter will decide the location based on the controlled area.
*/
    virtual void CMD_MH_Stop_Media(int iPK_Device,int iPK_MediaType,int iPK_DeviceTemplate,int iPK_EntertainArea) { string sCMD_Result; CMD_MH_Stop_Media(iPK_Device,iPK_MediaType,iPK_DeviceTemplate,iPK_EntertainArea,sCMD_Result,NULL);};
    virtual void CMD_MH_Stop_Media(int iPK_Device,int iPK_MediaType,int iPK_DeviceTemplate,int iPK_EntertainArea,string &sCMD_Result,Message *pMessage);

/*
    COMMAND: #73 - MH Send Me To Remote
    COMMENTS: An Orbiter sends this command when it wants to go to the active remote control.  This device will send the sender of the command a 'goto' command with the current remote.
    PARAMETERS:
*/
    virtual void CMD_MH_Send_Me_To_Remote() { string sCMD_Result; CMD_MH_Send_Me_To_Remote(sCMD_Result,NULL);};
    virtual void CMD_MH_Send_Me_To_Remote(string &sCMD_Result,Message *pMessage);

/*
    COMMAND: #74 - Bind to Media Remote
    COMMENTS: When an orbiter goes to a media remote control screen, it fires this command so that the media plug-in knows it is sitting at a remote, and needs to be notified when the media changes or the cover art changes.  This should be in the onLoad commands of eve
    PARAMETERS:
        #2 PK_Device
            The device the orbiter is controlling.
        #3 PK_DesignObj
            The object where the remote displays the graphical image of the cover art.  It will get update object images when the cover art changes.
        #8 On/Off
            If 1, bind (the Orbiter is sitting at the remote screen).  If 0, the orbiter has left the remote screen, and does not need media changed commands.
        #16 PK_DesignObj_CurrentScreen
            The current screen.
        #25 PK_Text
            The text object that contains the media description.  This will get set whenever the media changes, such as changing discs or channels.
        #39 Options
            Miscellaneous options.  These are not pre-defined, but are specific to a remote and the plug-in.  For example, the PVR plug-in needs to know what tuning device is active.
        #45 PK_EntertainArea
            The entertainment area the orbiter is controlling.
        #56 PK_Text_Timecode
            If the remote wants time code information, the object is stored here.  This can include an optional |n at the end, where n is the number of seconds to update (ie how often to update the counter).
        #62 PK_Text_SectionDesc
            The text object for the section description.  The section is tracks on a cd, or shows on a tv channel.
        #63 PK_Text_Synopsis
            The text object for the synopsis, a full description.  Examples are a DVD synopsis, or a description of a tv show.
*/
    virtual void CMD_Bind_to_Media_Remote(int iPK_Device,string sPK_DesignObj,string sOnOff,string sPK_DesignObj_CurrentScreen,int iPK_Text,string sOptions,int iPK_EntertainArea,int iPK_Text_Timecode,int iPK_Text_SectionDesc,int iPK_Text_Synopsis) { string sCMD_Result; CMD_Bind_to_Media_Remote(iPK_Device,sPK_DesignObj.c_str(),sOnOff.c_str(),sPK_DesignObj_CurrentScreen.c_str(),iPK_Text,sOptions.c_str(),iPK_EntertainArea,iPK_Text_Timecode,iPK_Text_SectionDesc,iPK_Text_Synopsis,sCMD_Result,NULL);};
    virtual void CMD_Bind_to_Media_Remote(int iPK_Device,string sPK_DesignObj,string sOnOff,string sPK_DesignObj_CurrentScreen,int iPK_Text,string sOptions,int iPK_EntertainArea,int iPK_Text_Timecode,int iPK_Text_SectionDesc,int iPK_Text_Synopsis,string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->
};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
