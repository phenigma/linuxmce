#include "ZoomSimple.h"

#include "MathUtils.h"

ZoomSimple::ZoomSimple(MeshFrame* PictureObject, int ScreenWidth, int ScreenHeight, int Width, int Height, int StartTime, int ZoomTime)
: ZoomBase(PictureObject, ScreenWidth, ScreenHeight, Width, Height, StartTime, ZoomTime)
{
	float PictureRatio = (float) this->Width/this->Height;

	float ZoomAmount = RandomInInterval(500, 1000)/1000.0f;

	int DeltaX = - RandomInInterval(0, (int) this->Width/ZoomAmount - ScreenWidth);
	int DeltaY = - RandomInInterval(0, (int) this->Height/ZoomAmount - ScreenHeight);

	ZoomStart = new FloatRect((float)DeltaX, (float)DeltaY, 
		(float) this->Width/ZoomAmount, (float)this->Height/ZoomAmount);

	ZoomAmount = RandomInInterval(500, 1000)/1000.0f;

	DeltaX = - RandomInInterval(0, (int) this->Width/ZoomAmount - ScreenWidth);
	DeltaY = - RandomInInterval(0, (int) this->Height/ZoomAmount - ScreenHeight);
	ZoomEnd = new FloatRect((float)DeltaX, (float)DeltaY, 
		(float) this->Width/ZoomAmount, (float)this->Height/ZoomAmount);

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
	Transform.ApplyScale(ZoomValue.Width, ZoomValue.Height, 0);
	Transform.ApplyTranslate(ZoomValue.Left, ZoomValue.Top, 1);
	//Transform.ApplyScale(ScreenWidth* (ScreenWidth/ZoomValue.Width), ScreenHeight* (ScreenHeight/ZoomValue.Height), 1);
	PictureObject->SetTransform(Transform);

	return Result;
}
