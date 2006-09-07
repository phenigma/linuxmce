#ifndef Container_H_
#define Container_H_

#include "Vertex.h"
#include "Triangle.h"
#include "GraphicImage.h"

class Container
{
public:
	Vertex* Vertexes;
	int NoVertexes;
	Triangle* Triangles;
	int NoTriangles;
	bool Blended_;

	Container();
	virtual ~Container();
	
	virtual void SetAlpha(float Alpha);
	virtual void SetTexture(GraphicImage* Texture);
	void SetColor(Point3D& Color);

	Container* Clone();
};

#endif 
