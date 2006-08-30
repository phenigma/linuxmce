#include "ZoomSimple.h"

#include "MathUtils.h"

ZoomSimple::ZoomSimple(MeshFrame* PictureObject, int ScreenWidth, int ScreenHeight, int Width, int Height, int StartTime, int ZoomTime)
: ZoomBase(PictureObject, ScreenWidth, ScreenHeight, Width, Height, StartTime, ZoomTime)
{

	int FirstPanX = RandomInInterval(0, Width*2/5);
	int FirstPanY = RandomInInterval(0, Height*2/5);
	float VideoRatio = (float) ScreenWidth/ScreenHeight;
	float PictureRatio = (float) Width/Height;
	float AspectRatio = VideoRatio / PictureRatio;

	int AuxScreenWidth = RandomInInterval(Width*1/5, Width);
	int AuxScreenHeight = RandomInInterval(Width*1/5, Width);

	int DeltaX = RandomInInterval(0,  (ScreenWidth - Width) / 2);
	int DeltaY = RandomInInterval(0,  (ScreenWidth - Width) / 2);

	ZoomStart = new FloatRect((float)DeltaX, (float)DeltaX/PictureRatio, 
		(float) AuxScreenWidth, (float)AuxScreenWidth/PictureRatio);

	ZoomEnd = new FloatRect((float)DeltaY, (float)DeltaY/PictureRatio, 
		(float) AuxScreenHeight, (float)AuxScreenHeight/PictureRatio);

}

ZoomSimple::~ZoomSimple(void)
{
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

	MeshTransform Transform;
	Transform.ApplyScale(Width, Height, 1);
	Transform.ApplyTranslate(ZoomValue.Left, ZoomValue.Top, 1);
	//Transform.ApplyScale(ScreenWidth* (ScreenWidth/ZoomValue.Width), ScreenHeight* (ScreenHeight/ZoomValue.Height), 1);
	PictureObject->SetTransform(Transform);

	return Result;
}
