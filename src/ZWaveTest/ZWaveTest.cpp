// ZWaveTest.cpp : Defines the entry point for the console application.
//

#include "DCE/Logger.h"
#include "DCE/ServerLogger.h"
#include "Serial/SerialPort.h" 

#include "PlutoZWSerialAPI.h"
#include "ZWJobInitialize.h"

using namespace DCE;

namespace DCE
{
	Logger *g_pPlutoLogger = NULL;
}

int main(int argc, char* argv[])
{
	g_pPlutoLogger = new FileLogger("ZWaveText.log");
	g_pPlutoLogger->Write(LV_WARNING, "------- BEGIN --------");

	PlutoZWSerialAPI * zwAPI = PlutoZWSerialAPI::instance();
	if( zwAPI != NULL )
	{
		ZWJobInitialize * initJob = new ZWJobInitialize(zwAPI);
		if( initJob != NULL )
		{
			zwAPI->insertJob(initJob);
			zwAPI->start();
			zwAPI->listen();
		}
	}
	g_pPlutoLogger->Flush();
		
	return 0;
}


