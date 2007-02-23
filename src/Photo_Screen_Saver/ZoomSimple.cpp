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
#include "ZoomSimple.h"

#include "MathUtils.h"

ZoomSimple::ZoomSimple(Frame* PictureObject, int ScreenWidth, int ScreenHeight, int Width, int Height, int StartTime, int ZoomTime)
: ZoomBase(PictureObject, ScreenWidth, ScreenHeight, Width, Height, StartTime, ZoomTime)
{
	float PictureRatio = (float) this->Width/this->Height;

	ZoomAmount1 = RandomInInterval(500, 1000)/1000.0f;

	DeltaX1 = RandomInInterval(0, int(this->Width/ZoomAmount1 - ScreenWidth))/2;
	DeltaY1 = RandomInInterval(0, int(this->Height/ZoomAmount1 - ScreenHeight))/2;

	ZoomStart = new FloatRect(float(-DeltaX1), float(-DeltaY1),
		(float) this->Width/ZoomAmount1, (float)this->Height/ZoomAmount1);

	ZoomAmount2 = RandomInInterval(500, 1000)/1000.0f;

	DeltaX2 = RandomInInterval(0, int(this->Width/ZoomAmount2 - ScreenWidth))/2;
	DeltaY2 = RandomInInterval(0, int(this->Height/ZoomAmount2 - ScreenHeight))/2;
	ZoomEnd = new FloatRect(float(-DeltaX2), float(-DeltaY2), 
		(float) this->Width/ZoomAmount2, (float)this->Height/ZoomAmount2);

}

ZoomSimple::~ZoomSimple(void)
{
	delete ZoomStart;
	delete ZoomEnd;
}

bool ZoomSimple::Update(int Time)
{
	bool Result = false;
	int ProgressTime = Time - StartTime;
	float Progress = (float) ProgressTime / TimeToZoom;
	if (Progress > 1.0f)
	{
		Result = true;
		Progress = 1.0f;
	}

	FloatRect ZoomValue = ZoomStart->Interpolate(*ZoomEnd, Progress);
/*
	if(ZoomValue.Left>0)
		ZoomValue.Left = 0;
	if(ZoomValue.Top>0)
		ZoomValue.Top = 0;
	if(ZoomValue.Left + ZoomValue.Width < ScreenWidth)
		ZoomValue.Left = ScreenWidth - ZoomValue.Width;
	if(ZoomValue.Top + ZoomValue.Height < ScreenHeight)
		ZoomValue.Top = ScreenHeight - ZoomValue.Height;
*/

	Transform pTransform;
	pTransform.ApplyScale(ZoomValue.Width, ZoomValue.Height, 1);
	pTransform.ApplyTranslate(ZoomValue.Left, ZoomValue.Top, 0);
	//Transform.ApplyScale(ScreenWidth* (ScreenWidth/ZoomValue.Width), ScreenHeight* (ScreenHeight/ZoomValue.Height), 1);
	PictureObject->SetTransform(pTransform);

	return Result;
}
