#include "MeshPainter.h"

#include "TextureManager.h"

#include <GL/gl.h>

/*virtual*/ void MeshPainter::PaintContainer(MeshContainer& Container, MeshTransform& Transform)
{
	int Count;
	MeshVertex* Vertexes = new MeshVertex[Container.NoVertexes];
	
	for(Count = 0; Count < Container.NoVertexes; Count++)
	{
		Vertexes[Count] = Container.Vertexes[Count];
		Vertexes[Count].ApplyTransform(Transform);
	}
	
	for(Count = 0; Count < Container.NoTriangles; Count++)
	{
		MeshTriangle& Triangle = Container.Triangles[Count];
		
		
		glBegin(GL_TRIANGLES);

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
		
		glEnd();
	}
	
	delete Vertexes;	
}
