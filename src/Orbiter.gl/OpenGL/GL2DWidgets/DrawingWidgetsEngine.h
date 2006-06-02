/**
  @author Ciprian Mustiata (ciprian.m@plutohome.com)
*/

#ifndef DRAWINGWIDGETSENGINE_H
#define DRAWINGWIDGETSENGINE_H

#include "basewidget.h"

#include <vector>

#define BASICWIDGET		1000
#define BASICWINDOW		(BASICWIDGET+1)
#define BEZIERWINDOW	(BASICWIDGET+2)

#include "../../DesignObj_Orbiter.h"

#include "PlutoUtils/MultiThreadIncludes.h"

class DrawingWidgetsEngine
{
	int Width, Height;
	std::vector<TBaseWidget*> Widgets;


public:
	/**
	* That function add inthe drawing list the widgets
	* @param Widget 
	*/
	void AddWidget(TBaseWidget* Widget);

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
	 *	Default constructor
	 */
	DrawingWidgetsEngine(int Width, int Height);

	/*
	 *	Default no-op destructor 
	 */
	~DrawingWidgetsEngine();
	///	Painting routines

	/** Paint all widgets registred
	*/
	void Paint(MeshTransform& Transform);
    	
	/**
		Widget list management
	*/
	/**
	 *    Factory method
	 * @param WidgetType Type of window it wants to be created 
	 * @param Top The top of final window
	 * @param Left The left of final window
	 * @param Width The width of final window
	 * @param Height The height of final window
	 * @param Text The text of final window
	 * @return 
	 */
	TBaseWidget* CreateWidget(int WidgetType, int Top, int Left, int Width, int Height, char* Text);
	/**
	 * Delete specific widget from painting list
	 * @param Widget Widget to be removed from the list. 
	 */
	void DeleteWidget(TBaseWidget* Widget);
	/**
	 *    Clears all widgets from drawing list
	 */
	void RemoveAll();

	/**
	 *	Removes the widget from the list of drawing widgets without delete it as object
	 *	returns false in case that widget doesn't exist
	 */
	bool RemoveWidgetFromList(TBaseWidget* Widget);


};

#endif
