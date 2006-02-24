#ifndef DRAWINGWIDGETSENGINE_H
#define DRAWINGWIDGETSENGINE_H

#include "basewidget.h"

#include <vector>

#define BASICWIDGET	1000
#define BASICWINDOW	(BASICWIDGET+1)
#define BEZIERWINDOW	(BASICWIDGET+2)

namespace DCE {

class DrawingWidgetsEngine
{
	int Width, Height;
	std::vector<TBaseWidget*> Widgets;
    	
	void AddWidget(TBaseWidget* Widget);

public:
	/**
	 * Getters
	 */
	int GetWidth() {
		return Width;
	}
	
	int GetHeight() {
		return Height;
	}
	/**
		Default constructor
	*/
	DrawingWidgetsEngine(int Width, int Height);
	/**
		Painting routines
	*/
	void ClearScreen(unsigned char Red, unsigned char Green, unsigned char Blue);
	void Paint();
    	
	/**
		Widget list management
	*/
	TBaseWidget* CreateWidget(int WidgetType, int Top, int Left, int Width, int Height, char* Text);
	void DeleteWidget(TBaseWidget* Widget);
	void RemoveAll();
};

}
#endif
