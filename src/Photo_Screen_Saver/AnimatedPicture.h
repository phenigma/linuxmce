#ifndef AnimatedPicture_H_
#define AnimatedPicture_H_

#include "Frame.h"
#include "GraphicImage.h"
#include "ZoomBase.h"

#include <string>

using namespace std;

class AnimatedPicture
{
	GraphicImage* Picture;
	Frame* PictureFrame;
	ZoomBase* Zoom;
	int m_nScreenWidth, m_nScreenHeight;

public:
	AnimatedPicture(int ScreenWidth, int ScreenHeight);

	bool LoadFromFile(string FileName);
	Frame* GetFrame();
	void SetZoomKind(int StartTime, int ZoomTime);

	void Paint();

	bool Update(int Time);

	~AnimatedPicture();
};

#endif
