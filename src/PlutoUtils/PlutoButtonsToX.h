#ifndef PlutoButtonsToX
#define PlutoButtonsToX

#include "pluto_main/Define_Buttons.h"

int PlutoButtonsToX(int PK_Button)
{
	switch( PK_Button )
	{
	case BUTTON_a_CONST:
		return XK_A;
	}
	return 0;
}

#endif
