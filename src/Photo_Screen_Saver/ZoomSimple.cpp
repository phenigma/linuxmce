#include "ZoomSimple.h"

#include "MathUtils.h"

ZoomSimple::ZoomSimple(MeshFrame* PictureObject, int ScreenWidth, int ScreenHeight, int Width, int Height, int StartTime, int ZoomTime)
: ZoomBase(PictureObject, ScreenWidth, ScreenHeight, Width, Height, StartTime, ZoomTime)
{
	int AuxScreenWidth = RandomInInterval(ScreenWidth*3/5, ScreenWidth);
	int AuxScreenHeight = RandomInInterval(ScreenHeight*3/5, ScreenHeight);

	int AuxScreenLeft = RandomInInterval(0, ScreenWidth - AuxScreenWidth);
	int AuxScreenTop = RandomInInterval(0, ScreenHeight - AuxScreenHeight);

	ZoomStart = new FloatRect((float)AuxScreenLeft, (float)AuxScreenTop, (float) AuxScreenWidth, (float)AuxScreenHeight);

	AuxScreenWidth = RandomInInterval(ScreenWidth*3/5, ScreenWidth);
	AuxScreenHeight = RandomInInterval(ScreenHeight*3/5, ScreenHeight);

	AuxScreenLeft = RandomInInterval(0, ScreenWidth - AuxScreenWidth);
	AuxScreenTop = RandomInInterval(0, ScreenHeight - AuxScreenHeight);
	ZoomEnd = new FloatRect((float)AuxScreenLeft, (float)AuxScreenTop, (float) AuxScreenWidth, (float)AuxScreenHeight);

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
	Transform.ApplyTranslate(-ZoomValue.Left, -ZoomValue.Top, 1);
	Transform.Scale(ScreenWidth* ScreenWidth/ZoomValue.Width, ScreenHeight* ScreenHeight/ZoomValue.Height, 1);
	PictureObject->SetTransform(Transform);

	return Result;
}
