#include "AnimatedPicture.h"

#include "ZoomFactory.h"
#include "Builder.h"

AnimatedPicture::AnimatedPicture(int ScreenWidth, int ScreenHeight) :
	Picture(NULL), PictureFrame(NULL), Zoom(NULL)
{
	this->Picture = new GraphicImage();

	m_nScreenWidth = ScreenWidth;
	m_nScreenHeight = ScreenHeight;
}

AnimatedPicture::~AnimatedPicture(void)
{
}

bool AnimatedPicture::LoadFromFile(string FileName)
{
	bool Result = Picture->Load(FileName);
	Sleep(200);

	Picture->Prepare(m_nScreenWidth, m_nScreenHeight);
	Sleep(200);

	PictureFrame = BuildRectangle(0.0f, 0.0f, 1.0f, 1.0f, Picture);
	
	return Result;
}

Frame* AnimatedPicture::GetFrame()
{
	return PictureFrame;
}

void AnimatedPicture::SetZoomKind(int StartTime, int ZoomTime)
{
	Zoom = ZoomFactory::Instance()->CreateZoomType(rand() % 2, PictureFrame, 
		int(Picture->Width * Picture->MaxU), int(Picture->Height * Picture->MaxV), 
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
