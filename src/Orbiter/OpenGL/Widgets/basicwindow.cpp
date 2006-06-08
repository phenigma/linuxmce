#include "basicwindow.h"

#include <GL/gl.h>

#include "../Mesh/MeshBuilder.h"
#include "../Mesh/MeshFrame.h"

/**
 * Creates a basic window with with default color and settigs
 * @param Left 
 * @param Top 
 * @param Width 
 * @param Height 
 * @param Text 
 * @return 
 */
TBasicWindow::TBasicWindow(int Left, int Top, int Width, int Height, char* Text)
	: TBaseWidget(Left, Top, Width, Height, Text) {
}

TBasicWindow::~TBasicWindow() {
}

/**
 * Paint the window using defaults
 */ 
void TBasicWindow::Paint(MeshTransform& Transform)
{
	if (!Visible)
		return;
		
	TBaseWidget::Paint(Transform);
	
	MeshContainer* Container;
	MeshBuilder MB;

	MB.Begin(MBMODE_TRIANGLE_STRIP);
	MB.SetColor(1.0f, 1.0f, 1.0f);
	MB.SetAlpha(Background.Alpha);
	MB.SetTexture(BackgroundTex);
	//1st
	MB.SetTexture2D(TextureWrapper2D.Left, TextureWrapper2D.Top ); 
	MB.AddVertexFloat(float(Left), float(Top), 0.0f);
	//2nd
	MB.SetTexture2D(TextureWrapper2D.Left, TextureWrapper2D.Top + 
		TextureWrapper2D.Height); 
	MB.AddVertexFloat( float(Left), float(Top + Height), 0.0f);
	//3rd
	MB.SetTexture2D(TextureWrapper2D.Left+TextureWrapper2D.Width, TextureWrapper2D.Top);
	MB.AddVertexFloat( float(Left + Width), float(Top), 0.0f);
	//4th
	MB.SetTexture2D(TextureWrapper2D.Left+TextureWrapper2D.Width, 
		TextureWrapper2D.Top + TextureWrapper2D.Height); 
	MB.AddVertexFloat(float(Left + Width), float(Top + Height), 0.0f);

	Container = MB.End();

	MeshFrame* Frame = new MeshFrame();

	Frame->SetMeshContainer(Container);
	Frame->Paint(Transform);
	delete Container;
	Frame->SetMeshContainer(NULL);
	delete Frame;
}

bool TBasicWindow::SetBackgroundImage(OpenGLTexture Background)
{
	// if wants to be set an invalid texture, the function will return false
	if(Background<0)
		return false;
		
	// set up the texture as texture background
	BackgroundTex = Background;
	
	return true;
}

void TBasicWindow::SetRectCoordinates(FloatRect Coordinates)
{
	SetLeft((int)Coordinates.Left);
	SetTop((int)Coordinates.Top);
	SetWidth((int)Coordinates.Width);
	SetHeight((int)Coordinates.Height);
}

