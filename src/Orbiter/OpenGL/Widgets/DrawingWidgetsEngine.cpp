#include "DrawingWidgetsEngine.h"

/* Standard OpenGL/GLX header files */
#include <GL/gl.h>

#include <stdlib.h>

#include "basicwindow.h"
#include "bezierwindow.h"

#include "DCE/Logger.h"

namespace GLEffect2D
{

DrawingWidgetsEngine::DrawingWidgetsEngine(int Width, int Height)
{
	this->Width = Width;
	this->Height = Height;
} 

DrawingWidgetsEngine::~DrawingWidgetsEngine()
{
}

void DrawingWidgetsEngine::Paint()
{

	vector<TBaseWidget*>::iterator Widget;
	for(Widget = Widgets.begin(); Widget < Widgets.end(); Widget++)
	{
		TBaseWidget* WidgetPtr = *Widget;
		if(WidgetPtr->GetParent() == NULL)
		{
			WidgetPtr->Paint();
			string WidgetName = "Widget: " + WidgetPtr->GetName();

			DCE::g_pPlutoLogger->Write(LV_CRITICAL, WidgetName.c_str());
		}
	}
	MeshTransform Transform;
	Transform.ApplyTranslate( (float)- Width / 2, (float)- Height / 2, (float)Height / 2);

	Context.Paint(Transform);
	//DCE::g_pPlutoLogger->Write(LV_CRITICAL, "EndFrame");
}

TBaseWidget* DrawingWidgetsEngine::CreateWidget(int WidgetType, int Top, int Left, int Width, int Height, char* Text)
{
	TBaseWidget * Result = NULL;
	switch(WidgetType) {
		case BASICWINDOW:
			Result = new TBasicWindow(&Context, Top, Left, Width, Height, Text);
			break;
		case BEZIERWINDOW:
			Result = new TBezierWindow(&Context,Top, Left, Width, Height, Text);
 			break;
	}
	
	AddWidget(Result);
	
	return Result;
}

void DrawingWidgetsEngine::AddWidget(TBaseWidget * Widget) {
	Widgets.push_back(Widget);
}

void DrawingWidgetsEngine::DeleteWidget(TBaseWidget* Widget) {
	vector<TBaseWidget*>::iterator FindItem;
	for(FindItem = Widgets.begin(); FindItem < Widgets.end(); FindItem++)
		if((*FindItem) == Widget) {
			delete Widget;
			Widgets.erase(FindItem);
			return;
		}
}

void DrawingWidgetsEngine::RemoveAll()
{
	vector<TBaseWidget*>::iterator FindItem;
	for(FindItem = Widgets.begin(); FindItem < Widgets.end(); FindItem++)
		delete *FindItem;
	Widgets.clear();
}

bool DrawingWidgetsEngine::RemoveWidgetFromList(TBaseWidget* Widget)
{
	vector<TBaseWidget*>::iterator FindItem;
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

}
