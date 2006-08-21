#include "GLMathUtils.h"

const float GLMathUtils::_PI = 3.1414926535f;

GLMathUtils::GLMathUtils(void)
{
}

GLMathUtils::~GLMathUtils(void)
{
}

float GLMathUtils::RadianToDegree(float Angle)
{
	return Angle/_PI*180.0f;
}

float GLMathUtils::DegreeToRadian(float Angle)
{
	return Angle*_PI/180.0f;
}

bool GLMathUtils::IsPowerOf2(int Value)
{
	if(Value<1)
		return false;
	while(Value % 2 != 1)
		Value /= 2;
	return Value == 1;
}

int GLMathUtils::MinPowerOf2(int Value)
{
	int Result = 1;
	while(Result<Value)
		Result*= 2;
	return Result;	
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

/**
	* Copy onstructor
	*/
FloatRect::FloatRect(PlutoRectangle Rectangle)
	: Top((float)Rectangle.Y),
		Left((float)Rectangle.X),  
		Width((float)Rectangle.Width),
		Height((float)Rectangle.Height) 
{
}

FloatRect FloatRect::Interpolate(FloatRect DestRect, float Value)
{
	FloatRect Result;
	
	Result.Left = MathUtils::InterpolateValues(Left, DestRect.Left, Value);
	Result.Top = MathUtils::InterpolateValues(Top, DestRect.Top, Value);
	Result.Width = MathUtils::InterpolateValues(Width, DestRect.Width, Value);
	Result.Height = MathUtils::InterpolateValues(Height, DestRect.Height, Value);
	
	return Result;
}


void T3DVertex::Translate(float ValueX, float ValueY, float ValueZ)
{
    X += ValueX;
    Y += ValueY;
    Z += ValueZ;
}

void T3DVertex::Scale(float ValueX, float ValueY, float ValueZ)
{
    X *= ValueX;
    Y *= ValueY;
    Z *= ValueZ;
}

const float MathUtils::_PI = 3.1414926535f;

int MathUtils::MinPowerOf2(int Value)
{
	int Result = 1;
	while(Result<Value)
		Result*= 2;
	return Result;
}

float MathUtils::InterpolateValues(float Source, float Dest, float Theta)
{
	return Source*(1-Theta)+Dest*Theta;
}

float MathUtils::InterpolateSinus(float Source, float Dest, float Theta)
{
	Theta = sin(Theta);
	return Source*(1-Theta)+Dest*Theta;
}

// Adds 2 Points. Don't Just Use '+' ;)
POINT_3D MathUtils::pointAdd(POINT_3D p, POINT_3D q) {
	p.x += q.x;		p.y += q.y;		p.z += q.z;
	return p;
}

// Multiplies A Point And A Constant. Don't Just Use '*'
POINT_3D MathUtils::pointTimes(float c, POINT_3D p) {
	p.x *= c;	p.y *= c;	p.z *= c;
	return p;
}

// Function For Quick Point Creation
POINT_3D MathUtils::makePoint(float a, float b, float c) {
	POINT_3D p;
	p.x = a;	p.y = b;	p.z = c;
	return p;
}

// Calculates 3rd Degree Polynomial Based On Array Of 4 Points
// And A Single Variable (u) Which Is Generally Between 0 And 1
POINT_3D MathUtils::Bernstein(float u, POINT_3D *p) {
	POINT_3D	a, b, c, d, r;

	a = pointTimes(pow(u,3), p[0]);
	b = pointTimes(3*pow(u,2)*(1-u), p[1]);
	c = pointTimes(3*u*pow((1-u),2), p[2]);
	d = pointTimes(pow((1-u),3), p[3]);

	r = pointAdd(pointAdd(a, b), pointAdd(c, d));

	return r;
}


