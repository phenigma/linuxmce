/**
 *
 * @file MythTV_Player.h
 * @brief header file for the MythTV_Player class
 *
 */

//<-dceag-d-b->
#ifndef MythTV_Player_h
#define MythTV_Player_h

//	DCE Implemenation for #35 MythTV Player

#include "Gen_Devices/MythTV_PlayerBase.h"
//<-dceag-d-e->

#include <qapplication.h>
#include <libmyth/mythcontext.h>
#include <libmyth/mythdialogs.h>
#include <libmyth/lcddevice.h>

#include <libmythtv/tv.h>
#include <libmythtv/frame.h>
#include <libmythtv/NuppelVideoPlayer.h>

#include "MythMainWindowResizable.h"

class RatPoisonWrapper;

//<-dceag-decl-b->
namespace DCE
{
	class MythTV_Player : public MythTV_Player_Command
	{
//<-dceag-decl-e->

    /** Private member variables */
    QApplication                *m_pQApplication;
    MythMainWindowResizable     *m_pMythMainWindow;
    TV                          *m_pMythTV;
    int                          m_iControllingDevice;
    pthread_t                    m_qApplicationThreadId;
    RatPoisonWrapper            *m_pRatWrapper;

    protected:
    bool InitMythTvGlobalContext();
    bool InitMythTvStuff();
    void waitToFireMediaChanged();


    static void *ProcessQApplicationEventThreadFunction(void *);
    /** Private methods */

    public:
    /** Public member variables */

    //<-dceag-const-b->
public:
		// Constructors/Destructor
		MythTV_Player(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~MythTV_Player();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

    virtual bool Connect();
    //<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_Floorplan_Info();

			*****EVENT***** accessors inherited from base class
	void EVENT_Playback_Info_Changed(string sMediaDescription,string sSectionDescription,string sSynposisDescription);
	void EVENT_Error_Occured(string sError_Message);

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #75 - Start TV */
	/** Start TV playback on this device. */

	virtual void CMD_Start_TV() { string sCMD_Result; CMD_Start_TV(sCMD_Result,NULL);};
	virtual void CMD_Start_TV(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #76 - Stop TV */
	/** Stop TV playback on this device. */

	virtual void CMD_Stop_TV() { string sCMD_Result; CMD_Stop_TV(sCMD_Result,NULL);};
	virtual void CMD_Stop_TV(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #84 - Get Video Frame */
	/** Capture a Video frame */
		/** @param #19 Data */
			/** The video frame */
		/** @param #20 Format */
			/** One of the following: "jpg", "png" */
		/** @param #23 Disable Aspect Lock */
			/** If true, don't worry about the aspect ratio.  Try to get the requested width and height. */
		/** @param #41 StreamID */
			/** Optional.  For multi stream devices, like media players, this identifies the stream. */
		/** @param #60 Width */
			/** The desired width of the video frame.  The sender need not respect this. */
		/** @param #61 Height */
			/** The desired height of the video frame.  The sender need not respect this. */

	virtual void CMD_Get_Video_Frame(string sDisable_Aspect_Lock,int iStreamID,int iWidth,int iHeight,char **pData,int *iData_Size,string *sFormat) { string sCMD_Result; CMD_Get_Video_Frame(sDisable_Aspect_Lock.c_str(),iStreamID,iWidth,iHeight,pData,iData_Size,sFormat,sCMD_Result,NULL);};
	virtual void CMD_Get_Video_Frame(string sDisable_Aspect_Lock,int iStreamID,int iWidth,int iHeight,char **pData,int *iData_Size,string *sFormat,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #129 - PIP - Channel Up */
	/** Go the next channel */

	virtual void CMD_PIP_Channel_Up() { string sCMD_Result; CMD_PIP_Channel_Up(sCMD_Result,NULL);};
	virtual void CMD_PIP_Channel_Up(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #130 - PIP - Channel Down */
	/** Go the previous channel. */

	virtual void CMD_PIP_Channel_Down() { string sCMD_Result; CMD_PIP_Channel_Down(sCMD_Result,NULL);};
	virtual void CMD_PIP_Channel_Down(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #187 - Tune to channel */
	/** This will make the device to tune to a specific channel. */
		/** @param #68 ProgramID */
			/** The Program ID that we need to tune to. */

	virtual void CMD_Tune_to_channel(string sProgramID) { string sCMD_Result; CMD_Tune_to_channel(sProgramID.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Tune_to_channel(string sProgramID,string &sCMD_Result,Message *pMessage);


//<-dceag-h-e->
};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
