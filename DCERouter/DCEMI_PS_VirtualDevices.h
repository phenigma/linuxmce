#ifndef DCEMI_PS_VirtualDevices_h
#define DCEMI_PS_VirtualDevices_h

#include "Router.h"
#include "Floorplan.h"
#include "DataGrid.h"
#include "MediaStream.h"



/*
	This device will have it's own Plug-in system for DataGrid's and Media Streams

	Plug-in callbacks
	Plug-in's can register (2) DataGridGenerators, and (3) sets of 4 Media call backs
*/




/*

	Classes to hold pointers to the call back functions, and general helper classes

*/



class DCEMI_PS_VirtualDevices : public PlutoPlugIn, public MySqlHelper
{
public:
	// Extra stuff
	pthread_mutexattr_t m_MutexAttr;
	



	DCEMI_PS_VirtualDevices(class Router *pRouter,class PlutoEvents *pPlutoEvents) ;
	~DCEMI_PS_VirtualDevices();
	int Register();
	void CheckForSpecialDevices();


};

#endif
