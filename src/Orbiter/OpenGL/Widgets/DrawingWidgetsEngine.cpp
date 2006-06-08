#include "DrawingWidgetsEngine.h"

/* Standard OpenGL/GLX header files */
#include <GL/gl.h>

#include <stdlib.h>

#include "basicwindow.h"
#include "bezierwindow.h"

DrawingWidgetsEngine::DrawingWidgetsEngine(int Width, int Height)
{
	this->Width = Width;
	this->Height = Height;
} 

DrawingWidgetsEngine::~DrawingWidgetsEngine()
{
}

void DrawingWidgetsEngine::Paint(MeshTransform& Transform)
{
	std::vector<TBaseWidget*>::iterator Widget;
	for(Widget = Widgets.begin(); Widget < Widgets.end(); Widget++)
	{
		TBaseWidget* WidgetPtr = *Widget;
		if(WidgetPtr->GetParent() == NULL)
		{
			MeshTransform Transform;
			WidgetPtr->Paint(Transform);
			std::string WidgetName = "Widget: " + WidgetPtr->GetName();

			//g_pPlutoLogger->Write(LV_CRITICAL, WidgetName.c_str());
		}
	}
	//g_pPlutoLogger->Write(LV_CRITICAL, "EndFrame");
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
			return;
		}
}

void DrawingWidgetsEngine::RemoveAll()
{
	std::vector<TBaseWidget*>::iterator FindItem;
	for(FindItem = Widgets.begin(); FindItem < Widgets.end(); FindItem++)
		delete *FindItem;
	Widgets.clear();
}

bool DrawingWidgetsEngine::RemoveWidgetFromList(TBaseWidget* Widget)
{
	std::vector<TBaseWidget*>::iterator FindItem;
	for(FindItem = Widgets.begin(); FindItem < Widgets.end(); FindItem++)
		if((*FindItem) == Widget) {
			FindItem = Widgets.erase(FindItem);
			return true;
		}
	return false;
}

void DrawingWidgetsEngine::Resize(int Width, int Height)
{
	this->Width = Width;
	this->Height = Height;
	
}
