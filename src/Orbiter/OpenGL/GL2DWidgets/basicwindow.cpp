#include "basicwindow.h"

#include <GL/gl.h>
#include "../OpenGLTextureConverter.h"

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
void TBasicWindow::Paint()
{
	if (!Visible)
		return;
		
	TBaseWidget::Paint();
	
	glLoadIdentity();
	
	glBindTexture(GL_TEXTURE_2D, BackgroundTex);
	// a window has 4 points
	glBegin(GL_QUADS);
	//set up the color for rendering
	
	glColor4f(Background.Red , Background.Green, Background.Blue, 
		Background.Alpha);	
	//first point
	glTexCoord2f( TextureWrapper2D.Left, TextureWrapper2D.Top + 
		TextureWrapper2D.Height); 
	glVertex3f(float(Left), float(Top), 0.0f);
	//second point
	glTexCoord2f( TextureWrapper2D.Left, TextureWrapper2D.Top ); 
	glVertex3f(float(Left), float(Top + Height), 0.0f);
	//third point
	glTexCoord2f( TextureWrapper2D.Left+TextureWrapper2D.Width, TextureWrapper2D.Top); 
	glVertex3f(float(Left + Width), float(Top + Height), 0.0f);
	//fourth point
	glTexCoord2f( TextureWrapper2D.Left+TextureWrapper2D.Width, 
		TextureWrapper2D.Top + TextureWrapper2D.Height); 
	glVertex3f(float(Left + Width), float(Top), 0.0f);
	glEnd();
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
}

