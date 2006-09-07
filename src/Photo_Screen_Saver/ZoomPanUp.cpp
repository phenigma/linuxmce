#include "ZoomPanUp.h"

#include "MathUtils.h"

ZoomPanUp::ZoomPanUp(Frame* PictureObject, int ScreenWidth, int ScreenHeight, int Width, int Height, int StartTime, int ZoomTime)
: ZoomBase(PictureObject, ScreenWidth, ScreenHeight, Width, Height, StartTime, ZoomTime)
{
	int FirstPanX = RandomInInterval(0, Width*2/5)-Width/5;
	int FirstPanY = RandomInInterval(0, Height*2/5)-Height/5;
	float VideoRatio = (float) ScreenWidth/ScreenHeight;
	float PictureRatio = (float) Width/Height;
	float AspectRatio = VideoRatio / PictureRatio;

	int AuxScreenWidth = RandomInInterval(Width*1/5, Width);
	int AuxScreenHeight = RandomInInterval(Width*1/5, Width);

	int DeltaX = RandomInInterval(0,  (ScreenWidth - Width) / 2);
	int DeltaY = RandomInInterval(0,  (ScreenWidth - Width) / 2);

	if(ScreenWidth < Width)
		ScreenWidth = Width;

	ZoomStart = new FloatRect(0, 0, 
		(float) ScreenWidth, (float)ScreenWidth/PictureRatio);

	ZoomEnd = new FloatRect((float)DeltaY+ FirstPanX, (float)DeltaY/PictureRatio + FirstPanY, 
		(float) ScreenWidth, (float)ScreenWidth/PictureRatio);
}

ZoomPanUp::~ZoomPanUp(void)
{
	delete ZoomStart;
	delete ZoomEnd;
}

bool ZoomPanUp::Update(int Time)
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

	Transform pTransform;
	pTransform.ApplyScale(ZoomValue.Width, ZoomValue.Height, 1);
	pTransform.ApplyTranslate(0, ZoomValue.Top, 0);
	//Transform.ApplyScale(ScreenWidth* (ScreenWidth/ZoomValue.Width), ScreenHeight* (ScreenHeight/ZoomValue.Height), 1);
	PictureObject->SetTransform(pTransform);

	return Result;
}
