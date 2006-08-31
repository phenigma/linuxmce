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

	int AuxScreenWidth = RandomInInterval(ScreenWidth*3/5, ScreenWidth)+ScreenWidth/5;

	int DeltaX = RandomInInterval(0,  (ScreenWidth - Width) / 2);
	int DeltaY = RandomInInterval(0,  (ScreenWidth - Width) / 2);

	ZoomStart = new FloatRect((float)DeltaX, (float)DeltaY, 
		(float) AuxScreenWidth, (float)AuxScreenWidth/PictureRatio);

	AuxScreenWidth = RandomInInterval(ScreenWidth*3/5, ScreenWidth)+ScreenWidth/5;

	DeltaX = RandomInInterval(0,  (ScreenWidth - Width) / 2);
	DeltaY = RandomInInterval(0,  (ScreenWidth - Width) / 2);
	ZoomEnd = new FloatRect((float)DeltaX, (float)DeltaY, 
		(float) AuxScreenWidth, (float)AuxScreenWidth/PictureRatio);

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

	MeshTransform Transform;
	Transform.ApplyScale(Width, Height, 1);
	Transform.ApplyTranslate(ZoomValue.Left, ZoomValue.Top, 1);
	//Transform.ApplyScale(ScreenWidth* (ScreenWidth/ZoomValue.Width), ScreenHeight* (ScreenHeight/ZoomValue.Height), 1);
	PictureObject->SetTransform(Transform);

	return Result;
}
