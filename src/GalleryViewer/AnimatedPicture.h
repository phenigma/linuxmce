/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
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
