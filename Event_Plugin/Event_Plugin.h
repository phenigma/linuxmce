//<-dceag-d-b->
#ifndef Event_Plugin_h
#define Event_Plugin_h

//	DCE Implemenation for #52 Event Plug-in

#include "Gen_Devices/Event_PluginBase.h"
//<-dceag-d-e->

#include "pluto_main/Database_pluto_main.h"
class Criteria;
class EventHandler;
class CriteriaParmNesting;
class Row_CriteriaParmNesting;


//<-dceag-decl-b->
namespace DCE
{
	class Event_Plugin : public Event_Plugin_Command
	{
//<-dceag-decl-e->
		// Private member variables
	    Database_pluto_main *m_pDatabase_pluto_main;
		map<int,Criteria *> m_mapCriteria;
		map<int,EventHandler *> m_mapEventHandler;
		Criteria *m_mapCriteria_Find(int PK_Criteria) { map<int,class Criteria *>::iterator it = m_mapCriteria.find(PK_Criteria); return it==m_mapCriteria.end() ? NULL : (*it).second; }

		// Private methods
public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Event_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Event_Plugin();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

		bool ProcessEvent(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo);
		CriteriaParmNesting *LoadCriteriaParmNesting(CriteriaParmNesting *pCriteriaParmNesting_Parent,Row_CriteriaParmNesting *pRow_CriteriaParmNesting);

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
