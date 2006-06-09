#ifndef BASICWINDOW_H
#define BASICWINDOW_H

#include "basewidget.h"

/**
@author ciplogic
*/

/**
 * We use for FloatRect
 */

class TBasicWindow : public TBaseWidget
{

	
public:
	TBasicWindow(int Left, int Top, int Width, int Height, char* Text);
	virtual ~TBasicWindow();

	virtual void Paint(MeshTransform& Transform);

	/**
	 *        Set up a background texture to one Window
	 * @param Background That surface must be in size as power of two (e.g. 256 x 512)
	 *      Default texture mapping will fill completly the window. 
	 *	Future implementation should add texture mapping wrappers
	 *  
	 * @return it returns true if the texture was corectly set
	 */
	bool SetBackgroundImage(OpenGLGraphic* Background);

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
	
	/**
	  Setters
	*/
	void SetAlpha(float Alpha) {
		if(Alpha>1.0f)
			Background.Alpha = 1.0f;
		else
			Background.Alpha = Alpha;
	}
	
	void SetLeft(int Left) {
		this->Left = Left;
	}

	void SetTop(int Top) {
		this->Top = Top;
	}

	void SetWidth(int Width) {
		this->Width = Width;
	}

	void SetHeight(int Height) {
		this->Height = Height;
	}

	void SetBackgroundColor(float Red, float Green, float Blue, float Alpha) {
		Background.Red = Red;
		Background.Green = Green;
		Background.Blue = Blue;
		Background.Alpha = Alpha;
	}
	
	void SetTextureWraping(float LeftCoordinate, float TopCoordinate, 
		float WidthCoordinate, float HeightCoordinate) {
		TextureWrapper2D.Left = LeftCoordinate;
		TextureWrapper2D.Top = TopCoordinate;
		TextureWrapper2D.Width = WidthCoordinate;
		TextureWrapper2D.Height = HeightCoordinate;
	}
	
	void SetTextureWraping(FloatRect UVRect) {
		TextureWrapper2D.Left = UVRect.Left;
		TextureWrapper2D.Top = UVRect.Top;
		TextureWrapper2D.Width = UVRect.Width;
		TextureWrapper2D.Height = UVRect.Height;
	}
	/**
	 * Utility function: given one FloatRect it set the coordinates of the window
	 * using the float based coordinates
	 */
	void SetRectCoordinates(FloatRect Coordinates);
			
	//Test::
	float RotateYVal;
	void RotateY(float Step) {RotateYVal = Step;}

};

#endif
