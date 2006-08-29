#include "ZoomBase.h"

ZoomBase::ZoomBase(MeshFrame* PictureObject, 
				   int ScreenWidth, int ScreenHeight, int Width, int Height, 
				   int StartTime, int ZoomTime)
{
	this->PictureObject = PictureObject;
	this->StartTime = StartTime;
	this->TimeToZoom = ZoomTime;

	this->ScreenWidth = ScreenWidth;
	this->ScreenHeight = ScreenHeight;
	this->Width = Width;
	this->Height = Height;
}

ZoomBase::~ZoomBase(void)
{

}
