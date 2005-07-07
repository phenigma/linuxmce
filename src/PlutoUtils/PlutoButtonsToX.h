#ifndef PlutoButtonsToX_h
#define PlutoButtonsToX_h

#include "pluto_main/Define_Button.h"

int PlutoButtonsToX(int PK_Button)
{
	switch( PK_Button )
	{
	case BUTTON_a_CONST:
		return XK_A;
	case BUTTON_1_CONST:
		return XK_1;
	}
	return 0;
}

#endif
