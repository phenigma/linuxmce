#include "AnimatedPicture.h"

#include "ZoomFactory.h"
#include "MeshBuilder.h"

AnimatedPicture::AnimatedPicture(int ScreenWidth, int ScreenHeight)
{
	this->Picture = new GraphicImage();
}

AnimatedPicture::~AnimatedPicture(void)
{
}

bool AnimatedPicture::LoadFromFile(string FileName)
{
	bool Result = Picture->Load(FileName);
	PictureFrame = BuildRectangle(0.0f, 0.0f, 1.0f, 1.0f, Picture);
	return Result;
}

MeshFrame* AnimatedPicture::GetMeshFrame()
{
	return PictureFrame;
}

void AnimatedPicture::SetZoomKind(int StartTime, int ZoomTime)
{
	Zoom = ZoomFactory::Instance()->CreateZoomType(0, PictureFrame, 
		(int)Picture->Width*Picture->MaxU, (int)Picture->Height*Picture->MaxV, 
		StartTime, ZoomTime);
}

bool AnimatedPicture::Update(int Time)
{
	bool Result = Zoom->Update(Time);

	return Result;
}

void AnimatedPicture::Paint()
{
	PictureFrame->Paint();
}
