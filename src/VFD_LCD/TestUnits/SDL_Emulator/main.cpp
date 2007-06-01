#include "SDLFrontEnd.h"
#include "../../LCDLogic/LCDManager.h"

int main( int argc, char* argv[] )
{
	LCDManager manager(NULL, NULL, NULL);
	SDLFrontEnd front_end(&manager, 400, 300);
	
	if(front_end.Init())
	{
		manager.Renderer(&front_end);
		return !front_end.EventLoop();
	}
	
	return 2;
}
