// ZWaveTest.cpp : Defines the entry point for the console application.
//

#include "DCE/Logger.h"
#include "DCE/ServerLogger.h"
#include "Serial/SerialPort.h" 

#include "PlutoZWSerialAPI.h"
#include "ZWJobInitialize.h"
#include "ZWJobLightLevel.h"
#include "ZWJobReceive.h"
#include "ZWJobReset.h"
#include "ZWJobAddNode.h"
#include "ZWJobRemoveNode.h"

#include <string>

using namespace DCE;

namespace DCE
{
	Logger *g_pPlutoLogger = NULL;
}

int main(int argc, char* argv[])
{
	g_pPlutoLogger = new FileLogger("ZWaveText.log");
#ifdef PLUTO_DEBUG
	g_pPlutoLogger->Write(LV_WARNING, "------- BEGIN --------");
#endif
	if(argc != 2)
	{
		printf("usage: %s <port name>\n", argv[0]);
		return 1;
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
					}
				}
				else if(line.find("receive") == 0)
				{
					ZWJobReceive * receiveJob = new ZWJobReceive(zwAPI);
					if( receiveJob != NULL )
					{
						zwAPI->insertJob(receiveJob);
					}
					ZWJobInitialize * initJob = new ZWJobInitialize(zwAPI);
					if( initJob != NULL )
					{
						zwAPI->insertJob(initJob);
					}
				}
				else if(line.find("reset") == 0)
				{
					ZWJobReset * resetJob = new ZWJobReset(zwAPI);
					if( resetJob != NULL )
					{
						zwAPI->insertJob(resetJob);
					}
					ZWJobInitialize * initJob = new ZWJobInitialize(zwAPI);
					if( initJob != NULL )
					{
						zwAPI->insertJob(initJob);
					}
				}
				else if(line.find("add_node") == 0)
				{
					ZWJobAddNode * addJob = new ZWJobAddNode(zwAPI);
					if( addJob != NULL )
					{
						zwAPI->insertJob(addJob);
					}
				}
				else if(line.find("remove_node") == 0)
				{
					ZWJobRemoveNode * removeJob = new ZWJobRemoveNode(zwAPI);
					if( removeJob != NULL )
					{
						zwAPI->insertJob(removeJob);
					}
				}
				else if(line.find("light") == 0)
				{//it should have a parameter "ON" or "OFF"
					unsigned char level = 0x00;
					unsigned char nodeID = 0x00;
					if(line.find("on") < line.size())
					{
						level = 0x63;
						g_pPlutoLogger->Write(LV_WARNING, "main: light on");
					}
					else if(line.find("off") < line.size())
					{
						level = 0x00;
						g_pPlutoLogger->Write(LV_WARNING, "main: light off");
					}
					else
					{
						g_pPlutoLogger->Write(LV_WARNING, "usage: `light on <node id>` or `light off <node id>` ");
						continue;
					}
					while(*line.end() == ' ' || *line.end() == '\t' || *line.end() == '\r' || *line.end() == '\n')
						line.erase(line.size() - 1, 1);
					size_t nodeIndex = line.rfind(' ');
					nodeID =   (int) strtol(&(line.c_str()[nodeIndex]), (char **)NULL, 10);
					if(nodeID == 0 && errno == EINVAL)
					{
						g_pPlutoLogger->Write(LV_WARNING, "usage: `light on <node id>` or `light off <node id>` ");
						continue;
					}
					ZWJobLightLevel *lightJob = new ZWJobLightLevel(zwAPI, level, nodeID);
					if(lightJob != NULL)
					{
						zwAPI->insertJob( lightJob );
					}
				}
				else if(line.find("sleep") == 0)
				{ //take a moment to relax
#if _WIN32
					Sleep(500);
#else
					usleep(500000);
#endif
				}
				else
				{
					g_pPlutoLogger->Write(LV_WARNING, "unkown  command %s", line.c_str());
				}
			}
			fclose(fstream);
			fstream = NULL;
			
			// process the commands
			zwAPI->start(argv[1]);
			zwAPI->listen();
			
			delete zwAPI;
			zwAPI = NULL;
			
			g_pPlutoLogger->Write(LV_WARNING, "******** END **********");
		}
		else
			g_pPlutoLogger->Write(LV_WARNING, "unable to open commands.txt");
	}
	g_pPlutoLogger->Flush();
	return 0;
}
