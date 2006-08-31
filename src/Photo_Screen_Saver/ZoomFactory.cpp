#include "ZoomFactory.h"

#include "ZoomSimple.h"
#include "ZoomPanUp.h"

ZoomFactory* ZoomFactory::Instance_ = NULL;

ZoomFactory* ZoomFactory::Instance(void)
{
	if(NULL == Instance_)
		Instance_ = new ZoomFactory();
	return Instance_;
}

ZoomFactory::ZoomFactory(void)
{
}



ZoomFactory::~ZoomFactory(void)
{
}

void ZoomFactory::Setup(int ScreenWidth, int ScreenHeight)
{
	this->ScreenWidth = ScreenWidth;
	this->ScreenHeight = ScreenHeight;
}

ZoomBase* ZoomFactory::CreateZoomType(int ZoomType, MeshFrame* PictureObject, int Width, int Height, 
									  int StartTime, int ZoomTime)
{
	ZoomType = 0;
	switch(ZoomType) {
	case 0:
		return new ZoomSimple(PictureObject, ScreenWidth, ScreenHeight, Width, Height, StartTime, ZoomTime);
		break;
	case 1:
		return new ZoomPanUp(PictureObject, ScreenWidth, ScreenHeight, Width, Height, StartTime, ZoomTime);
		break;
	default:
		return new ZoomSimple(PictureObject, ScreenWidth, ScreenHeight, Width, Height, StartTime, ZoomTime);
	}
	
}
