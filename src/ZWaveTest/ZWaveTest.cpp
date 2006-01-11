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
		FILE* fstream = NULL;
		fstream = fopen("commands.txt", "r");
		if(fstream != NULL)
		{
			char buffer[1024];
			while(fgets(buffer, 1024, fstream) != NULL)
			{
				if(buffer[0] == '#' || strlen(buffer) == 0)
					continue;
				if(strncmp(buffer, "init", 4) == 0)
				{
					ZWJobInitialize * initJob = new ZWJobInitialize(zwAPI);
					if( initJob != NULL )
					{
						zwAPI->insertJob(initJob);
						zwAPI->start();
						zwAPI->listen();
					}
				}
				else
				{
					g_pPlutoLogger->Write(LV_WARNING, "unkown command %s", buffer);
				}
			}
		}
		else
			g_pPlutoLogger->Write(LV_WARNING, "unable to open commands.txt");
	}
	g_pPlutoLogger->Flush();
	return 0;
}
