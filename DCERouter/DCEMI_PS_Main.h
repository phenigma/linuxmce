#ifndef DCEMI_PS_Main_h
#define DCEMI_PS_Main_h

#include "PlugIn.h"
#include "../pluto_main/Define_PlugIn.h"
#include "PlutoUtils/MySqlHelper.h"

class DCEMI_PS_Main : public PlugIn, public MySqlHelper
{
public:
	// Required overrides
	DCEMI_PS_Main(class Router *pRouter) : PlugIn(pRouter) {}
	virtual bool Register();
	virtual int ID_get() { return DCEPLUGIN_PS_Main_CONST; }
	virtual string Description_get() { return "Pluto DCE Plug-in: Main Pluto Core Plug-in v: 2.0"; }

	/*
		Message Interceptors
	*/

	// Handle an event indicating that the number of messages for a user has changed
};

#endif
