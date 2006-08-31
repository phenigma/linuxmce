#include "ZoomSimple.h"

#include "MathUtils.h"

ZoomSimple::ZoomSimple(MeshFrame* PictureObject, int ScreenWidth, int ScreenHeight, int Width, int Height, int StartTime, int ZoomTime)
: ZoomBase(PictureObject, ScreenWidth, ScreenHeight, Width, Height, StartTime, ZoomTime)
{
	float PictureRatio = (float) this->Width/this->Height;

	ZoomAmount1 = RandomInInterval(500, 1000)/1000.0f;

	DeltaX1 = RandomInInterval(0, (int) this->Width/ZoomAmount1 - ScreenWidth)-2;
	DeltaY1 = RandomInInterval(0, (int) this->Height/ZoomAmount1 - ScreenHeight)-2;

	ZoomStart = new FloatRect(-DeltaX1, -DeltaY1,
		(float) this->Width/ZoomAmount1, (float)this->Height/ZoomAmount1);

	ZoomAmount2 = RandomInInterval(500, 1000)/1000.0f;

	DeltaX2 = RandomInInterval(0, (int) this->Width/ZoomAmount2 - ScreenWidth)-2;
	DeltaY2 = RandomInInterval(0, (int) this->Height/ZoomAmount2 - ScreenHeight)-2;
	ZoomEnd = new FloatRect(-DeltaX2, -DeltaY2, 
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

	if(ZoomValue.Left>0)
		return Result;
	if(ZoomValue.Top>0)
		return Result;
	if(ZoomValue.Left + ZoomValue.Width < ScreenWidth-2)
		return Result;
	if(ZoomValue.Top + ZoomValue.Height < ScreenHeight-2)
		return Result;
	MeshTransform Transform;
	Transform.ApplyScale(ZoomValue.Width, ZoomValue.Height, 1);
	Transform.ApplyTranslate(ZoomValue.Left, ZoomValue.Top, 0);
	//Transform.ApplyScale(ScreenWidth* (ScreenWidth/ZoomValue.Width), ScreenHeight* (ScreenHeight/ZoomValue.Height), 1);
	PictureObject->SetTransform(Transform);

	return Result;
}
