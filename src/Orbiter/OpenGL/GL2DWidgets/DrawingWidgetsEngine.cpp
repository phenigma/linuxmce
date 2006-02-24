#include "DrawingWidgetsEngine.h"

/* Standard OpenGL/GLX header files */
#include <GL/gl.h>

#include <stdlib.h>

#include "basicwindow.h"
#include "bezierwindow.h"

namespace DCE {

DrawingWidgetsEngine::DrawingWidgetsEngine(int Width, int Height)
{
	this->Width = Width;
	this->Height = Height;
	/**
	  OpenGL starting code 
	*/
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);  // Change Matrix Mode to Projection
	glLoadIdentity();             // Reset View
	glOrtho(0, Width, 0, Height, 0, Width);
	glMatrixMode(GL_MODELVIEW);   // Change Projection to Matrix Mode
	glLoadIdentity();
	
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 1.0f, 0.0f, 0.0f);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glFlush();
	
	/**
		Activate alpha blending 
	*/	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
} 

/*!
    \fn DrawingWidgetsEngine::ClearScreen(unsigned char Red, unsigned char Green, unsigned char Blue)
    
    Clear the openGL screen using an RGB based color
 */
void DrawingWidgetsEngine::ClearScreen(unsigned char Red, unsigned char Green, unsigned char Blue)
{
	glClearColor(Red/255.0f, Green/255.0f, Blue/255.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

/**
 * Paint all widgets registred
 */
void DrawingWidgetsEngine::Paint()
{
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

/**
 * That function add inthe drawing list the widgets
  * @param Widget 
 */
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

// TODO: delete all widgets
void DrawingWidgetsEngine::RemoveAll()
{
	std::vector<TBaseWidget*>::iterator FindItem;
	for(FindItem = Widgets.begin(); FindItem < Widgets.end(); FindItem++)
		delete *FindItem;
	Widgets.clear();
}

} /// end namespace DCE
