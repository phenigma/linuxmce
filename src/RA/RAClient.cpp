#include <iostream>
#include "PlutoUtils/CommonIncludes.h"	
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "DCE/Logger.h"
#include "Message.h"
#include "PlutoUtils/MyStl.h"
#include "RA/RA_Request.h"
#include "RA/RA_Processor.h"


#ifdef WIN32
long _tmain(long argc, _TCHAR* argv[])
{
	long err;
	WSADATA wsaData;
	err = WSAStartup(MAKEWORD( 1, 1 ),(LPWSADATA)  &wsaData);
#else
long main(long argc, char *argv[])
{
#endif
	string ServerAddress="localhost:3460";

	string s, Cmd, page;
	string::size_type pos = 0;

	RA_Processor rp(ClientID,SoftwareVersion);

	class VR_RequestPayment *pRA_RequestPayment = new VR_RequestPayment(&pConfig,1250,"938-82","Misc Videos","Mrs. John Doe",14,12,pRST);
	rp.AddRequest(pRA_RequestPayment);
	while( rp.SendRequests(ServerAddress) );

	delete Logger;

    Sleep(50);	
#ifdef _WIN32
	WSACleanup();
#endif

	return 0;
}

