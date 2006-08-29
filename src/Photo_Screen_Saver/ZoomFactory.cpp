#include "ZoomFactory.h"

#include "ZoomSimple.h"

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
	return new ZoomSimple(PictureObject, ScreenWidth, ScreenHeight, Width, Height, StartTime, ZoomTime);
}
