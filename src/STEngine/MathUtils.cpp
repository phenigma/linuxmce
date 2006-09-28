#include "MathUtils.h"

#include "stdlib.h"

const float _PI = 3.1414926535f;

int MinPowerOf2(int Value)
{
	int Result = 1;
	while(Result<Value)
		Result*= 2;
	return Result;	
}

float DegreeToRadian(float Angle)
{
	return Angle*_PI/180.0f;
}


FloatRect::FloatRect()
: Top(0),
Left(0),
Width(1),
Height(1)
{
}

/**
* Constructor with setup the variables
*/
FloatRect::FloatRect(float TopValue, float LeftValue, float WidthValue, float HeightValue) 
: Top(TopValue),
Left(LeftValue),
Width(WidthValue),
Height(HeightValue) 
{
}

FloatRect FloatRect::Interpolate(FloatRect DestRect, float Value)
{
	FloatRect Result;

	Result.Left = InterpolateValues(Left, DestRect.Left, Value);
	Result.Top = InterpolateValues(Top, DestRect.Top, Value);
	Result.Width = InterpolateValues(Width, DestRect.Width, Value);
	Result.Height = InterpolateValues(Height, DestRect.Height, Value);

	return Result;
}

float InterpolateValues(float Source, float Dest, float Theta)
{
	return Source*(1-Theta)+Dest*Theta;
}

int RandomInInterval(int BeginValue, int EndValue)
{
	if(EndValue==BeginValue)
		return 0;
	if(EndValue>BeginValue)
		return BeginValue + rand() % (EndValue - BeginValue);
	else
		return EndValue + rand() % (BeginValue - EndValue);
}
