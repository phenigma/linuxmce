#include "DrawingWidgetsEngine.h"

/* Standard OpenGL/GLX header files */
#include <GL/gl.h>

#include <stdlib.h>

#include "basicwindow.h"
#include "bezierwindow.h"

#include "../OpenGLTextureConverter.h"

DrawingWidgetsEngine::DrawingWidgetsEngine(int Width, int Height)
{
	this->Width = Width;
	this->Height = Height;
	this->NextDisplay = NULL;
	this->LastDisplay = NULL;
	OldScreen = 0;
	NewScreen = 0;
	
	UVRect.Width = (float)Width/MathUtils::MinPowerOf2(Width);
	UVRect.Height = (float)Height/MathUtils::MinPowerOf2(Height);

	m_GLTextureThreadMutex = new pluto_pthread_mutex_t("OpenGL Texture safety lock");	
	pthread_cond_init(&m_GLTextureThreadCond, NULL);
	m_GLTextureThreadMutex->Init(NULL, &m_GLTextureThreadCond);
} 

DrawingWidgetsEngine::~DrawingWidgetsEngine()
{
	delete m_GLTextureThreadMutex;
}

void DrawingWidgetsEngine::ClearScreen(unsigned char Red, unsigned char Green, unsigned char Blue)
{
	glClearColor(Red/255.0f, Green/255.0f, Blue/255.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void DrawingWidgetsEngine::Paint()
{
	SetUpNextDisplay();
	ClearScreen(0, 0, 0);
	std::vector<TBaseWidget*>::iterator Widget;
	for(Widget = Widgets.begin(); Widget < Widgets.end(); Widget++)
		//display only parent widgets (windows), the drawing is a duty of parents to be call
		if((*Widget)->GetParent() == NULL)
			(*Widget)->Paint();
}

TBaseWidget* DrawingWidgetsEngine::CreateWidget(int WidgetType, int Top, int Left, int Width, int Height, char* Text)
{
	TBaseWidget * Result = NULL;
	switch(WidgetType) {
		case BASICWINDOW:
			Result = new TBasicWindow(Top, Left, Width, Height, Text);
			break;
		case BEZIERWINDOW:
			Result = new TBezierWindow(Top, Left, Width, Height, Text);
 			break;
	}
	
	AddWidget(Result);
	
	return Result;
}

void DrawingWidgetsEngine::AddWidget(TBaseWidget * Widget) {
	Widgets.push_back(Widget);
}

void DrawingWidgetsEngine::DeleteWidget(TBaseWidget* Widget) {
	std::vector<TBaseWidget*>::iterator FindItem;
	for(FindItem = Widgets.begin(); FindItem < Widgets.end(); FindItem++)
		if((*FindItem) == Widget) {
			delete Widget;
			Widgets.erase(FindItem);
		}
}

void DrawingWidgetsEngine::RemoveAll()
{
	std::vector<TBaseWidget*>::iterator FindItem;
	for(FindItem = Widgets.begin(); FindItem < Widgets.end(); FindItem++)
		delete *FindItem;
	Widgets.clear();
}

void DrawingWidgetsEngine::SetUpNextDisplay()
{
	PLUTO_SAFETY_LOCK(glm, *m_GLTextureThreadMutex);
	if(!NextDisplay)
		return;
	if(OldScreen)
	{
		glDeleteTextures(1, &OldScreen);
	}
	OldScreen = NewScreen;
	NewScreen = OpenGLTextureConverter::GenerateTexture(NextDisplay);

	if (LastDisplay)
	{
		OldScreen = OpenGLTextureConverter::GenerateTexture(LastDisplay);
		delete this->LastDisplay;
		this->LastDisplay = NULL;
	}


	if(this->NextDisplay)
	{
		delete this->NextDisplay;
		this->NextDisplay = NULL;
	}
}

void DrawingWidgetsEngine::ConfigureNextScreen(PlutoGraphic* NextDisplay)
{
	PLUTO_SAFETY_LOCK(glm, *m_GLTextureThreadMutex);
	if(!NextDisplay)
		return;
	if(this->NextDisplay)
	{
		delete this->NextDisplay;
		this->NextDisplay = NULL;
	}
	this->NextDisplay = NextDisplay->Clone();
}

void DrawingWidgetsEngine::ConfigureOldScreen(PlutoGraphic* LastDisplay)
{
	PLUTO_SAFETY_LOCK(glm, *m_GLTextureThreadMutex);
	if(this->LastDisplay)
	{
		delete this->LastDisplay;
		this->LastDisplay = NULL;
	}
	this->LastDisplay = LastDisplay->Clone();    
}
