#include "Point3D.h"

void Point3D::ApplyTransform(Transform& pTransform)
{
	float X = this->X * pTransform.TransformMatrix[0 * 4 + 0] + 
		this->Y * pTransform.TransformMatrix[1 * 4 + 0] + 
		this->Z * pTransform.TransformMatrix[2 * 4 + 0] + 
		pTransform.TransformMatrix[3 * 4 + 0];
	float Y = this->X * pTransform.TransformMatrix[0 * 4 + 1] + 
		this->Y * pTransform.TransformMatrix[1 * 4 + 1] + 
		this->Z * pTransform.TransformMatrix[2 * 4 + 1] + 
		pTransform.TransformMatrix[3 * 4 + 1];
	float Z = this->X * pTransform.TransformMatrix[0 * 4 + 2] + 
		this->Y * pTransform.TransformMatrix[1 * 4 + 2] + 
		this->Z * pTransform.TransformMatrix[2 * 4 + 2] + 
		pTransform.TransformMatrix[3 * 4 + 2];
	this->X = X;
	this->Y = Y;
	this->Z = Z;
}
