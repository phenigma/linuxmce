#include "SDLFrontEnd.h"
#include "../../LCDLogic/LCDManager.h"
#include "../../Renderers/LCDRenderer.h"

int main( int argc, char* argv[] )
{
	LCDManager manager(NULL, NULL);
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
