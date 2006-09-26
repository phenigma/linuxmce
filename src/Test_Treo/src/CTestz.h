#ifndef CTESTZ_H
#define CTESTZ_H

#include <PalmOS.h>
#include <string>
#include "PlutoInc.h"

using namespace std;

#define _DOUBLEBUFFERING_





class CTestz {
private:
  void PlutoColorToRGBColorType( RGBColorType& dstColor, PlutoColor srcColor ){
  	dstColor.index = 0;
	dstColor.r = srcColor.R();
	dstColor.g = srcColor.G();
	dstColor.b = srcColor.B();
  }	
  
  WinHandle m_hScreenBuffer, m_hScreen;
  
  UInt32 m_ui32Width, m_ui32Height, m_ui32Depth;
  UInt16 m_ui16CoordSys;
  
  RectangleType m_rBufferBounds, m_rScreenBounds;
  void SetScreenBuffer( bool bActive=true );
  void DrawBufferToScreen( void );
  
  
public:

	CTestz();
	virtual ~CTestz();
	
	void erase( void );
	
	virtual void Configure();
	
	virtual void SolidRectangle(int iX, int iY, int iWidth, int iHeight, PlutoColor color );//, string ParentObjectID = "", string ObjectID = "");
	virtual void HollowRectangle(int X, int Y, int Width, int Height, PlutoColor color );//, string ParentObjectID = "", string ObjectID = "");
	virtual void DrawLine(int iX, int iY, int iWidth, int iHeight, PlutoColor color);
	
	//virtual void RenderText( string &sTextToDisplay, class DesignObjText *Text, class TextStyle *pTextStyle, PlutoPoint point = PlutoPoint(0, 0), string ObjectID = "") = 0;
	virtual void RenderText( string &sTextToDisplay, class TextStyle *pTextStyle, PlutoPoint point );
	
	/*
	void test(void){
		char timeString[10]="Test";
		WinDrawChars(timeString, StrLen(timeString), 10, 10);
	}
	*/
};


#endif // CTESTZ_H
