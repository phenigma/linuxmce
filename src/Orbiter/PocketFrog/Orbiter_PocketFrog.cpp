#include "Orbiter_PocketFrog.h"
#include "MainDialog.h"

#include "../pluto_main/Define_Button.h"
#include "../pluto_main/Define_Direction.h" 
//-----------------------------------------------------------------------------------------------------
#define VK_A 0x41
#define VK_C 0x43
#define VK_E 0x45
#define VK_T 0x54
#define VK_P 0x50
#define VK_L 0x4C
#define VK_R 0x52
#define VK_Z 0x5A
//-----------------------------------------------------------------------------------------------------
#define RED_MASK_16		(0x1F << 11)
#define GREEN_MASK_16	(0x3F << 5)
#define BLUE_MASK_16	0x1F
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
	Surface* pLogoSurface;
	pLogoSurface = LoadImage( GetDisplay(), TEXT("\\Storage Card\\logo.gif") );
	GetDisplay()->Blit( 0, 0, pLogoSurface );
	delete pLogoSurface;
    GetDisplay()->Update();

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
		GetDisplay()->Update();
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
/*virtual*/ bool Orbiter_PocketFrog::PocketFrogButtonDown(int button)
{
	HandleKeyEvents(WM_KEYDOWN, button, 0L);
	return true;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ bool Orbiter_PocketFrog::PocketFrogButtonUp(int button)
{
	HandleKeyEvents(WM_KEYUP, button, 0L);
	return true;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::HandleKeyEvents(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static WPARAM wOldParam = 0;
	static bool bLastEvent_KeyDown = false;

	if(uMsg == WM_KEYDOWN && bLastEvent_KeyDown && wOldParam == wParam) //this is a repeated key
		return;

	wOldParam = wParam;
	bLastEvent_KeyDown = uMsg == WM_KEYDOWN; //false if WM_KEYUP

    if (uMsg == WM_KEYDOWN)
    {
		if ( wParam == 81)
		{
			OnQuit();
			return;
		}

        g_pPlutoLogger->Write(LV_STATUS, "Key pressed event");
		switch (wParam)
        {
			case VK_SHIFT:
				m_bShiftDown=true;
				break;
			case VK_CONTROL:
				m_bControlDown=true;
				break;
			case VK_MENU:
				m_bAltDown=true;
				break;
			case VK_CAPITAL:
				m_bCapsLock = !m_bCapsLock;
				break;
			default:
				m_cKeyDown = clock();  // We don't care how long the shift, ctrl or alt are held down, but the other keys do matter
				break;
        }
    }
    else if (uMsg == WM_KEYUP)
    {
		RecordKeyboardAction(long(wParam));

        bool bHandled=false;
clock_t ccc=clock();
        m_bRepeat = m_cKeyDown && clock() - m_cKeyDown > CLOCKS_PER_SEC/2;
        m_cKeyDown=0;

        g_pPlutoLogger->Write(LV_STATUS, "key up %d  rept: %d  shif: %d",(int) wParam, (int) m_bRepeat, (int) m_bShiftDown);

#ifndef PHONEKEYS
        if(wParam >= VK_A && wParam <= VK_Z) // A-Z keys
        {
            if((!m_bCapsLock && !m_bShiftDown) || (m_bCapsLock && m_bShiftDown))
                bHandled = Orbiter::ButtonDown(BUTTON_a_CONST + wParam - VK_A);
            else
                bHandled = Orbiter::ButtonDown(BUTTON_A_CONST + wParam - VK_Z);
        }
        else
#endif 
        if( wParam == VK_SHIFT)
            m_bShiftDown=false;
        else if( wParam == VK_CONTROL)
            m_bControlDown=false;
        else if( wParam == VK_MENU )
            m_bAltDown=false;
		else if( wParam == VK_ESCAPE && m_bControlDown && m_bAltDown)
		{
			OnQuit();
		}
		else if( wParam == VK_F10)
		{
			ShowMainDialog();
		}
        else if( !m_bShiftDown && !m_bControlDown && !m_bAltDown && !m_bRepeat )
        {
            switch (wParam)
            {
				case '0':
					bHandled=Orbiter::ButtonDown(BUTTON_0_CONST);
					break;
				case '1':
					bHandled=Orbiter::ButtonDown(BUTTON_1_CONST);
					break;
				case '2':
					bHandled=Orbiter::ButtonDown(BUTTON_2_CONST);
					break;
				case '3':
					bHandled=Orbiter::ButtonDown(BUTTON_3_CONST);
					break;
				case '4':
					bHandled=Orbiter::ButtonDown(BUTTON_4_CONST);
					break;
				case '5':
					bHandled=Orbiter::ButtonDown(BUTTON_5_CONST);
					break;
				case '6':
					bHandled=Orbiter::ButtonDown(BUTTON_6_CONST);
					break;
				case '7':
					bHandled=Orbiter::ButtonDown(BUTTON_7_CONST);
					break;
				case '8':
					bHandled=Orbiter::ButtonDown(BUTTON_8_CONST);
					break;
				case '9':
					bHandled=Orbiter::ButtonDown(BUTTON_9_CONST);
					break;

#ifdef PHONEKEYS
                case VK_C:
                    bHandled = Orbiter::ButtonDown(BUTTON_Phone_C_CONST);
                    break;
                case VK_P:
                    bHandled = Orbiter::ButtonDown(BUTTON_Phone_Pencil_CONST);
                    break;
                case VK_T:
                    bHandled = Orbiter::ButtonDown(BUTTON_Phone_Talk_CONST);
                    break;
                case VK_E:
                    bHandled = Orbiter::ButtonDown(BUTTON_Phone_End_CONST);
                    break;
                case VK_L:
                    bHandled = Orbiter::ButtonDown(BUTTON_Phone_Soft_left_CONST);
                    break;
                case VK_R:
                    bHandled = Orbiter::ButtonDown(BUTTON_Phone_Soft_right_CONST);
                    break;
                case '*':
                    bHandled = Orbiter::ButtonDown(BUTTON_Asterisk_CONST);
                    break;
				case '#':
                    bHandled = Orbiter::ButtonDown(BUTTON_Pound_CONST);
                    break;
#endif 

				case VK_F1:
					bHandled=Orbiter::ButtonDown(BUTTON_F1_CONST);
					break;
				case VK_F2:
					bHandled=Orbiter::ButtonDown(BUTTON_F2_CONST);
					break;
				case VK_F3:
					bHandled=Orbiter::ButtonDown(BUTTON_F3_CONST);
					break;
				case VK_F4:
					bHandled=Orbiter::ButtonDown(BUTTON_F4_CONST);
					break;
				case VK_F5:
					bHandled=Orbiter::ButtonDown(BUTTON_F5_CONST);
					break;
				case VK_UP:
					bHandled=Orbiter::ButtonDown(BUTTON_Up_Arrow_CONST);
					break;
				case VK_DOWN:
					bHandled=Orbiter::ButtonDown(BUTTON_Down_Arrow_CONST);
					break;
				case VK_LEFT:
					bHandled=Orbiter::ButtonDown(BUTTON_Left_Arrow_CONST);
					break;
				case VK_RIGHT:
					bHandled=Orbiter::ButtonDown(BUTTON_Right_Arrow_CONST);
					break;
				case VK_RETURN:
					bHandled=Orbiter::ButtonDown(BUTTON_Enter_CONST);
					break;
				case VK_BACK:
					bHandled=Orbiter::ButtonDown(BUTTON_Back_CONST);
					break;
				default:
					g_pPlutoLogger->Write(LV_STATUS, "Unknown key: %d", (int) wParam);
            };
        } 
        else if( m_bShiftDown && !m_bControlDown && !m_bAltDown && !m_bRepeat )
        {
            switch (wParam)
            {
				case VK_UP:
					bHandled=Orbiter::ButtonDown(BUTTON_Shift_Up_Arrow_CONST);
					break;
				case VK_DOWN:
					bHandled=Orbiter::ButtonDown(BUTTON_Shift_Down_Arrow_CONST);
					break;
				case VK_LEFT:
					bHandled=Orbiter::ButtonDown(BUTTON_Shift_Left_Arrow_CONST);
					break;
				case VK_RIGHT:
					bHandled=Orbiter::ButtonDown(BUTTON_Shift_Right_Arrow_CONST);
					break;
				case '3':
					bHandled=Orbiter::ButtonDown(BUTTON_Pound_CONST);
					break;
				case '8':
					bHandled=Orbiter::ButtonDown(BUTTON_Asterisk_CONST);
					break;
            };
        }
		else if( m_bShiftDown && m_bControlDown && wParam==VK_F1 )
			GotoScreen(m_sMainMenu);
        else if( m_bRepeat )
        {
            switch (wParam)
            {
				case '0':
					bHandled=Orbiter::ButtonDown(BUTTON_Rept_0_CONST);
					break;
				case '1':
					bHandled=Orbiter::ButtonDown(BUTTON_Rept_1_CONST);
					break;
				case '2':
					bHandled=Orbiter::ButtonDown(BUTTON_Rept_2_CONST);
					break;
				case '3':
					bHandled=Orbiter::ButtonDown(BUTTON_Rept_3_CONST);
					break;
				case '4':
					bHandled=Orbiter::ButtonDown(BUTTON_Rept_4_CONST);
					break;
				case '5':
					bHandled=Orbiter::ButtonDown(BUTTON_Rept_5_CONST);
					break;
				case '6':
					bHandled=Orbiter::ButtonDown(BUTTON_Rept_6_CONST);
					break;
				case '7':
					bHandled=Orbiter::ButtonDown(BUTTON_Rept_7_CONST);
					break;
				case '8':
					bHandled=Orbiter::ButtonDown(BUTTON_Rept_8_CONST);
					break;
				case '9':
					bHandled=Orbiter::ButtonDown(BUTTON_Rept_9_CONST);
					break;
#ifdef PHONEKEYS
                case VK_C:
                    bHandled = Orbiter::ButtonDown(BUTTON_Rept_Phone_C_CONST);
                    break;
                case VK_P:
                    bHandled = Orbiter::ButtonDown(BUTTON_Rept_Phone_Pencil_CONST);
                    break;
                case VK_T:
                    bHandled = Orbiter::ButtonDown(BUTTON_Rept_Phone_Talk_CONST);
                    break;
                case VK_E:
                    bHandled = Orbiter::ButtonDown(BUTTON_Rept_Phone_End_CONST);
                    break;
                case VK_L:
                    bHandled = Orbiter::ButtonDown(BUTTON_Rept_Phone_Soft_left_CONST);
                    break;
                case VK_R:
                    bHandled = Orbiter::ButtonDown(BUTTON_Rept_Phone_Soft_right_CONST);
                    break;
                case '*':
                    bHandled = Orbiter::ButtonDown(BUTTON_Rept_Asterisk_CONST);
                    break;
				case '#':
                    bHandled = Orbiter::ButtonDown(BUTTON_Rept_Pound_CONST);
                    break;
#endif 
			}
		}
	}
}
//-----------------------------------------------------------------------------------------------------
/*static inline*/ Pixel Orbiter_PocketFrog::GetColor16(PlutoColor color)
{
	return (Pixel)(((color.R() << 8) & RED_MASK_16) | ((color.G() << 3) & GREEN_MASK_16) | (color.B() >> 3));		

}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::SolidRectangle(int x, int y, int width, int height, PlutoColor color, int Opacity)
{
	g_pPlutoLogger->Write(LV_STATUS, "Solid rectangle, %d, %d, %d, %d", x, y, width, height);
	//GetDisplay()->FillRect(x, y, width, height, GetColor16(color));
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::HollowRectangle(int X, int Y, int Width, int Height, PlutoColor color)
{
	//TODO:
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
	GetDisplay()->Blit( rectTotal.X, rectTotal.Y, pSurface );
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::BeginPaint()
{

}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::EndPaint()
{
	m_bNeedToUpdate = true;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::UpdateRect(PlutoRectangle rect)
{
	//m_bNeedToUpdate = true;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::OnQuit()
{
	Shutdown();
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
	//
}
//-----------------------------------------------------------------------------------------------------
