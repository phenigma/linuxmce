#ifndef Vertex_H_
#define Vertex_H_

#include "Point3D.h"

class Vertex : public Point3D
{
public:
	Point3D UVW;
	Point3D Normal;
	Point3D Color;
	float Alpha;	
};

#endif
