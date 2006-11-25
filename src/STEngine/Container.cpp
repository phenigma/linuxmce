#include "Container.h"

#include "MathUtils.h"

#include <map>

Container::Container(void) : Blended_(false)
{
    NoVertexes = 0;
    Vertexes = NULL;
    NoTriangles = 0;
    Triangles = NULL;
}

Container::~Container(void)
{
	delete [] Vertexes;
	delete [] Triangles;
}

void Container::SetAlpha(float Alpha)
{
	for(int i = 0; i < NoVertexes; i++)
		Vertexes[i].Alpha = Alpha;
}

void Container::SetTexture(GraphicImage* Texture)
{
	for(int i = 0; i < NoTriangles; i++)
		Triangles[i].Texture = 	Texture;
}

void Container::SetColor(Point3D& Color)
{
	for(int i = 0; i < NoVertexes; i++)
	{
		Vertexes[i].Color = Color;
	}

}
