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

#include <iostream>

namespace DCE
{
    Logger *g_pPlutoLogger;
}

using namespace DCE;
using namespace std;

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
		cout << "Usage: MessageSendTest server [-targetType [device|category|template]] DeviceFrom DeviceTo MsgType(1=Command, 2=Event) MsgID [parm1id param1value] [parm2id parm2value] ..." << endl;
		cout << "\t Important:" << endl;
		cout << "\t\t- you can prefix parameter value with \"@filename.ext\" to insert a data parameter from a file" << endl;
		cout << "\t\t- the default target type is the device." << endl;
        return 1;
    }
    if (((argc) % 2) != 0)
    {
        printf("Parameters must be paired with an ID\n");
        return 1;
    }

    string ServerAddress=argv[1];

    g_pPlutoLogger = new FileLogger(stdout);
    if (g_pPlutoLogger == NULL)
        fprintf(stderr,"Problem creating logger.  Check params.\n");

    Event_Impl *pEvent = new Event_Impl(DEVICEID_MESSAGESEND, 0, ServerAddress);

	int baseMessageSpecPos = 2;
	int targetType = 0; // Device;
	if ( argv[2] == "-targetType" )
	{
		baseMessageSpecPos += 2;
		if ( argv[3] == "category" )
			targetType = 1;
		else if ( argv[3] == "template" )
			targetType = 2;
	}

    int msgFrom=atoi(argv[baseMessageSpecPos + 0]);
	int msgTo=atoi(argv[baseMessageSpecPos + 1]);
	int msgType=atoi(argv[baseMessageSpecPos + 2]);
	int msgID=atoi(argv[baseMessageSpecPos + 3]);

    Message *pMsg;

	switch ( targetType )
	{
		case 0: // the destination is not an actual device.
			pMsg = new Message(msgFrom, msgTo, PRIORITY_NORMAL, msgType, msgID, 0);
			break;
		case 1: // the target is a category
			pMsg = new Message(msgFrom, msgTo, true, BL_SameHouse,  PRIORITY_NORMAL, msgType, msgID, 0);
			break;
		case 2:
			pMsg = new Message(msgFrom, msgTo, BL_SameHouse,  PRIORITY_NORMAL, msgType, msgID, 0);
			break;
	}

	if ( pMsg == NULL )
	{
		cout << "Could not parse command line params. Exiting!" << endl;
		exit(100);
	}

    for(int i=baseMessageSpecPos + 4; i<argc; i+=2)
    {
        char *pParam = argv[i + 1];
        int ParamNum = atoi(argv[i]);
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

