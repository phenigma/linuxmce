//<-dceag-d-b->
#ifndef File_Grids_Plugin_h
#define File_Grids_Plugin_h

//	DCE Implemenation for #10 File Grids Plug-in

#include "Gen_Devices/File_Grids_PluginBase.h"
//<-dceag-d-e->

#include "Datagrid_Plugin/Datagrid_Plugin.h"
class Database_pluto_main;

//<-dceag-decl-b->! custom
namespace DCE
{
class File_Grids_Plugin : public File_Grids_Plugin_Command, public DataGridGeneratorPlugIn
{
//<-dceag-decl-e->
//<-dceag-const-b->
public:
		// Constructors/Destructor
		File_Grids_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~File_Grids_Plugin();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->
	// Private member variables
	class Datagrid_Plugin *m_pDatagrid_Plugin;
	Database_pluto_main *m_pDatabase_pluto_main;

	// Private methods
public:
	// Public member variables

	// Datagrids
	class DataGridTable *DevicesCommands(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage);
	class DataGridTable *FileList(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage);

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
