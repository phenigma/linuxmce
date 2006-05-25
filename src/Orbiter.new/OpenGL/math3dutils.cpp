//
// C++ Implementation: math3dutils
//
// Description: 
//
//
// Author: ,,, <opengl@edgar>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "math3dutils.h"

#include <math.h>

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

T3DVertex T3DVertex::Transform(T3DMatrix& Matrix)
{
	T3DVertex Result;
	Result.X = X * Matrix.Values[0 * 4 + 0] 
		 + Y * Matrix.Values[1 * 4 + 0] 
		 + Z * Matrix.Values[2 * 4 + 0] 
		 + Matrix.Values[3 * 4 + 0];	

	Result.Y = X * Matrix.Values[0 * 4 + 1] 
		 + Y * Matrix.Values[1 * 4 + 1]
		 + Z * Matrix.Values[2 * 4 + 1]
		 + Matrix.Values[3 * 4 + 1];
		 
	Result.Z = X * Matrix.Values[0 * 4 + 2]
		 + Y * Matrix.Values[1 * 4 + 2] 
		 + Z * Matrix.Values[2 * 4 + 2] 
		 + Matrix.Values[3 * 4 + 2];
		 
	return Result;
}

T3DMatrix::T3DMatrix()
{
	SetIdentity();
}

T3DMatrix::~T3DMatrix()
{
}

void T3DMatrix::SetIdentity()
{
	int i;
	for(i = 0; i<16; i++)
		Values[i] = 0.0f;
	for(i = 0; i<4; i++)
		Values[i*5] = 1.0f;
}

void T3DMatrix::SetTranslate(float ValueX, float ValueY, float ValueZ)
{
	SetIdentity();
	Values[4 * 3 + 0] = ValueX;
	Values[4 * 3 + 1] = ValueY;
	Values[4 * 3 + 2] = ValueZ;
}

void T3DMatrix::SetScale(float ValueX, float ValueY, float ValueZ)
{
	SetIdentity();
	Values[0 * 4 + 0] = ValueX;
	Values[1 * 4 + 1] = ValueY;
	Values[2 * 4 + 2] = ValueZ;
}

void T3DMatrix::SetRotateX(float Angle)
{
	//S = sinus (Angle)
	//C = cosinus(Angle);
	float S, C;
	
	S = sin(Angle);
	C = cos(Angle);
	
	Values[1 * 4 + 1] = C;
	Values[2 * 4 + 2] = C;
	Values[1 * 4 + 2] = -S;
	Values[2 * 4 + 1] = S; 
}

void T3DMatrix::SetRotateY(float Angle)
{
	//S = sinus (Angle)
	//C = cosinus(Angle);
	float S, C;
	
	S = sin(Angle);
	C = cos(Angle);
	
	Values[0 * 4 + 0] = C;
	Values[2 * 4 + 2] = C;
	Values[0 * 4 + 2] = -S;
	Values[2 * 4 + 0] = S; 
}

void T3DMatrix::SetRotateZ(float Angle)
{
	//S = sinus (Angle)
	//C = cosinus(Angle);
	float S, C;
	
	S = sin(Angle);
	C = cos(Angle);
	
	Values[0 * 4 + 0] = C;
	Values[1 * 4 + 1] = C;
	Values[0 * 4 + 1] = -S;
	Values[1 * 4 + 0] = S; 
}

T3DMatrix T3DMatrix::Multiply(T3DMatrix Matrix)
{
	int M, N, P;
	T3DMatrix Result;
	for(M = 0; M<4; M++)
		for(P = 0; P<4; P++)
		{
			Result.Values[M * 4 + P] = 0;
 			for(N = 0; N<4; N++)
				Result.Values[M * 4 + P] += 
					Values[M * 4 + N] * Matrix.Values[N * 4 + P];
		}
	return Result;
}

T3DMatrix T3DMatrix::operator = (T3DMatrix& Matrix)
{
	for(int i = 0; i<16; i++)
		Values[i] = Matrix.Values[i];
	return (*this);
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

