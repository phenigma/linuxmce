//<-dceag-d-b->
#ifndef Mozilla_Plugin_h
#define Mozilla_Plugin_h

//	DCE Implemenation for #30 Mozilla Plug-in

#include "Gen_Devices/Mozilla_PluginBase.h"
//<-dceag-d-e->

#include "Datagrid_Plugin/Datagrid_Plugin.h"

#include <list>
#include <string>
using namespace std;

//<-dceag-decl-b->! custom
namespace DCE
{

class Mozilla_Plugin : public Mozilla_Plugin_Command, public DataGridGeneratorPlugIn
{
//<-dceag-decl-e->
//<-dceag-const-b->
public:
		// Constructors/Destructor
		Mozilla_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Mozilla_Plugin();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->
	// Private member variables
	class Datagrid_Plugin *m_pDatagrid_Plugin;

	// Private methods
	list<pair<string, string> > GetUserBookmarks(string sPK_User);
public:
	// Public member variables

	// Datagrids
	class DataGridTable * BookmarkList(string GridID, string Parms, void * ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage);

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
