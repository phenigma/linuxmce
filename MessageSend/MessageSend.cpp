// OCDeviceTemplate.cpp : Defines the entry point for the console application.
//

#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"
#include "DCE/Logger.h"
#include "DCE/Message.h"
#include "DCE/DeviceData_Impl.h"

namespace DCE
{
	Logger *g_pPlutoLogger;
}

using namespace DCE;

#ifdef WIN32
int _tmain(int argc, _TCHAR* argv[])
{
	int err;
	WSADATA wsaData;
	err = WSAStartup(MAKEWORD( 1, 1 ),(LPWSADATA)  &wsaData);
#else
int main(int argc, char *argv[])
{
#endif

	if (argc<6)
	{
		printf("Usage: MessageSendTest server DeviceFrom DeviceTo MsgType(1=Command, 2=Event) MsgID [parm1id param1value] [parm2id parm2value] ...\n"
			   "Prefix parameter value with \"@filename.ext\" to insert a data parameter from a file\n\n");
		return 1;
	}
	if (((argc) % 2) != 0)
	{
		printf("Parameters must be paired with an ID\n");
		return 1;
	}

	string ServerAddress=argv[1];
	int DeviceID=10000;

	g_pPlutoLogger = new FileLogger(stdout);
	if (g_pPlutoLogger == NULL)
		fprintf(stderr,"Problem creating logger.  Check params.\n");

	Event_Impl *pEvent = new Event_Impl(DeviceID, ServerAddress);

	int msgFrom=atoi(argv[2]), msgTo=atoi(argv[3]), msgType=atoi(argv[4]), msgID=atoi(argv[5]);

	Message *pMsg = new Message(msgFrom, msgTo, PRIORITY_NORMAL, msgType, msgID, 0);

	for(int i=0;i<(argc-6)/2;i++)
	{
		char *pParam = argv[(i*2)+7];
		int ParamNum = atoi(argv[(i*2)+6]);
		if (pParam[0]=='@')
		{
			if (ParamNum < 0)
				ParamNum = -ParamNum;
            
			FILE *f = fopen(pParam+1, "rb");
			if (f)
			{
				fseek(f, 0, SEEK_END);
				size_t length = ftell(f);
				fseek(f, 0, SEEK_SET);
				char *pData = new char[length+1];
				fread(pData, length, 1, f);
				fclose(f);
				pMsg->m_mapData_Lengths[ParamNum]=(int)length;
				pMsg->m_mapData_Parameters[ParamNum]=pData;
			}
			else
			{
				printf("\nWarning: Could not open file %s.\n", pParam+1);
			}
		}
		else
		{
			pMsg->m_mapParameters[ParamNum] = string(pParam);
		}
	}
	pEvent->SendMessage(pMsg);

    Sleep(50);	
#ifdef _WIN32
	WSACleanup();
#endif

	return 0;
}

