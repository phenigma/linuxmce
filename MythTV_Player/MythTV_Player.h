//<-dceag-d-b->
#ifndef MythTV_Player_h
#define MythTV_Player_h

//	DCE Implemenation for #35 MythTV Player

#include "Gen_Devices/MythTV_PlayerBase.h"
//<-dceag-d-e->

#include <qapplication.h>
#include <mythtv/mythcontext.h>
#include <mythtv/mythdialogs.h>
#include <mythtv/lcddevice.h>

#include <tv.h>

//<-dceag-decl-b->
namespace DCE
{
	class MythTV_Player : public MythTV_Player_Command
	{
//<-dceag-decl-e->
    // Private member variables
    QApplication    *m_pQApplication;
    MythMainWindow  *m_pMythMainWindow;
    TV              *m_pMythTV;

protected:
    bool initMythTVGlobalContext();
    // Private methods
public:
    // Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		MythTV_Player(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~MythTV_Player();
		virtual bool Register();
//<-dceag-const-e->

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
	COMMAND: #75 - Start TV
	COMMENTS: Start TV playback on this device.
	PARAMETERS:
*/
	virtual void CMD_Start_TV() { string sCMD_Result; CMD_Start_TV(sCMD_Result,NULL);};
	virtual void CMD_Start_TV(string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #76 - Stop TV
	COMMENTS: Stop TV playback on this device.
	PARAMETERS:
*/
	virtual void CMD_Stop_TV() { string sCMD_Result; CMD_Stop_TV(sCMD_Result,NULL);};
	virtual void CMD_Stop_TV(string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #187 - Tune to channel
	COMMENTS: This will make the device to tune to a specific channel.
	PARAMETERS:
		#48 ProgramID
			The Program ID that we need to tune to.
*/
	virtual void CMD_Tune_to_channel(string sProgramID) { string sCMD_Result; CMD_Tune_to_channel(sProgramID.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Tune_to_channel(string sProgramID,string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->
};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
