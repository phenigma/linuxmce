#include "MeshContainer.h"

MeshContainer::MeshContainer(void) : Blended_(false), NoVertexes(0), NoTriangles(0),
	Vertexes(NULL), Triangles(NULL)
{
}

MeshContainer::~MeshContainer(void)
{
	delete Vertexes;
	delete Triangles;
}

void MeshContainer::SetAlpha(float Alpha)
{
	for(int i = 0; i < NoVertexes; i++)
		Vertexes[i].Alpha = Alpha;
}

void MeshContainer::SetTexture(OpenGLGraphic* Texture)
{
	for(int i = 0; i < NoTriangles; i++)
		Triangles[i].Texture = 	Texture;
}
