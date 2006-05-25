#ifndef __OSD_COMPASS_H__
#define __OSD_COMPASS_H__

#include "../Orbiter.h"

//forward declarations
class PlutoRectangle;

class OSDCompass
{
public:
	OSDCompass(PlutoRectangle Area);
	~OSDCompass(void);

	void Update(int NextMouseX, int NextMouseY, unsigned long ulAbsoluteTimeInMs);
};

#endif
