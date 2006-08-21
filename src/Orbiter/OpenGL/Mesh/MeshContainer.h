#ifndef MeshContainer_H_
#define MeshContainer_H_

#include "MeshVertex.h"
#include "MeshTriangle.h"
#include "../OpenGLGraphic.h"

#include "../GLMathUtils.h"

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
	void SetColor(Point3D& Color);
	virtual void SetUVMapping(FloatRect Rect);

	MeshContainer* Clone();
};

#endif 
