#include "MeshContainer.h"

#include "../math3dutils.h"

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

/*virtual*/ void MeshContainer::SetUVMapping(FloatRect Rect)
{
	for(int i = 0; i < NoVertexes; i++)
	{
		Vertexes[i].UVW.X = MathUtils::InterpolateValues(Rect.Left, Rect.Left + Rect.Width, 
			Vertexes[i].UVW.X);
		Vertexes[i].UVW.Y = MathUtils::InterpolateValues(Rect.Top, Rect.Top + Rect.Height, 
			Vertexes[i].UVW.Y);
	}
}
