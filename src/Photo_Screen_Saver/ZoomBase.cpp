#include "ZoomBase.h"

ZoomBase::ZoomBase(Frame* PictureObject, 
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

	if(ScreenWidth < Width)
	{
		this->Height = int(Height*(float)ScreenWidth/Width + 1);
		this->Width = ScreenWidth;
	}
	if(ScreenHeight < Height)
	{
		this->Width = int(Width*(float)ScreenWidth/Width + 1);
		this->Height = ScreenHeight;
	}

	if(ScreenWidth > Width)
	{
		this->Height = int(Height*(float)ScreenWidth/Width + 1);
		this->Width = ScreenWidth;
	}
	if(ScreenHeight > Height)
	{
		this->Width = int(Width*(float)ScreenWidth/Width + 1);
		this->Height = ScreenHeight;
	}
}

ZoomBase::~ZoomBase(void)
{

}
