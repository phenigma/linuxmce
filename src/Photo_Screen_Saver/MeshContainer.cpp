#include "MeshContainer.h"

#include "MathUtils.h"

#include <map>

MeshContainer::MeshContainer(void) : Blended_(false), NoVertexes(0), NoTriangles(0),
	Vertexes(NULL), Triangles(NULL)
{
}

MeshContainer::~MeshContainer(void)
{
	delete [] Vertexes;
	delete [] Triangles;
}

void MeshContainer::SetAlpha(float Alpha)
{
	for(int i = 0; i < NoVertexes; i++)
		Vertexes[i].Alpha = Alpha;
}

void MeshContainer::SetTexture(GraphicImage* Texture)
{
	for(int i = 0; i < NoTriangles; i++)
		Triangles[i].Texture = 	Texture;
}

void MeshContainer::SetColor(Point3D& Color)
{
	for(int i = 0; i < NoVertexes; i++)
	{
		Vertexes[i].Color = Color;
	}

}
