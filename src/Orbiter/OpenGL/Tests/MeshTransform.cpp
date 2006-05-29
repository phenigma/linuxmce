#include "MeshTransform.h"

#include "GLMathUtils.h"

#include <stdlib.h>
#include <memory.h>
#include <math.h>

MeshTransform::MeshTransform(void)
{
	this->SetIdentity();
}

MeshTransform::~MeshTransform(void)
{
}

void MeshTransform::SetIdentity(void)
{
	int i;
	for(i = 0; i<16; i++)
		TransformMatrix[i] = 0.0f;
	for(i = 0; i<4; i++)
		TransformMatrix[i*5] = 1.0f;
}

void MeshTransform::Translate(float X, float Y, float Z)
{
	SetIdentity();
	TransformMatrix[3 * 4 + 0] = X;
	TransformMatrix[3 * 4 + 1] = Y;
	TransformMatrix[3 * 4 + 2] = Z;
}

void MeshTransform::ApplyTranslate(float X, float Y, float Z)
{
	MeshTransform Transform;
	Transform.Translate(X, Y, Z);
	ApplyTransform(Transform);
}

void MeshTransform::Scale(float X, float Y, float Z)
{
	SetIdentity();
	TransformMatrix[0 * 4 + 0] = X;
	TransformMatrix[1 * 4 + 1] = Y;
	TransformMatrix[2 * 4 + 2] = Z;
}

void MeshTransform::ApplyScale(float X, float Y, float Z)
{
	MeshTransform Transform;
	Transform.Scale(X, Y, Z);
	ApplyTransform(Transform);
}

void MeshTransform::RotateX(float Angle)
{
	SetIdentity();
	float RadAngle = GLMathUtils::DegreeToRadian(Angle);
	float S, C;
	S = sin(RadAngle); 
	C = cos(RadAngle);
	TransformMatrix[1 * 4 + 1] = C;
	TransformMatrix[2 * 4 + 2] = C;
	TransformMatrix[1 * 4 + 2] = -S;
	TransformMatrix[2 * 4 + 1] = S; 
}

void MeshTransform::ApplyRotateX(float Angle)
{
	MeshTransform Transform;
	Transform.RotateX(Angle);
	ApplyTransform(Transform);
}

void MeshTransform::RotateY(float Angle)
{
	SetIdentity();
	float RadAngle = GLMathUtils::DegreeToRadian(Angle);
	float S, C;
	S = sin(RadAngle); 
	C = cos(RadAngle);
	TransformMatrix[0 * 4 + 0] = C;
	TransformMatrix[2 * 4 + 2] = C;
	TransformMatrix[0 * 4 + 2] = -S;
	TransformMatrix[2 * 4 + 0] = S; 
}

void MeshTransform::ApplyRotateY(float Angle)
{
	MeshTransform Transform;
	Transform.RotateY(Angle);
	ApplyTransform(Transform);
}

void MeshTransform::RotateZ(float Angle)
{
	SetIdentity();
	float RadAngle = GLMathUtils::DegreeToRadian(Angle);
	float S, C;
	S = sin(RadAngle); 
	C = cos(RadAngle);
	TransformMatrix[0 * 4 + 0] = C;
	TransformMatrix[1 * 4 + 1] = C;
	TransformMatrix[0 * 4 + 1] = -S;
	TransformMatrix[1 * 4 + 0] = S; 
}

void MeshTransform::ApplyRotateZ(float Angle)
{
	MeshTransform Transform;
	Transform.RotateZ(Angle);
	ApplyTransform(Transform);
}

void MeshTransform::ApplyTransform(MeshTransform& Transform)
{
	int M, N, P;
	MeshTransform Final;
	for(M = 0; M < 4; M++)
	{
		for(P = 0; P < 4; P++)
		{
			Final.TransformMatrix[M * 4 + P] = 0.0f;
			for(N = 0; N < 4; N++)
			{
				Final.TransformMatrix[M * 4 + P] += 
					TransformMatrix[M * 4 + N] * Transform.TransformMatrix[N * 4 + P];
			}
		}
	}
	CopyFrom(Final);
}

void MeshTransform::CopyFrom(MeshTransform& Transform)
{
	for(int i = 0; i<16; i++)
		this->TransformMatrix[i] = Transform.TransformMatrix[i];
}
