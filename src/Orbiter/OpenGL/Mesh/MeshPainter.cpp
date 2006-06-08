#include "MeshPainter.h"

#include "../Texture/TextureManager.h"

#include <GL/gl.h>

#include <stdio.h>

MeshPainter* MeshPainter::_Instance = NULL;

/*static*/ MeshPainter* MeshPainter::Instance()
{
	if(_Instance == NULL)
		_Instance = new MeshPainter();
	return _Instance;
}

MeshPainter::MeshPainter()
{
	pthread_mutex_init(&SafetyPaintMutex, NULL);	
}

/*virtual*/ void MeshPainter::PaintContainer(MeshContainer& Container, MeshTransform& Transform)
{
	//GLSafetyLock LockArea(&SafetyPaintMutex);
	
	int Count;
	MeshVertex* Vertexes = new MeshVertex[Container.NoVertexes];
	
	for(Count = 0; Count < Container.NoVertexes; Count++)
	{
		Vertexes[Count] = Container.Vertexes[Count];
		Vertexes[Count].ApplyTransform(Transform);
	}
	
	glBegin(GL_TRIANGLES);
	for(Count = 0; Count < Container.NoTriangles; Count++)
	{
		MeshTriangle& Triangle = Container.Triangles[Count];
		
		TextureManager::Instance()->SetupTexture(Triangle.Texture);

		MeshVertex& Vertex = Vertexes[Triangle.Vertex1];
		glTexCoord2f(Vertex.UVW.X, Vertex.UVW.Y);
		glColor4f(Vertex.Color.X, Vertex.Color.Y, Vertex.Color.Z, Vertex.UVW.Z);
		glVertex3f(Vertex.X, Vertex.Y, Vertex.Z);
		
		Vertex = Vertexes[Triangle.Vertex2];
		glTexCoord2f(Vertex.UVW.X, Vertex.UVW.Y);
		glColor4f(Vertex.Color.X, Vertex.Color.Y, Vertex.Color.Z, Vertex.UVW.Z);
		glVertex3f(Vertex.X, Vertex.Y, Vertex.Z);
		
		Vertex = Vertexes[Triangle.Vertex3];
		glTexCoord2f(Vertex.UVW.X, Vertex.UVW.Y);
		glColor4f(Vertex.Color.X, Vertex.Color.Y, Vertex.Color.Z, Vertex.UVW.Z);
		glVertex3f(Vertex.X, Vertex.Y, Vertex.Z);
		
	}
	glEnd();
	
	delete Vertexes;	
}

void MeshPainter::CleanUp()
{
	
}
