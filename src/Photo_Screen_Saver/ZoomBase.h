#ifndef ZoomBase_H_
#define ZoomBase_H_

#include "MeshFrame.h"

#include "MathUtils.h"

class ZoomBase
{
protected:
	int StartTime;
	int TimeToZoom;
	MeshFrame* PictureObject;
	int ScreenWidth, ScreenHeight, Width, Height;
public:
	ZoomBase(MeshFrame* PictureObject, int ScreenWidth, int ScreenHeight, int Width, int Height, int StartTime, int ZoomTime);
	virtual ~ZoomBase(void);

	virtual bool Update(int Time) = 0;


};

#endif
