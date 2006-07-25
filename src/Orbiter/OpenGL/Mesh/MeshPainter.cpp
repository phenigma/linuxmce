#include "MeshPainter.h"

#include "../Texture/TextureManager.h"
#include "../OpenGLGraphic.h"
#include "../ExtensionManager.h"

#include <GL/gl.h>

#include <stdio.h>

#include "DCE/Logger.h"

MeshPainter* MeshPainter::Instance_ = NULL;

//#define DEBUG_LINES

/*static*/ MeshPainter* MeshPainter::Instance()
{
	if(Instance_ == NULL)
		Instance_ = new MeshPainter();
	return Instance_;
}

MeshPainter::MeshPainter()
{
	pthread_mutex_init(&SafetyPaintMutex, NULL);	
}

void MeshPainter::Setup(ExtensionManager *ExtensionManager)
{
	ExtensionManager_ = ExtensionManager;
}

/*virtual*/ void MeshPainter::PaintContainer(MeshContainer& Container, MeshTransform& Transform,
	MeshTransform& TextureTransform)
{
	if(NULL == ExtensionManager_)
		throw "ExtensionManager_ not set!";

	if(Container.Blended_)
	{
		ExtensionManager_->EnableZBuffer(false);
		ExtensionManager_->EnableBlended(true);
	}

	int Count;
	MeshVertex* Vertexes = new MeshVertex[Container.NoVertexes];
	
	for(Count = 0; Count < Container.NoVertexes; Count++)
	{
		Vertexes[Count] = Container.Vertexes[Count];
		Vertexes[Count].ApplyTransform(Transform);
	}
	
#ifdef DEBUG
	//if(Container.NoTriangles)
		//DCE::g_pPlutoLogger->Write(LV_WARNING, "GL_TRIANGLES");
#endif

	glBegin(GL_TRIANGLES);
	for(Count = 0; Count < Container.NoTriangles; Count++)
	{
		MeshTriangle& Triangle = Container.Triangles[Count];

		TextureManager::Instance()->SetupTexture(Triangle.Texture ? Triangle.Texture->Texture : 0);

		if (Triangle.Texture)
		{
			float MaxU = Triangle.Texture->MaxU;
			float MaxV = Triangle.Texture->MaxV;

			MeshVertex& Vertex = Vertexes[Triangle.Vertex1];
			glTexCoord2f(Vertex.UVW.X*MaxU, Vertex.UVW.Y*MaxV);
			glColor4f(Vertex.Color.X, Vertex.Color.Y, Vertex.Color.Z, Vertex.Alpha);
			glVertex3f(Vertex.X, Vertex.Y, Vertex.Z);

			Vertex = Vertexes[Triangle.Vertex2];
			glTexCoord2f(Vertex.UVW.X*MaxU, Vertex.UVW.Y*MaxV);
			glColor4f(Vertex.Color.X, Vertex.Color.Y, Vertex.Color.Z, Vertex.Alpha);
			glVertex3f(Vertex.X, Vertex.Y, Vertex.Z);

			Vertex = Vertexes[Triangle.Vertex3];
			glTexCoord2f(Vertex.UVW.X*MaxU, Vertex.UVW.Y*MaxV);
			glColor4f(Vertex.Color.X, Vertex.Color.Y, Vertex.Color.Z, Vertex.Alpha);
			glVertex3f(Vertex.X, Vertex.Y, Vertex.Z);
		}
		else
		{
			MeshVertex& Vertex = Vertexes[Triangle.Vertex1];
			glColor4f(Vertex.Color.X, Vertex.Color.Y, Vertex.Color.Z, Vertex.Alpha);
			glVertex3f(Vertex.X, Vertex.Y, Vertex.Z);

			Vertex = Vertexes[Triangle.Vertex2];
			glColor4f(Vertex.Color.X, Vertex.Color.Y, Vertex.Color.Z, Vertex.Alpha);
			glVertex3f(Vertex.X, Vertex.Y, Vertex.Z);

			Vertex = Vertexes[Triangle.Vertex3];
			glColor4f(Vertex.Color.X, Vertex.Color.Y, Vertex.Color.Z, Vertex.Alpha);
			glVertex3f(Vertex.X, Vertex.Y, Vertex.Z);			
		}
	}
	glEnd();

#ifdef DEBUG_LINES
	for(Count = 0; Count < Container.NoTriangles; Count++) 
	{
		MeshTriangle& Triangle = Container.Triangles[Count];

		glDisable(GL_TEXTURE_2D);
		glBegin(GL_LINE_STRIP);
		glColor3f(1, 1, 1);
		MeshVertex& Vertex = Vertexes[Triangle.Vertex1];
		glVertex3f(Vertex.X, Vertex.Y, Vertex.Z);
		Vertex = Vertexes[Triangle.Vertex2];
		glVertex3f(Vertex.X, Vertex.Y, Vertex.Z);

		Vertex = Vertexes[Triangle.Vertex3];
		glVertex3f(Vertex.X, Vertex.Y, Vertex.Z);

		Vertex = Vertexes[Triangle.Vertex1];
		glVertex3f(Vertex.X, Vertex.Y, Vertex.Z);

		glEnd();
		glEnable(GL_TEXTURE_2D);
	}
#endif

	if(Container.Blended_)
	{
		ExtensionManager_->EnableZBuffer(false);
		ExtensionManager_->EnableBlended(false);
	}
	
	delete Vertexes;	
}

void MeshPainter::CleanUp()
{	
	delete Instance_;
	Instance_ = NULL;
}
