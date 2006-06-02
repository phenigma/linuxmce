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

bool GLMathUtils::IsPowOf2(int Value)
{
	if(Value<1)
		return false;
	while(Value % 2 != 1)
		Value /= 2;
	return Value == 1;
}
