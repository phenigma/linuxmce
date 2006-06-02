#ifndef MeshContainer_H_
#define MeshContainer_H_

#include "MeshVertex.h"
#include "MeshTriangle.h"

class MeshContainer
{
public:
	MeshVertex* Vertexes;
	int NoVertexes;
	MeshTriangle* Triangles;
	int NoTriangles;


	MeshContainer();
	virtual ~MeshContainer();
	
	virtual void SetAlpha(float Alpha);
	virtual void SetTexture(OpenGLTexture Texture);
};

#endif 
