//<-dceag-d-b->
#ifndef Climate_Plugin_h
#define Climate_Plugin_h

//	DCE Implemenation for #31 Climate Plug-in

#include "Gen_Devices/Climate_PluginBase.h"
//<-dceag-d-e->

#include "Orbiter_Plugin/FollowMe_Plugin.h"
#include "Datagrid_Plugin/Datagrid_Plugin.h"
class Database_pluto_main;

//<-dceag-decl-b->!
namespace DCE
{
	class Climate_Plugin : public Climate_Plugin_Command, public DataGridGeneratorPlugIn, public FollowMe_Plugin
	{
//<-dceag-decl-e->
	// Private member variables

	// Private methods
public:
	// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Climate_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Climate_Plugin();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

	class Datagrid_Plugin *m_pDatagrid_Plugin;
	class Orbiter_Plugin *m_pOrbiter_Plugin;
	Database_pluto_main *m_pDatabase_pluto_main;

	// Datagrids
	class DataGridTable *ClimateScenariosGrid(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage);

	// Follow-me
	virtual void FollowMe_EnteredRoom(OH_Orbiter *pOH_Orbiter,class Room *pRoom_Prior,class Room *pRoom_Current) {}
	virtual void FollowMe_LeftRoom(OH_Orbiter *pOH_Orbiter,class Room *pRoom_Prior,class Room *pRoom_Current) {}

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


//<-dceag-h-e->
};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
//<-dceag-const2-b->!
