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
	/**
	* That function add inthe drawing list the widgets
	* @param Widget 
	*/
	void AddWidget(TBaseWidget* Widget);

	/**
	 *	Safe thread mutex
	 */
	pluto_pthread_mutex_t* m_GLTextureThreadMutex;
	pthread_cond_t m_GLTextureThreadCond;
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
	 *	Default constructor
	 */
	DrawingWidgetsEngine(int Width, int Height);

	/*
	 *	Default no-op destructor 
	 */
	~DrawingWidgetsEngine();
	///	Painting routines
	/**
	 * Clear the openGL screen using an RGB based color
	 * @param Red Red component of the clear color
	 * @param Green Green component of the clear color
	 * @param Blue Blue component of the clear color
	 */
	void ClearScreen(unsigned char Red, unsigned char Green, unsigned char Blue);
	/** Paint all widgets registred
	*/
	void Paint();
    	
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
	 * Texture management
	 */

	OpenGLTexture OldScreen, NewScreen;

	FloatRect UVRect;

	PlutoGraphic *NextDisplay, *LastDisplay;

	void SetUpNextDisplay();

	/**
	  Configuring next image (based for transitions), the attached texture is NewScreen
	 */
	void ConfigureNextScreen(PlutoGraphic* NextScreen);

	/**
	  Configuring last image (based for transitions), the attached texture is OldScreen
	  Is mainly used for PocketFrog implementation which destroys constantly the OpenGL context
	 */
	void ConfigureOldScreen(PlutoGraphic* Last);
	
};

#endif
