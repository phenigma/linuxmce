#ifndef MeshVertex_H_
#define MeshVertex_H_

#include "Point3D.h"

class MeshVertex : public Point3D
{
public:
	Point3D UVW;
	Point3D Normal;
	Point3D Color;
	
};

#endif
