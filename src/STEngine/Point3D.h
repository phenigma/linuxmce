#ifndef Point3D_H_
#define Point3D_H_

#include "Transform.h"

class Point3D
{
public:
	float X, Y, Z;

	void ApplyTransform(Transform& pTransform);

};

#endif
