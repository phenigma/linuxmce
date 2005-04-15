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
		cout << "Usage: MessageSendTest server [-targetType [device|category|template]] [-r | -o] DeviceFrom DeviceTo MsgType(1=Command, 2=Event) MsgID [parm1id param1value] [parm2id parm2value] ..." << endl
			<< "\tthe default target type is the device." << endl
			<< "\t-r means the message will be sent with a response request" << endl
			<< "\t-o means the message will expect out parameters" << endl
			<< "\t\twhich will be echoed to the screen as resp: [response]" << endl
			<< "\tthe parm ID can be prefixed with a letter:" << endl
			<< "\t\tD send as a data paramter, rather than text" << endl
			<< "\t\tB the value is a filename, the contents sent as a data parameter" << endl
			<< "\t\tT the value is a filename, the contents sent as a text parameter" << endl;

        return 1;
    }

    g_pPlutoLogger = new FileLogger(stdout);
    if (g_pPlutoLogger == NULL)
        fprintf(stderr,"Problem creating logger.  Check params.\n");

	int baseMessageSpecPos = 1;
	int targetType = 0; // Device;
	bool bResponseRequired=false,bOutParams=false;
	while( argv[baseMessageSpecPos][0]=='-' )
	{
		if ( stricmp(argv[baseMessageSpecPos], "-targetType") == 0 )
		{
			if ( strcmp(argv[baseMessageSpecPos+1], "category") == 0 )
				targetType = 1;
			else if ( strcmp(argv[baseMessageSpecPos+1], "template") == 0 )
				targetType = 2;
			baseMessageSpecPos += 2;
		}
		else if ( strcmp(argv[baseMessageSpecPos], "-r") == 0 )
		{
			bResponseRequired=true;
			baseMessageSpecPos++;
		}
		else if ( strcmp(argv[baseMessageSpecPos], "-o") == 0 )
		{
			bOutParams=true;
			baseMessageSpecPos++;
		}
		else
		{
			cerr << "Unknown argument: " << argv[baseMessageSpecPos] << endl;
		}
	}

    string ServerAddress=argv[baseMessageSpecPos++];
	Event_Impl *pEvent = new Event_Impl(DEVICEID_MESSAGESEND, 0, ServerAddress);

    Message *pMsg=new Message(argc-baseMessageSpecPos,&argv[baseMessageSpecPos],targetType);

	// We need a response.  It will be a string if there are no out parameters
	if( !bOutParams )
	{
		if( bResponseRequired )
			pMsg->m_eExpectedResponse = ER_DeliveryConfirmation;  // i.e. just an "OK"
		string sResponse; // We'll use this only if a response wasn't passed in
		bool bResult;
		
		if( bResponseRequired )
			bResult = pEvent->SendMessage( pMsg, sResponse );
		else
			bResult = pEvent->SendMessage( pMsg );

		if( !bResponseRequired )
		{
			if( !bResult )
				cerr << "Failed to send" << endl;
			return bResult ? 0 : -1;
		}
		cout << "RESP: " << sResponse << endl;
		return -1;
	}

	// There are out parameters, we need to get a message back in return
	pMsg->m_eExpectedResponse = ER_ReplyMessage;
	Message *pResponse = pEvent->SendReceiveMessage( pMsg );
	if( !pResponse || pResponse->m_dwID != 0 )
	{
		if(pResponse)
			delete pResponse;

		cerr << "Failed to send message" << endl;
		return -1;
	}

	for( map<long, string>::iterator it=pResponse->m_mapParameters.begin();it!=pResponse->m_mapParameters.end();++it)
	{
		cout << (*it).first << ":" << (*it).second << endl;
	}

	for( map<long, char *>::iterator it=pResponse->m_mapData_Parameters.begin();it!=pResponse->m_mapData_Parameters.end();++it)
	{
		cout << "D" << (*it).first << ":" << (*it).second << ":" << (*it).first << ".out" << endl;
		FILE *file = fopen( (StringUtils::itos((*it).first) + ".out").c_str(),"wb");
		if( file )
		{
			fwrite( (*it).second,pResponse->m_mapData_Lengths[(*it).first],1,file );
			fclose(file);
		}
	}


    Sleep(50);
#ifdef _WIN32
    WSACleanup();
#endif

    return 0;
}

