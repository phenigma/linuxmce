#include "Orbiter_PocketFrog.h"
//-----------------------------------------------------------------------------------------------------
using namespace Frog;
CComModule _Module;
//-----------------------------------------------------------------------------------------------------
Orbiter_PocketFrog *Orbiter_PocketFrog::m_pInstance = NULL; //the one and only
//-----------------------------------------------------------------------------------------------------
Orbiter_PocketFrog::Orbiter_PocketFrog(int DeviceID, string ServerAddress, string sLocalDirectory, 
									   bool bLocalMode, int nImageWidth, int nImageHeight, 
									   bool bFullScreen) :
		Orbiter(DeviceID, ServerAddress, sLocalDirectory, bLocalMode, nImageWidth, nImageHeight)
{
    m_config.szWindowName     = _T("Orbiter CE (Pocket Frog)");
    m_config.orientation      = ORIENTATION_WEST;
    m_config.splashScreenTime = 0;	

	m_bNeedToUpdate = false;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ Orbiter_PocketFrog::~Orbiter_PocketFrog()
{

}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ bool Orbiter_PocketFrog::GameInit()
{
	m_display = GetDisplay();

	Surface* pLogoSurface;
	pLogoSurface = LoadImage( m_display, TEXT("\\Storage Card\\logo.gif") );
	m_display->Blit( 0, 0, pLogoSurface );
	delete pLogoSurface;
    m_display->Update();

	Initialize(gtPocketFrogGraphic);

	if(m_bQuit)
		return false;

	if(m_bReload)
		return false;

	if (!m_bLocalMode)
		CreateChildren();

	return true;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::GameEnd()
{

}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::GameLoop()
{
	if(m_bNeedToUpdate)
	{
		m_bNeedToUpdate = false;
		m_display->Update();
	}
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::StylusDown( Point stylus )
{
	RegionDown(stylus.x, stylus.y);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::StylusUp( Point stylus )
{
	RegionUp(stylus.x, stylus.y);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::StylusMove( Point stylus )
{
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::SolidRectangle(int x, int y, int width, int height, PlutoColor color, int Opacity)
{

}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::HollowRectangle(int X, int Y, int Width, int Height, PlutoColor color)
{

}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::ReplaceColorInRectangle(int x, int y, int width, int height, PlutoColor ColorToReplace, PlutoColor ReplacementColor)
{

}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::RenderText(class DesignObjText *Text,class TextStyle *pTextStyle)
{

}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::SaveBackgroundForDeselect(DesignObj_Orbiter *pObj)
{

}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ PlutoGraphic *Orbiter_PocketFrog::CreateGraphic()
{
	return new PocketFrogGraphic(this);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::RenderScreen()
{
	g_pPlutoLogger->Write(LV_STATUS,"$$$ RENDER SCREEN $$$ %s",(m_pScreenHistory_Current ? m_pScreenHistory_Current->m_pObj->m_ObjectID.c_str() : " NO SCREEN"));

    if (m_pScreenHistory_Current)
    {
        PLUTO_SAFETY_LOCK(cm, m_ScreenMutex);
		//SDL_FillRect(m_pScreenImage, NULL, SDL_MapRGBA(m_pScreenImage->format, 0, 0, 0, 255));
    }

    Orbiter::RenderScreen();
    
	m_bNeedToUpdate = true;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::RedrawObjects()
{
    Orbiter::RedrawObjects();
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::RenderGraphic(class PlutoGraphic *pPlutoGraphic, PlutoRectangle rectTotal, bool bDisableAspectRatio)
{
	if(!pPlutoGraphic || pPlutoGraphic->GraphicType_get() != gtPocketFrogGraphic)
		return;//nothing to render or not an pocket frog graphic

	if(pPlutoGraphic->IsEmpty())
		return;

	PocketFrogGraphic *pPocketFrogGraphic = (PocketFrogGraphic *) pPlutoGraphic;
	Surface *pSurface = pPocketFrogGraphic->m_pSurface;
	m_display->Blit( rectTotal.X, rectTotal.Y, pSurface );
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::BeginPaint()
{

}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::EndPaint()
{

}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::UpdateRect(PlutoRectangle rect)
{

}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::OnQuit()
{

}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::Initialize(GraphicType Type, int iPK_Room, int iPK_EntertainArea)
{
	Orbiter::Initialize(Type, iPK_Room, iPK_EntertainArea);
}
//-----------------------------------------------------------------------------------------------------
/*static*/ void Orbiter_PocketFrog::Cleanup()
{
	g_pPlutoLogger->Write(LV_STATUS, "Orbiter_PocketFrog: need to cleanup orbiter...");
	if(NULL != m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;

		g_pPlutoLogger->Write(LV_STATUS, "Orbiter_PocketFrog: orbiter deleted.");
	}
}
//-----------------------------------------------------------------------------------------------------
/*static*/ void Orbiter_PocketFrog::BuildOrbiter(int DeviceID, string ServerAddress, string sLocalDirectory, bool bLocalMode, 
	int nImageWidth, int nImageHeight, bool bFullScreen)
{
	if(NULL == m_pInstance)
	{
		g_pPlutoLogger->Write(LV_STATUS, "Orbiter_PocketFrog constructor.");
		m_pInstance = new Orbiter_PocketFrog(DeviceID, ServerAddress, 
				sLocalDirectory, bLocalMode, nImageWidth, 
				nImageHeight, bFullScreen);
	}
	else
	{
		throw "Orbiter_PocketFrog already created!";
	}
}
//-----------------------------------------------------------------------------------------------------
/*static*/ Orbiter_PocketFrog *Orbiter_PocketFrog::GetInstance()
{
	if(!m_pInstance)
		g_pPlutoLogger->Write(LV_STATUS, "Orbiter_PocketFrog:GetInstance The instance to orbiter is NULL");

	return m_pInstance;
}
//-----------------------------------------------------------------------------------------------------
void Orbiter_PocketFrog::WriteStatusOutput(const char* pMessage)
{
}
//-----------------------------------------------------------------------------------------------------