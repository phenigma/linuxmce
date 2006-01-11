// ZWaveTest.cpp : Defines the entry point for the console application.
//

#include "DCE/Logger.h"
#include "DCE/ServerLogger.h"
#include "Serial/SerialPort.h" 

#include "PlutoZWSerialAPI.h"
#include "ZWJobInitialize.h"
#include "ZWJobLightLevel.h"
#include <string>

using namespace DCE;

namespace DCE
{
	Logger *g_pPlutoLogger = NULL;
}

int main(int argc, char* argv[])
{
	g_pPlutoLogger = new FileLogger("ZWaveText.log");
	g_pPlutoLogger->Write(LV_WARNING, "------- BEGIN --------");
	
	if(argc != 2)
	{
		printf("usage: %c <port name>\n", argv[0]);
	}
	
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
				std::string line = buffer;
				while(*line.begin() == ' ' || *line.begin() == '\t')
					line.erase(0, 1); 
				if(*line.begin() == '#' || line.size() == 0)
					continue;
				if(line.find("init") == 0)
				{
					ZWJobInitialize * initJob = new ZWJobInitialize(zwAPI);
					if( initJob != NULL )
					{
						zwAPI->insertJob(initJob);
						zwAPI->start(argv[1]);
						zwAPI->listen();
					}
				}
				else if(line.find("light") == 0)
				{//it should have a parameter "ON" or "OFF"
					unsigned char level = 0x00;
					unsigned char nodeID = 0x00;
					if(line.find("on") < line.size())
					{
						level = 0xFF;
					}
					else if(line.find("off") < line.size())
					{
						level = 0x00;
					}
					else
					{
						g_pPlutoLogger->Write(LV_WARNING, "usage: `light on <node id>` or `light off <node id>` ");
						continue;
					}
					while(*line.end() == ' ' || *line.end() == '\t' || *line.end() == '\r' || *line.end() == '\n')
						line.erase(line.size() - 1, 1);
					int nodeIndex = line.rfind(' ');
					nodeID =   (int) strtol(&(line.c_str()[nodeIndex]), (char **)NULL, 10);
					if(nodeID == 0 && errno == EINVAL)
					{
						g_pPlutoLogger->Write(LV_WARNING, "usage: `light on <node id>` or `light off <node id>` ");
						continue;
					}
					ZWJobLightLevel *lightJob = new ZWJobLightLevel(zwAPI, 0xff, nodeID);
					if(lightJob != NULL)
					{
						zwAPI->insertJob( lightJob );
						zwAPI->start(argv[1]);
						zwAPI->listen();
					}
				}
				else
				{
					g_pPlutoLogger->Write(LV_WARNING, "unkown  command %s", line.c_str());
				}
			}
		}
		else
			g_pPlutoLogger->Write(LV_WARNING, "unable to open commands.txt");
	}
	g_pPlutoLogger->Flush();
	return 0;
}
