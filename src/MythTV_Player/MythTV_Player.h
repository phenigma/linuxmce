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

// #include <qapplication.h>
// #include <libmyth/mythcontext.h>
// #include <libmyth/mythdialogs.h>
// #include <libmyth/lcddevice.h>
//
// #include <libmythtv/tv.h>
// #include <libmythtv/frame.h>
// #include <libmythtv/NuppelVideoPlayer.h>

// #include "MythMainWindowResizable.h"

class RatPoisonWrapper;

//<-dceag-decl-b->
namespace DCE
{
	class MythTV_Player : public MythTV_Player_Command
	{
//<-dceag-decl-e->
    private:
        long unsigned int            m_iMythFrontendWindowId;

        /** Private member variables */
//        QApplication                *m_pQApplication;
//        MythMainWindowResizable     *m_pMythMainWindow;
//        TV                          *m_pMythTV;
        int                          m_iControllingDevice;
        pthread_t                    m_qApplicationThreadId;
        RatPoisonWrapper            *m_pRatWrapper;

        void selectWindow();
        bool checkWindowName(long unsigned int window, string windowName);

    protected:
//        void waitToFireMediaChanged();

        bool LaunchMythFrontend();

        void processKeyBoardInputRequest(int iXKeySym);

        // This should be Window but if i put #include <X11/Xlib.h>  in this it will break the compilation.
        bool locateMythTvFrontendWindow(long unsigned int window);

		bool checkXServerConnection();
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

    virtual void CreateChildren();
    //<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class

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
			/** Format of the frame */
		/** @param #23 Disable Aspect Lock */
			/** Disable Aspect Ratio */
		/** @param #41 StreamID */
			/** The ID of the stream */
		/** @param #60 Width */
			/** Frame width */
		/** @param #61 Height */
			/** Frame height */

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
		/** @param #39 Options */
			/** Extra data to allow the receiver to properly identify the channel ( this is the xmltvid value inside the mythtv database). */
		/** @param #68 ProgramID */
			/** The Program ID that we need to tune to. */

	virtual void CMD_Tune_to_channel(string sOptions,string sProgramID) { string sCMD_Result; CMD_Tune_to_channel(sOptions.c_str(),sProgramID.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Tune_to_channel(string sOptions,string sProgramID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #190 - Enter/Go */
	/** Enter was hit */

	virtual void CMD_EnterGo() { string sCMD_Result; CMD_EnterGo(sCMD_Result,NULL);};
	virtual void CMD_EnterGo(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #200 - Move Up */
	/** Up */

	virtual void CMD_Move_Up() { string sCMD_Result; CMD_Move_Up(sCMD_Result,NULL);};
	virtual void CMD_Move_Up(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #201 - Move Down */
	/** Down */

	virtual void CMD_Move_Down() { string sCMD_Result; CMD_Move_Down(sCMD_Result,NULL);};
	virtual void CMD_Move_Down(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #202 - Move Left */
	/** Left */

	virtual void CMD_Move_Left() { string sCMD_Result; CMD_Move_Left(sCMD_Result,NULL);};
	virtual void CMD_Move_Left(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #203 - Move Right */
	/** Right */

	virtual void CMD_Move_Right() { string sCMD_Result; CMD_Move_Right(sCMD_Result,NULL);};
	virtual void CMD_Move_Right(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #204 - 0 */
	/** 0 */

	virtual void CMD_0() { string sCMD_Result; CMD_0(sCMD_Result,NULL);};
	virtual void CMD_0(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #205 - 1 */
	/** 1 */

	virtual void CMD_1() { string sCMD_Result; CMD_1(sCMD_Result,NULL);};
	virtual void CMD_1(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #206 - 2 */
	/** 2 */

	virtual void CMD_2() { string sCMD_Result; CMD_2(sCMD_Result,NULL);};
	virtual void CMD_2(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #207 - 3 */
	/** 3 */

	virtual void CMD_3() { string sCMD_Result; CMD_3(sCMD_Result,NULL);};
	virtual void CMD_3(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #208 - 4 */
	/** 4 */

	virtual void CMD_4() { string sCMD_Result; CMD_4(sCMD_Result,NULL);};
	virtual void CMD_4(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #209 - 5 */
	/** 5 */

	virtual void CMD_5() { string sCMD_Result; CMD_5(sCMD_Result,NULL);};
	virtual void CMD_5(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #210 - 6 */
	/** 6 */

	virtual void CMD_6() { string sCMD_Result; CMD_6(sCMD_Result,NULL);};
	virtual void CMD_6(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #211 - 7 */
	/** 7 */

	virtual void CMD_7() { string sCMD_Result; CMD_7(sCMD_Result,NULL);};
	virtual void CMD_7(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #212 - 8 */
	/** 8 */

	virtual void CMD_8() { string sCMD_Result; CMD_8(sCMD_Result,NULL);};
	virtual void CMD_8(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #213 - 9 */
	/** 9 */

	virtual void CMD_9() { string sCMD_Result; CMD_9(sCMD_Result,NULL);};
	virtual void CMD_9(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #240 - Back / Prior Menu */
	/** Navigate back .. ( Escape ) */

	virtual void CMD_Back_Prior_Menu() { string sCMD_Result; CMD_Back_Prior_Menu(sCMD_Result,NULL);};
	virtual void CMD_Back_Prior_Menu(string &sCMD_Result,Message *pMessage);


//<-dceag-h-e->
};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->


//<-dceag-const2-b->!
