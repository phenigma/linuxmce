//<-dceag-d-b->
#ifndef Media_Grids_Plugin_h
#define Media_Grids_Plugin_h

//	DCE Implemenation for #25 Media Grids Plug-in

#include "Gen_Devices/Media_Grids_PluginBase.h"
//<-dceag-d-e->

//<-dceag-decl-b->
namespace DCE
{
	class Media_Grids_Plugin : public Media_Grids_Plugin_Command
	{
//<-dceag-decl-e->
//<-dceag-const-b->
public:
		// Constructors/Destructor
		Media_Grids_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Media_Grids_Plugin();
		virtual bool Register();
//<-dceag-const-e->

	// Private methods
public:
	// Public member variables
	class Datagrid_Plugin *m_pDatagrid_Plugin;
	class Media_Plugin *m_pMedia_Plugin;
	class Orbiter_Plugin *m_pOrbiter_Plugin;

	// Datagrids
	class DataGridTable *MediaTracks(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage);

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
