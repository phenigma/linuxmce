#include "SDLFrontEnd.h"
//--------------------------------------------------------------------------------------------------------
#include "../../LCDLogic/LCDManager.h"
#include "../../LCDLogic/MenuLoader.h"
#include "../../LCDLogic/MenuHolder.h"
#include "../../LCDLogic/ActionProcessor.h"
//--------------------------------------------------------------------------------------------------------
#include "../../Renderers/LCDRenderer.h"
//--------------------------------------------------------------------------------------------------------
#include "../../InputProviders/SocketStatusInputProvider.h"
//--------------------------------------------------------------------------------------------------------
int main( int argc, char* argv[] )
{
	MenuLoader menu_loader;
	string sMenuFilename;

	if(argc < 2)
	{
		DCE::LoggerWrapper::GetInstance()->Write(LV_STATUS, "Usage: SDL_Emulator <menu file>\n");
		return 2;
	}

	sMenuFilename = argv[1];

	MenuHolder *pMenu_Holder = menu_loader.Load(sMenuFilename);
	if(NULL == pMenu_Holder)
	{
		DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Cannot parse %s", sMenuFilename.c_str());
		return 4;
	}

	pMenu_Holder->Setup(new ActionProcessor(NULL));

	LCDManager manager(pMenu_Holder);
	SDLFrontEnd front_end(&manager, 400, 300);
	//LCDRenderer lcd_renderer("/dev/ttyUSB0");

#ifdef WIN32
	WORD    wVersion;
    WSADATA wsaData;
    wVersion = MAKEWORD( 1, 1 );
	if (WSAStartup(wVersion, &wsaData)!=0)
	{
		int ec = WSAGetLastError();
		char s[91];
		sprintf(s, "WSAStartup err %d", ec);
		cerr << s << endl;
		return 3;
	}
#endif

	SocketStatusInputProvider socket_status_provider(&manager, 10000);

	int nReturnCode = 2;

	if(front_end.Init())
	{
		manager.AddRenderer(&front_end);
		//manager.AddRenderer(&lcd_renderer);
		
		nReturnCode = !front_end.EventLoop();
	}

#ifdef WIN32
    WSACleanup();
#endif

	return nReturnCode;
}
//--------------------------------------------------------------------------------------------------------
