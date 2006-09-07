#ifndef ZoomFactory_H_
#define ZoomFactory_H_

#include "ZoomBase.h"

class ZoomFactory
{
	ZoomFactory(void);
	static ZoomFactory* Instance_;
	int ScreenWidth, ScreenHeight;
public:
	static ZoomFactory* Instance();
	~ZoomFactory(void);

	void Setup(int ScreenWidth, int ScreenHeight);

	ZoomBase* CreateZoomType(int ZoomType, Frame* PictureObject, int Width, int Height, int StartTime, int ZoomTime);
};

#endif
