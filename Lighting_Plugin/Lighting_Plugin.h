
//<-dceag-d-b->
#ifndef Lighting_Plugin_h
#define Lighting_Plugin_h

//	DCE Implemenation for #32 Lighting Plug-in

#include "Gen_Devices/Lighting_PluginBase.h"
//<-dceag-d-e->

class Database_pluto_main;

//<-dceag-decl-b->
namespace DCE
{
	class Lighting_Plugin : public Lighting_Plugin_Command
	{
//<-dceag-decl-e->
	// Private member variables

	// Private methods
public:
	// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Lighting_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Lighting_Plugin();
		virtual bool Register();
//<-dceag-const-e->

	
	class Datagrid_Plugin *m_pDatagrid_Plugin;
	class Orbiter_Plugin *m_pOrbiter_Plugin;
	Database_pluto_main *m_pDatabase_pluto_main;

	// Datagrids
	class DataGridTable *LightingScenariosGrid( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );

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
