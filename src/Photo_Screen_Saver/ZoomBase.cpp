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

	if(ScreenWidth < Width)
	{
		this->Height = Height*(float)ScreenWidth/Width + 1;
		this->Width = ScreenWidth;
	}
	if(ScreenHeight < Height)
	{
		this->Width = Width*(float)ScreenWidth/Width + 1;
		this->Height = ScreenHeight;
	}

	if(ScreenWidth > Width)
	{
		this->Height = Height*(float)ScreenWidth/Width + 1;
		this->Width = ScreenWidth;
	}
	if(ScreenHeight > Height)
	{
		this->Width = Width*(float)ScreenWidth/Width + 1;
		this->Height = ScreenHeight;
	}
}

ZoomBase::~ZoomBase(void)
{

}
