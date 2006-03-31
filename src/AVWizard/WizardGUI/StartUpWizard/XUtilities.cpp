#include "XUtilities.h"

#include <stdlib.h>
#include <stdio.h>

int SetXDisplayMode(int Width, int Height, int Refresh)
{
#ifndef WIN32
	char command[1024];
	snprintf(command, 1024, "xrandr -d :0 -s %dx%d -r %d", Width, Height, Refresh);
	system(command);
#endif
	return 0;
}
