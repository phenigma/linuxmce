//<-dceag-d-b->
#ifndef DCE_Router_h
#define DCE_Router_h

//	DCE Implemenation for #1 DCE Router

#include "Gen_Devices/DCE_RouterBase.h"
//<-dceag-d-e->

//<-dceag-decl-b->
class DCE_Router : public DCE_Router_Command
{
//<-dceag-decl-e->
	// Private member variables

	// Private methods
public:
	// Public member variables

//<-dceag-const-b->
public:
	// Constructors/Destructor
	DCE_Router(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
	virtual ~DCE_Router();
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

//<-dceag-h-e->
};

#endif
