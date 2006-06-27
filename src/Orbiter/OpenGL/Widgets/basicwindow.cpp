#include "basicwindow.h"

#include <GL/gl.h>

#include "../Mesh/MeshBuilder.h"
#include "DCE/Logger.h"

using namespace DCE;

/**
 * Creates a basic window with with default color and settigs
 * @param Left 
 * @param Top 
 * @param Width 
 * @param Height 
 * @param Text 
 * @return 
 */
TBasicWindow::TBasicWindow(MeshFrame* Context,
						   int Left, 
						   int Top, 
						   int Width, 
						   int Height, 
						   std::string Text)
	: TBaseWidget(Context, Left, Top, Width, Height, Text)
{
		MeshBuilder MB;

		MB.Begin(MBMODE_TRIANGLES);
		MB.SetColor(1.0f, 1.0f, 1.0f);
		MB.SetAlpha(Background.Alpha);
		MB.SetTexture(BackgroundTex);
		//1st
		MB.SetTexture2D(0.0f, 0.0f); 
		MB.AddVertexFloat(0.0f, 0.0f, 0.0f);
		//2nd
		MB.SetTexture2D(0.0f, -1024.0f/600.f); 
		MB.AddVertexFloat( 0.0f, 1.0f, 0.0f);
		//3rd
		MB.SetTexture2D(1024.0f/800.f, 0.0f);
		MB.AddVertexFloat( 1.0f, 0.0f, 0.0f);


		//2nd
		MB.SetTexture2D(0.0f, -1024.0f/600.f); 
		MB.AddVertexFloat( 0.0f, 1.0f, 0.0f);
		//3rd
		MB.SetTexture2D(1024.0f/800.f, 0.0f);
		MB.AddVertexFloat( 1.0f, 0.0f, 0.0f);
		//4th
		MB.SetTexture2D(1024.0f/800.f, -1024.0f/600.f); 
		MB.AddVertexFloat(1.0f, 1.0f, 0.0f);

		Container = MB.End();

		Frame = new MeshFrame();

		Frame->SetMeshContainer(Container);

		Context->AddChild(Frame);
}

TBasicWindow::~TBasicWindow() {
	Context->RemoveChild(Frame);
	delete Frame;
}

/**
 * Paint the window using defaults
 */ 
void TBasicWindow::Paint()
{
	if (!Visible)
		return;
		
	TBaseWidget::Paint();
	g_pPlutoLogger->Write(LV_WARNING, "TBasicWindow::Paint %d, %d", Width, Height);
}

bool TBasicWindow::SetBackgroundImage(OpenGLGraphic* Background)
{
	// if wants to be set an invalid texture, the function will return false
	if(Background == 0)
		return false;
		
	// set up the texture as texture background
	BackgroundTex = Background;

	Container->SetTexture(Background);
	
	return true;
}

void TBasicWindow::SetRectCoordinates(FloatRect Coordinates)
{
	MeshTransform Transform;
	Transform.ApplyScale(Coordinates.Width, Coordinates.Height, 1.0f);
	Transform.ApplyTranslate(Coordinates.Left, Coordinates.Top, 0.0f);
	Frame->SetTransform(Transform);
	SetLeft(Coordinates.Left);
	SetTop(Coordinates.Top);
	SetWidth(Coordinates.Width);
	SetHeight(Coordinates.Height);
}

void TBasicWindow::SetTextureWraping(float LeftCoordinate, float TopCoordinate, 
	float WidthCoordinate, float HeightCoordinate) 
{
	TextureWrapper2D.Left = LeftCoordinate;
	TextureWrapper2D.Top = TopCoordinate;
	TextureWrapper2D.Width = WidthCoordinate;
	TextureWrapper2D.Height = HeightCoordinate;
	SetTextureWraping(TextureWrapper2D);
}

void TBasicWindow::SetTextureWraping(FloatRect UVRect) 
{
	TextureWrapper2D = UVRect;
	Container->SetUVMapping(UVRect);
}

