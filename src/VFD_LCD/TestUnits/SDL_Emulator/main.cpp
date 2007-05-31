#include "SDLFrontEnd.h"
#include "../../LCDLogic/LCDManager.h"

int main( int argc, char* argv[] )
{
	SDLFrontEnd front_end(400, 300);
	
	if(front_end.Init())
	{
		LCDManager manager(NULL, &front_end, NULL);
		return !front_end.EventLoop();
	}
	
	return 2;
}
