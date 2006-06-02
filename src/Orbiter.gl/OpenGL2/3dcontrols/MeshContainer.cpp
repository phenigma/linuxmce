#include "MeshContainer.h"

MeshContainer::MeshContainer(void)
{
}

MeshContainer::~MeshContainer(void)
{
}

void MeshContainer::SetTexture(OpenGLTexture Texture)
{
	for(int i = 0; i<NoTriangles; i++)
		Triangles[i].Texture = Texture;
}