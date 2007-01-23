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
	delete Picture;
	delete Zoom;
	delete PictureFrame;
}

bool AnimatedPicture::LoadFromFile(string FileName)
{
	bool Result = Picture->Load(FileName);
	Picture->Prepare(m_nScreenWidth, m_nScreenHeight);

	delete PictureFrame;
	PictureFrame = BuildRectangle(0.0f, 0.0f, 1.0f, 1.0f, Picture);
	
	return Result;
}

Frame* AnimatedPicture::GetFrame()
{
	return PictureFrame;
}

void AnimatedPicture::SetZoomKind(int StartTime, int ZoomTime)
{
	delete Zoom;
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
