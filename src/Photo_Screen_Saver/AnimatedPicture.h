#pragma once

#include "MeshFrame.h"
#include "GraphicImage.h"
#include "ZoomBase.h"

#include <string>

using namespace std;

class AnimatedPicture
{
	GraphicImage* Picture;
	MeshFrame* PictureFrame;
	ZoomBase* Zoom;
	int ScreenWidth;
	int ScreenHeight;
public:
	AnimatedPicture(int ScreenWidth, int ScreenHeight);

	bool LoadFromFile(string FileName);
	MeshFrame* GetMeshFrame();
	void SetZoomKind(int StartTime, int ZoomTime);

	void Paint();

	bool Update(int Time);

	~AnimatedPicture();
};
