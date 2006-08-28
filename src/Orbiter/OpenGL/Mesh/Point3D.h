#ifndef Point3D_H_
#define Point3D_H_

#include "MeshTransform.h"

class Point3D
{
public:
	float X, Y, Z;

	void ApplyTransform(MeshTransform& Transform);

	Point3D(float X = 0.0f, float Y = 0.0f, float Z = 0.0f);

};

#endif
