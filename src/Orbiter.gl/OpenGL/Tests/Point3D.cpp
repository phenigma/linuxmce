#include "Point3D.h"

void Point3D::ApplyTransform(MeshTransform& Transform)
{
	float X = this->X * Transform.TransformMatrix[0 * 4 + 0] + 
		this->Y * Transform.TransformMatrix[1 * 4 + 0] + 
		this->Z * Transform.TransformMatrix[2 * 4 + 0] + 
		Transform.TransformMatrix[3 * 4 + 0];
	float Y = this->X * Transform.TransformMatrix[0 * 4 + 1] + 
		this->Y * Transform.TransformMatrix[1 * 4 + 1] + 
		this->Z * Transform.TransformMatrix[2 * 4 + 1] + 
		Transform.TransformMatrix[3 * 4 + 1];
	float Z = this->X * Transform.TransformMatrix[0 * 4 + 2] + 
		this->Y * Transform.TransformMatrix[1 * 4 + 2] + 
		this->Z * Transform.TransformMatrix[2 * 4 + 2] + 
		Transform.TransformMatrix[3 * 4 + 2];
	this->X = X;
	this->Y = Y;
	this->Z = Z;
}
