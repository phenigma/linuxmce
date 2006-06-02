#ifndef BEZIERWINDOW_H
#define BEZIERWINDOW_H

/**
  @author Ciprian Mustiata (ciprian.m@plutohome.com)
*/

#include "basewidget.h"
#include "../../DesignObj_Orbiter.h"


/**
 * Bezier code - based on NEHE tutorial
 * http://nehe.gamedev.net/data/lessons/lesson.asp?lesson=28
 * 
 */

/** Structure For A 3rd Degree Bezier Patch 
*/
typedef struct bpatch {
	// 4x4 Grid Of Anchor Points
	POINT_3D	anchors[4][4];				
	// Display List For Bezier Patch
	GLuint		dlBPatch;					
	// Texture For The Patch
	GLuint		texture;	
	//  Number of patch divisions
	int Divisions;
} BEZIER_PATCH;

/** That class describe a bezier patch. That is defined as a 16 points 
*/
class TBezierWindow : public TBaseWidget
{
public:
	/**
	* Creates a bezier window with with default color and settigs
	* @param Left Left coordinate of the window (in pixels) 
	* @param Top Top coordinate of the window (in pixels)
	* @param Width Width size of the window (in pixels)
	* @param Height Height size of the window (in pixels)
	* @param Text Attached text of the window (for now, not used), maybe for future implementations
	*/
 	TBezierWindow(int Left, int Top, int Width, int Height, char* Text);
	virtual ~TBezierWindow();

	/**
	* Paint the window using defaults
	*/ 
	virtual void Paint(MeshFrame* Frame);

	/**  Set up a background texture to one Window
	 * @param Background That surface must be in size as power of two (e.g. 256 x 512)
	 *      Default texture mapping will fill completly the window. 
	 *	Future implementation should add texture mapping wrappers
	 *  
	 * @return it returns true if the texture was corectly set
	 */
	bool SetBackgroundImage(OpenGLTexture Background);

public:
	/**
	  Getters
	*/
	
	float GetAlpha() const {
		return Background.Alpha;	
	}
	
	int GetLeft() const {
		return Left;
	}

	int GetTop() const {
		return Top;
	}
	
	int GetWidth() const {
		return Width;
	}

	int GetHeight() const {
		return Height;
	}
	
	/*
	  Setters
	*/
	/** Set the level of transparency (0.= completly transparent
	 */
	void SetAlpha(float Alpha) {
		if(Alpha>1.0f)
			Background.Alpha = 1.0f;
		else
			Background.Alpha = Alpha;
	}
	
	/** Set the left coordinate of the window
	 */
	void SetLeft(int Left) {
		this->Left = Left;
	}

	/** Set the top coordinate of the window
	 */
	void SetTop(int Top) {
		this->Top = Top;
	}

	/** Set the width (in pixels) of the window
	 */
	void SetWidth(int Width) {
		this->Width = Width;
	}

	/** Set the height (in pixels) of the window
	 */
	void SetHeight(int Height) {
		this->Height = Height;
	}

	/** Set the RGBA color of the window. That color will mix with the curent texture if exists
	 */
	void SetBackgroundColor(float Red, float Green, float Blue, float Alpha) {
		Background.Red = Red;
		Background.Green = Green;
		Background.Blue = Blue;
		Background.Alpha = Alpha;
	}
	
	/** Set the UV texture coordinates of the texture. That will make the texture to be streched without any 
	  geometry modification. Too is used to fix the non power of 2 texture size. 
	 */
	void SetTextureWraping(float LeftCoordinate, float TopCoordinate, 
		float WidthCoordinate, float HeightCoordinate) {
		TextureWrapper2D.Left = LeftCoordinate;
		TextureWrapper2D.Top = TopCoordinate;
		TextureWrapper2D.Width = WidthCoordinate;
		TextureWrapper2D.Height = HeightCoordinate;
	}
	
	/** Utility function: given one FloatRect it set the coordinates of the window
	 * using the float based coordinates
	 */
	void SetRectCoordinates(FloatRect Coordinates);
		
	/** Utility function: creates proper OpenGL texture for one
	 * non-rectangular surface
	 * automatically set up the correct wrapping coordinates
	 */
	void SetUpTextureFromSurface(PlutoGraphic *Picture);
	
	//Test:
	BEZIER_PATCH BezierDefinition;
};

#endif

