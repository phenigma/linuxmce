#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>
#include <list>


#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/StringUtils.h"
#include "DCE/Logger.h"
#include "DCE/Message.h"
//#include "DeviceCreator.h" //TODO : what/was is this?
#include "PlutoUtils/MyStl.h"
#include "VIPEmulatorSocket.h"
#include "VIPEmulator.h"
#include "VIPShared/PlutoConfig.h"


class PlutoConfig *gp_PlutoConfig=NULL;
class Logger *g_pPlutoLogger;

void* EmulatorThread(void* param)
{
	g_pPlutoLogger = new DCEFileLogger("C:\\temp.log");
	if(g_pPlutoLogger == NULL)
		fprintf(stderr,"Problem creating logger.  Check params.\n");


	VIPEmulator *pServer = new VIPEmulator();
	pServer->Initialize();
	pServer->StartListening(3461);//gp_PlutoConfig->m_iServerPort);
	pServer->Run();
	delete pServer;
	delete g_pPlutoLogger;
	return NULL;
}

DCESocket *VIPEmulator::CreateSocket(SOCKET newsock)
{
	DCESocket *pSocket = new VIPEmulatorSocket(this, newsock);
	return pSocket;
}

void VIPEmulator::Initialize()
{
}


void VIPEmulator::Run()
{
	if (m_ListenerThreadID)
		pthread_join(m_ListenerThreadID, NULL);
}
