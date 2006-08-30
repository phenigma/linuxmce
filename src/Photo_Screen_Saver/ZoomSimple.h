#ifndef ZoomSimple_H_
#define ZoomSimple_H_

#include "ZoomBase.h"

class ZoomSimple : public ZoomBase
{
	FloatRect* ZoomStart;
	FloatRect* ZoomEnd;
public:
	ZoomSimple(MeshFrame* PictureObject, int ScreenWidth, int ScreenHeight, int Width, int Height, int StartTime, int ZoomTime);
	~ZoomSimple(void);

	virtual bool Update(int Time);
};

#endif 
