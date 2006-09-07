#ifndef ZoomPanUp_H_
#define ZoomPanUp_H_

#include "ZoomBase.h"

class ZoomPanUp : public ZoomBase
{
	FloatRect* ZoomStart;
	FloatRect* ZoomEnd;
public:
	ZoomPanUp(Frame* PictureObject, int ScreenWidth, int ScreenHeight, int Width, int Height, int StartTime, int ZoomTime);
	~ZoomPanUp(void);

	virtual bool Update(int Time);
};

#endif 
