//<-dceag-d-b->
#ifndef Generic_Analog_Capture_Card_h
#define Generic_Analog_Capture_Card_h

//	DCE Implemenation for #64 Generic Analog Capture Card

#include "Gen_Devices/Generic_Analog_Capture_CardBase.h"
//<-dceag-d-e->
#include "DCE/DeviceData_Router.h"
class Database_pluto_main;

//<-dceag-decl-b->
namespace DCE
{
	class Generic_Analog_Capture_Card : public Generic_Analog_Capture_Card_Command
	{
//<-dceag-decl-e->
		// Private member variables

		// Private methods
public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Generic_Analog_Capture_Card(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Generic_Analog_Capture_Card();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		Generic_Analog_Capture_Card(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
//<-dceag-const2-e->

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_Type();
	int DATA_Get_Number_of_ports();
	string DATA_Get_Video_Standard();

			*****EVENT***** accessors inherited from base class

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #84 - Get Video Frame */
	/** Get's a picture from a specified surveilance camera */
		/** @param #19 Data */
			/** The video frame */
		/** @param #41 CameraID */
			/** The ID (nnumber) of the camera from where to take the snapshot. */

	virtual void CMD_Get_Video_Frame(int iCameraID,char **pData,int *iData_Size) { string sCMD_Result; CMD_Get_Video_Frame(iCameraID,pData,iData_Size,sCMD_Result,NULL);};
	virtual void CMD_Get_Video_Frame(int iCameraID,char **pData,int *iData_Size,string &sCMD_Result,Message *pMessage);


//<-dceag-h-e->
	private:
		    DeviceData_Router* find_Device(int iPK_Device);
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
