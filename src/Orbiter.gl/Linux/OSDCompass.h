#ifndef __OSD_COMPASS_H__
#define __OSD_COMPASS_H__

#include "../Orbiter.h"

//forward declarations
class PlutoRectangle;

class OSDCompass
{
	Orbiter* pOrbiter;
	PlutoRectangle Area;
	int OldMouseX, OldMouseY;
	unsigned long ulAbsoluteTimeInMs;
	bool FirstUpdate;

	void PaintTriangle(int x1, int y1, int x2, int y2, int x3, int y3);
	void PaintUpperTriangle(int x1, int y1, int x2, int x3, int y2);
	void PaintLowerTriangle(int x1, int y1, int x2, int x3, int y2);

	float ComputeAngle(int DeltaX, int DeltaY);
public:
	OSDCompass(Orbiter* pOrbiter, PlutoRectangle Area);
	~OSDCompass(void);

	void Update(int NextMouseX, int NextMouseY, unsigned long ulAbsoluteTimeInMs);

	
};

#endif
