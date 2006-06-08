#include "MeshContainer.h"

MeshContainer::MeshContainer(void)
{
}

MeshContainer::~MeshContainer(void)
{
}

void MeshContainer::SetAlpha(float Alpha)
{
	for(int i = 0; i < NoVertexes; i++)
		Vertexes[i].UVW.Z = Alpha;
}

void MeshContainer::SetTexture(OpenGLTexture Texture)
{
	for(int i = 0; i < NoVertexes; i++)
		Triangles[i].Texture = 	Texture;
}
