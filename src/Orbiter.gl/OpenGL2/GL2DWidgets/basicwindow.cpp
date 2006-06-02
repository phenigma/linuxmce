#include "basicwindow.h"

#include <GL/gl.h>
#include "../OpenGLTextureConverter.h"

#include "../3dcontrols/MeshBuilder.h"

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
	: TBaseWidget(Left, Top, Width, Height, Text),
	  Container	(NULL)
{
	CreateMeshContainer();
	IsFrameSetup = false;
	FloatRect Coordinates;
	Coordinates.Left = Left;
	Coordinates.Top = Top;
	Coordinates.Width = Width;
	Coordinates.Height = Height;
	SetRectCoordinates(Coordinates);
}

void TBasicWindow::CreateMeshContainer()
{
	MeshBuilder MB;
	MB.Begin(MBMODE_TRIANGLE_STRIP);
	MB.SetTexture2D(0.0f, 0.0f);
	MB.SetAlpha(0.5);
	//glDisable(GL_BLEND);

	
	MB.SetTexture2D(0.0f, 0.0f);
	MB.SetColor(1.0f, 1.0f, 0.0f);
	MB.AddVertexFloat(-640, 480, -480);
	MB.SetColor(1.0f, 0.0f, 0.0f);
	MB.AddVertexFloat( -640, -480, -480);
	MB.SetColor(0.0f, 1.0f, 0.0f);
	MB.AddVertexFloat( 640,-480, -480); 

	Container = MB.End();

	Frame = new MeshFrame();
	Frame->SetMeshContainer(Container);	

}

TBasicWindow::~TBasicWindow() {
	delete Container;
	Frame->RemoveChild(this->Frame);
}

/**
 * Paint the window using defaults
 */ 
void TBasicWindow::Paint(MeshFrame* Frame)
{
	if (!Visible)
		return;
		
	
	glBindTexture(GL_TEXTURE_2D, BackgroundTex);
	// a window has 4 points
	glBegin(GL_QUADS);
	//set up the color for rendering
	
	glColor4f(Background.Red , Background.Green, Background.Blue, 
		Background.Alpha);	
	//first point
	glTexCoord2f( TextureWrapper2D.Left, TextureWrapper2D.Top ); 
	glVertex3f(float(Left), float(Top), 0.0f);
	//second point
	glTexCoord2f( TextureWrapper2D.Left, TextureWrapper2D.Top + 
		TextureWrapper2D.Height); 
	glVertex3f(float(Left), float(Top + Height), 0.0f);
	//third point
	glTexCoord2f( TextureWrapper2D.Left+TextureWrapper2D.Width, 
		TextureWrapper2D.Top + TextureWrapper2D.Height); 
	glVertex3f(float(Left + Width), float(Top + Height), 0.0f);
	//fourth point
	glTexCoord2f( TextureWrapper2D.Left+TextureWrapper2D.Width, TextureWrapper2D.Top); 
	glVertex3f(float(Left + Width), float(Top), 0.0f);
	glEnd();
	/*
	if(IsFrameSetup == false)
		Frame->AddChild(this->Frame);
	Container->SetTexture(1);*/
	TBaseWidget::Paint(Frame);
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

void TBasicWindow::SetUpTextureFromSurface(PlutoGraphic* Picture)
{
	if(Picture == NULL)
		return;
	
	if(BackgroundTex != 0)
		glDeleteTextures(1, &BackgroundTex);
		
	SetTextureWraping(0.0, 0.0, 
		((float)Picture->Width)/MathUtils::MinPowerOf2(Picture->Width), 
		((float)Picture->Height)/MathUtils::MinPowerOf2(Picture->Height));	
	
	BackgroundTex = OpenGLTextureConverter::GenerateTexture(Picture);
}

void TBasicWindow::SetRectCoordinates(FloatRect Coordinates)
{
	SetLeft((int)Coordinates.Left);
	SetTop((int)Coordinates.Top);
	SetWidth((int)Coordinates.Width);
	SetHeight((int)Coordinates.Height);
	MeshTransform Transform;
	Transform.Translate(0, 0, -Height);
	Transform.Scale(Width, Height, 0);
	Frame->SetTransform(Transform);
}

