#ifndef MeshContainer_H_
#define MeshContainer_H_

#include "MeshVertex.h"
#include "MeshTriangle.h"
#include "../OpenGLGraphic.h"

class MeshContainer
{
public:
	MeshVertex* Vertexes;
	int NoVertexes;
	MeshTriangle* Triangles;
	int NoTriangles;
	bool Blended_;

	MeshContainer();
	virtual ~MeshContainer();
	
	virtual void SetAlpha(float Alpha);
	virtual void SetTexture(OpenGLGraphic* Texture);
};

#endif 
