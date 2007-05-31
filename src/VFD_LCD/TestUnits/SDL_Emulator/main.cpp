#include "SDLFrontEnd.h"

int main( int argc, char* argv[] )
{
	SDLFrontEnd front_end(400, 300);
	
	if(front_end.Init())
		return !front_end.EventLoop();
	
	return 2;
}
