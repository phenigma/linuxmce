#ifndef MeshBuilder_H_
#define MeshBuilder_H_

#include "MeshContainer.h"

/**
 * MB_BMODE = Mesh Builder BuildMode
 * MB_BMODE_TRIANGLES = classic mode, similar with glBegin(GL_Triangles)
 * 
 */ 
#define MBMODE_TRIANGLES			1
#define MBMODE_TRIANGLE_STRIP 		2

#include <vector>
#include "../Texture/TextureManager.h"
#include "../OpenGLGraphic.h"

class MeshBuilder
{
	unsigned char BuildMode;
	
	std::vector<MeshVertex> Vertexes;
	std::vector<MeshTriangle>Triangles;
	
	float GlobalU, GlobalV, GlobalAlpha;
	float GlobalRed, GlobalGreen, GlobalBlue;
	OpenGLGraphic* GlobalMaterial;
	
	virtual void BuildTriangle();
	virtual void AddTriangle(int Vertex1, int Vertex2, int Vertex3);
	
public:
	MeshBuilder(void);
	virtual ~MeshBuilder(void);
	
	virtual void CleanUp();
	
	virtual void Begin(unsigned char BuildMode);
	
	/**
	 * At the end of building of the primitive as result will be a MeshContainer
	 */ 
	virtual MeshContainer* End();
	
	virtual void AddVertexFloat(float X, float Y, float Z);
	virtual void AddVertexFloatUV(float X, float Y, float Z, float U, float V);

	virtual void SetTexture2D(float U, float V);
	virtual void SetTexture(OpenGLGraphic* Texture);
	virtual void SetColor(float Red, float Green, float Blue);
	
	virtual void SetAlpha(float Alpha);
};

#endif
