#include "SDLFrontEnd.h"
#include "../../LCDLogic/LCDManager.h"
#include "../../LCDLogic/MenuLoader.h"
#include "../../LCDLogic/MenuHolder.h"
#include "../../LCDLogic/ActionProcessor.h"
#include "../../Renderers/LCDRenderer.h"

int main( int argc, char* argv[] )
{
	DCE::LoggerWrapper::GetInstance()->Write(LV_STATUS, "Usage: SDL_Emulator <menu file>\n");

	MenuLoader menu_loader;
	string sMenuFilename = "../../Docs/menu-sample.xml";

	if(argc >= 2)
		sMenuFilename = argv[1];

	MenuHolder *pMenu_Holder = menu_loader.Load(sMenuFilename);
	pMenu_Holder->Setup(new ActionProcessor(NULL));

	LCDManager manager(pMenu_Holder);
	SDLFrontEnd front_end(&manager, 400, 300);
	LCDRenderer lcd_renderer("/dev/ttyUSB0");
	
	if(front_end.Init())
	{
		manager.AddRenderer(&front_end);
		manager.AddRenderer(&lcd_renderer);
		
		return !front_end.EventLoop();
	}
	
	return 2;
}
