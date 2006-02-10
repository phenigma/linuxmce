#include "PocketFrogWrapper.h"
	
/*virtual*/ void PlutoGame::ButtonDown(int button)
{
	PocketFrogButtonDown(button);
}

/*virtual*/ void PlutoGame::ButtonUp(int button)
{
	PocketFrogButtonUp(button);
}