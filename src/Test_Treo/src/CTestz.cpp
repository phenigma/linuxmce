#include "CTestz.h"

CTestz::CTestz()
{
	m_hScreenBuffer = NULL;
}

CTestz::~CTestz()
{
	if ( m_hScreenBuffer ) {
		WinDeleteWindow( m_hScreenBuffer, false );
		m_hScreenBuffer = NULL;
	}
	WinSetCoordinateSystem( m_ui16CoordSys );
}


void CTestz::Configure()
{
	// this enables a 1-to-1 correspondence between coordinates and pixels.
	m_ui16CoordSys = WinGetCoordinateSystem();
	WinSetCoordinateSystem( kCoordinatesNative );
	
	//For Palm OS Garnet, only kCoordinatesNative, kCoordinatesStandard, and kCoordinatesDouble are supported.
	
	/*
	Err err= FtrGet(sysFtrCreator, sysFtrNumROMVersion, &romVersion);
	if ( !err ) {
		//The romVersion parameter should be greater than or equal to 0x05403000
		//sysMakeROMVersion(5,4,0,sysROMStageRelease,0)
		//This feature set corresponds to version 5.4 of Palm OS Garnet.
	}
	*/
	Err err;
	err = WinScreenMode( winScreenModeGet, &m_ui32Width, &m_ui32Height, &m_ui32Depth, NULL );
	#ifdef _DOUBLEBUFFERING_
  	  UInt16 error;
	  m_hScreenBuffer = WinCreateOffscreenWindow( m_ui32Width, m_ui32Height, nativeFormat, &error );
	  WinGetDrawWindowBounds( &m_rBufferBounds );	  
	#endif.
	WinSetCoordinateSystem( m_ui16CoordSys );
}

void CTestz::erase( void )
{
  if ( m_hScreenBuffer ) {
  	m_hScreen = WinGetDrawWindow();
  	WinSetDrawWindow( m_hScreenBuffer );
  	WinEraseRectangle(&m_rBufferBounds, 0);
  	WinSetDrawWindow( m_hScreen );
  	DrawBufferToScreen();
  }  
}

void CTestz::SetScreenBuffer( bool bActive )
{
	if ( !m_hScreenBuffer ) return;						
	if ( bActive ) {
		WinSetCoordinateSystem( kCoordinatesNative );
		m_hScreen = WinGetDrawWindow();
		WinGetDrawWindowBounds( &m_rScreenBounds );
		WinSetDrawWindow( m_hScreenBuffer );				
	}
	else {
		WinSetDrawWindow( m_hScreen );
		WinSetCoordinateSystem( m_ui16CoordSys );
	}						
}

void CTestz::DrawBufferToScreen( void )
{
    if ( !m_hScreenBuffer ) return;	
    WinSetCoordinateSystem( kCoordinatesNative );
    
    if ( !m_hScreen ) {
    	m_hScreen = WinGetDrawWindow( );
    	WinGetDrawWindowBounds( &m_rScreenBounds );
    }
	WinCopyRectangle( m_hScreenBuffer, m_hScreen, &m_rBufferBounds, 
	                  m_rScreenBounds.topLeft.x, m_rScreenBounds.topLeft.y, winPaint );
	WinSetCoordinateSystem( m_ui16CoordSys );
}

void CTestz::SolidRectangle(int iX, int iY, int iWidth, int iHeight, PlutoColor color )
{
	SetScreenBuffer();
	
	WinPushDrawState();
	RectangleType rP;	
	rP.topLeft.x = iX;
	rP.topLeft.y = iY;
	rP.extent.x = iWidth;
	rP.extent.y = iHeight;

	RGBColorType prevRgb, rgbColor;
	PlutoColorToRGBColorType( rgbColor, color );
	WinSetForeColorRGB( &rgbColor, &prevRgb);

//  WinDrawRectangle always uses winPaint mode
//    for other modes use WinPaintRectangle ( which uses current drawing state )
	WinDrawRectangle( &rP, 0 );	// for round corners != 0
	
	WinPopDrawState();
	
	SetScreenBuffer( false );
	DrawBufferToScreen();
}

void CTestz::HollowRectangle(int X, int Y, int Width, int Height, PlutoColor color )
{
	SetScreenBuffer();
	
	WinPushDrawState();
	
	RectangleType rP;	
	rP.topLeft.x = X;
	rP.topLeft.y = Y;
	rP.extent.x = Width;
	rP.extent.y = Height;

	RGBColorType prevRgb, rgbColor;
	PlutoColorToRGBColorType( rgbColor, color );
	WinSetForeColorRGB( &rgbColor, &prevRgb);

//  WinDrawRectangleFrame always uses winPaint mode
//    for other modes use WinPaintRectangleFrame ( which uses current drawing state )
//  FrameType = ( noFrame, simpleFrame, rectangleFrame, simple3DFrame, roundFrame, 
//                boldRoundFrame, popupFrame, dialogFrame, menuFrame  )
	WinDrawRectangleFrame( simpleFrame, &rP );	// for round corners != 0
	
	WinPopDrawState();

	SetScreenBuffer( false );
	DrawBufferToScreen();
}


void CTestz::DrawLine(int iX, int iY, int iWidth, int iHeight, PlutoColor color )
{	
	SetScreenBuffer();
	
	WinPushDrawState();

	RGBColorType prevRgb, rgbColor;
	PlutoColorToRGBColorType( rgbColor, color );
	WinSetForeColorRGB( &rgbColor, &prevRgb);

//  WinDrawLine always uses winPaint mode
//    for other modes use WinPaintLine ( which uses current drawing state )
	WinDrawLine( iX, iY, iWidth, iHeight );	
	
	WinPopDrawState();	

	SetScreenBuffer( false );
	DrawBufferToScreen();
}

void CTestz::RenderText( string &sTextToDisplay, class TextStyle *pTextStyle, PlutoPoint point )
{
	SetScreenBuffer();
	
	WinPushDrawState();

	//RGBColorType prevRgb;
	//WinSetForeColorRGB( &rgbColor, &prevRgb);

    if ( pTextStyle ) {
    	if ( pTextStyle->m_bUnderline ) {
    		// UnderlineModeType = ( noUnderline, grayUnderline, solidUnderline, colorUnderline )
    		WinSetUnderlineMode( solidUnderline );
    	}
    	if ( pTextStyle->m_bBold ) {
    		FntSetFont( boldFont );
    	}
    	RGBColorType prevRgb, rgbColor;
    	PlutoColorToRGBColorType( rgbColor, pTextStyle->m_ForeColor );    	
    	WinSetForeColorRGB( &rgbColor, &prevRgb);
    	WinSetTextColorRGB( &rgbColor, &prevRgb);
    	PlutoColorToRGBColorType( rgbColor, pTextStyle->m_BackColor );    	
    	WinSetBackColorRGB( &rgbColor, &prevRgb);
    }
//  WinDrawChars always uses winPaint mode
//    for other modes use WinPaintChars ( which uses current drawing state )
	WinDrawChars( sTextToDisplay.c_str(), sTextToDisplay.length(), point.X, point.Y );	
	
	WinPopDrawState();	

	SetScreenBuffer( false );
	DrawBufferToScreen();
}
