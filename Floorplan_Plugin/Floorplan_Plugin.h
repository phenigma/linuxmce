//<-dceag-d-b->
#ifndef Floorplan_Plugin_h
#define Floorplan_Plugin_h

//	DCE Implemenation for #39 Floorplan Plug-in

#include "Gen_Devices/Floorplan_PluginBase.h"
//<-dceag-d-e->

//<-dceag-decl-b->
namespace DCE
{
	class Floorplan_Plugin : public Floorplan_Plugin_Command
	{
//<-dceag-decl-e->
		// Private member variables

		// Private methods
public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Floorplan_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Floorplan_Plugin();
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

/* 
	COMMAND: #183 - Get Floorplan Info
	COMMENTS: The orbiter will call this command to get a delimited list of all the floorplan objects and their locations on the map.
	PARAMETERS:
		#5 Value To Assign
			A | delimited list in the format, where {} indicate a repeating value: #pages,{#Types,{#Objects,{DeviceDescription, ObjectDescription, FillX Point, FillY Point, PK_DesignObj, Page, PK_Device, Type}}}
*/
	virtual void CMD_Get_Floorplan_Info(,string *sValue_To_Assign) { string sCMD_Result; CMD_Get_Floorplan_Info(sValue_To_Assign,sCMD_Result,NULL);};
	virtual void CMD_Get_Floorplan_Info(,string *sValue_To_Assign,string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->