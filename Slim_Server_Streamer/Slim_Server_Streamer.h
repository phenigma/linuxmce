//<-dceag-d-b->
#ifndef Slim_Server_Streamer_h
#define Slim_Server_Streamer_h

//	DCE Implemenation for #53 Slim Server Streamer

#include "Gen_Devices/Slim_Server_StreamerBase.h"
//<-dceag-d-e->

//<-dceag-decl-b->
namespace DCE
{
	class Slim_Server_Streamer : public Slim_Server_Streamer_Command
	{
//<-dceag-decl-e->
		// Private member variables

		// Private methods
public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Slim_Server_Streamer(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Slim_Server_Streamer();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
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


	/** @brief COMMAND: #249 - Start Streaming */
	/** Starts streaming */
		/** @param #13 Filename */
			/** The filename to stream */
		/** @param #41 StreamID */
			/** Identifier for this streaming session. */
		/** @param #59 MediaURL */
			/** The url to use to play this stream. */

	virtual void CMD_Start_Streaming(string sFilename,int iStreamID,string *sMediaURL) { string sCMD_Result; CMD_Start_Streaming(sFilename.c_str(),iStreamID,sMediaURL,sCMD_Result,NULL);};
	virtual void CMD_Start_Streaming(string sFilename,int iStreamID,string *sMediaURL,string &sCMD_Result,Message *pMessage);


//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
