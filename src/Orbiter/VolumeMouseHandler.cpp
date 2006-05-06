#include "VolumeMouseHandler.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/ProcessUtils.h"
#include "PlutoUtils/Other.h"
#include "MouseGovernor.h"
#include "MouseIterator.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "pluto_main/Define_Button.h"
#include "pluto_main/Define_DesignObj.h"

#ifndef WIN32
#include <SDL_image.h>
#include "SDL/OrbiterSDL.h"
#include "SDL/SDLGraphic.h"
#include "Linux/OrbiterLinux.h"
#endif

using namespace DCE;

VolumeMouseHandler::VolumeMouseHandler(DesignObj_Orbiter *pObj,string sOptions,MouseBehavior *pMouseBehavior)
        : MouseHandler(pObj,sOptions,pMouseBehavior)
        //, m_pPrevSurface(NULL)
{
}

VolumeMouseHandler::~VolumeMouseHandler()
{
    //if (m_pPrevSurface)
    //    delete m_pPrevSurface;
}

void VolumeMouseHandler::Start()
{
	if( m_pMouseBehavior->m_pMouseHandler_Vertical && m_pMouseBehavior->m_pMouseHandler_Vertical->m_pObj )
	{
		NeedToRender render( m_pMouseBehavior->m_pOrbiter, "start volume" );
		m_pMouseBehavior->m_bMouseHandler_Horizontal_Exclusive = true;
		m_pMouseBehavior->m_pMouseHandler_Vertical->m_pObj->m_bHidden = true;
		m_pMouseBehavior->m_pOrbiter->RenderObjectAsync(m_pMouseBehavior->m_pMouseHandler_Vertical->m_pObj);// Redraw even if the object was already in this state,  because maybe we're hiding this and something that
		if( m_pMouseBehavior->m_pMouseHandler_Vertical->m_pObj->m_pParentObject )
			m_pMouseBehavior->m_pOrbiter->RenderObjectAsync((DesignObj_Orbiter *) m_pMouseBehavior->m_pMouseHandler_Vertical->m_pObj->m_pParentObject);
	}
m_pMouseBehavior->m_pOrbiter->m_bPK_Device_NowPlaying_Audio_DiscreteVolume=true;
	if( m_pMouseBehavior->m_pOrbiter->m_bPK_Device_NowPlaying_Audio_DiscreteVolume==false || m_pMouseBehavior->m_iTime_Last_Mouse_Up )
	{
		m_bTapAndRelease=true;
		m_pObj->m_GraphicToDisplay=GRAPHIC_NORMAL;
		int X=m_pObj->m_rPosition.X+m_pObj->m_pPopupPoint.X+m_pObj->m_rPosition.Width/2;
		m_iLastGoodPosition=X;
		m_pMouseBehavior->SetMousePosition(X,m_pObj->m_rPosition.Y+m_pObj->m_pPopupPoint.Y+m_pObj->m_rPosition.Height/2);
		m_iLastNotch = 0;
	}
	else
	{
		m_pMouseBehavior->m_pMouseGovernor->SetBuffer(250);
		m_bTapAndRelease=false;
		m_pObj->m_GraphicToDisplay=1;
		m_iCancelLevel = m_iLastNotch = atoi(m_pMouseBehavior->m_pOrbiter->GetEvents()->GetDeviceDataFromDatabase(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying_Audio,DEVICEDATA_Volume_Level_CONST).c_str());
		int X = m_pObj->m_rPosition.Width * m_iLastNotch / 100;
		m_iLastGoodPosition=X;
NeedToRender render( m_pMouseBehavior->m_pOrbiter, "change to discrete volume" );
PLUTO_SAFETY_LOCK(nd,m_pMouseBehavior->m_pOrbiter->m_NeedRedrawVarMutex);
m_pMouseBehavior->m_pOrbiter->m_vectObjs_NeedRedraw.push_back(m_pObj);
		m_pMouseBehavior->SetMousePosition(m_pObj->m_rPosition.X+m_pObj->m_pPopupPoint.X+X,m_pObj->m_rPosition.Y+m_pObj->m_pPopupPoint.Y+m_pObj->m_rPosition.Height/2);
	}
	DrawSquare(m_iLastNotch,PlutoColor::White());
    // NEW CODE, try Green
	//DrawSquare(m_iLastNotch,PlutoColor::Green());
}

void VolumeMouseHandler::Stop()
{
NeedToRender render( m_pMouseBehavior->m_pOrbiter, "change to discrete volume" );
PLUTO_SAFETY_LOCK(nd,m_pMouseBehavior->m_pOrbiter->m_NeedRedrawVarMutex);
m_pMouseBehavior->m_pOrbiter->m_vectObjs_NeedRedraw.push_back(m_pObj);
g_pPlutoLogger->Write(LV_CORPCLIENT,"m_pObj.disabled = true");
}

bool VolumeMouseHandler::ButtonDown(int PK_Button)
{
	if( PK_Button==BUTTON_Mouse_1_CONST || PK_Button==BUTTON_Mouse_8_CONST )
		m_pMouseBehavior->Clear(true); // this will be deleted
	return false; // Keep processing
}

bool VolumeMouseHandler::ButtonUp(int PK_Button)
{
	if( PK_Button==BUTTON_Mouse_2_CONST )
	{
		if( !m_bTapAndRelease )
		{
			DCE::CMD_Set_Volume CMD_Set_Volume(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying_Audio,StringUtils::itos(m_iCancelLevel));
			m_pMouseBehavior->m_pOrbiter->SendMessage(CMD_Set_Volume.m_pMessage);
		}
		m_pMouseBehavior->Clear(true); // this will be deleted
		return false; // Keep processing
	}
	else if( PK_Button==BUTTON_Mouse_8_CONST && !m_bTapAndRelease )
	{
		m_pMouseBehavior->Clear(true); // this will be deleted
		return false; // Keep processing

	}
	return false; // Keep processing
}

void VolumeMouseHandler::Move(int X,int Y,int PK_Direction)
{
	m_iLastGoodPosition=X;
	if( m_bTapAndRelease )
	{
		int NotchWidth = m_pObj->m_rPosition.Width/15; // Allow for 7 repeat levels in each direction
		int Notch = (X-m_pObj->m_rPosition.X-m_pObj->m_pPopupPoint.X)/NotchWidth - 7;
		if( Notch<-7 )
			Notch=-7;
		if( Notch>7 )
			Notch=7;
		if( Notch!=m_iLastNotch )
		{
			DrawSquare(m_iLastNotch,PlutoColor::Blue());
			m_pMouseBehavior->m_pMouseIterator->SetIterator(MouseIterator::if_Volume,Notch,500,this);
			m_iLastNotch = Notch;
			DrawSquare(m_iLastNotch,PlutoColor::White());
		}
	}
	else
	{
		int NotchWidth = m_pObj->m_rPosition.Width/100; // Allow for 7 repeat levels in each direction
		int Notch = (X - m_pObj->m_rPosition.X - m_pObj->m_pPopupPoint.X) * 100 / m_pObj->m_rPosition.Width;
		if( Notch<0 )
			Notch=0;
		if( Notch>100 )
			Notch=100;
		if( Notch!=m_iLastNotch )
		{
			DrawSquare(m_iLastNotch,PlutoColor::Blue());
			DCE::CMD_Set_Volume CMD_Set_Volume(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying_Audio,StringUtils::itos(Notch));
			m_pMouseBehavior->m_pMouseGovernor->SendMessage(CMD_Set_Volume.m_pMessage);
			m_iLastNotch = Notch;
			DrawSquare(m_iLastNotch,PlutoColor::White());
		}
	}
}

bool VolumeMouseHandler::SlowDrift(int &X,int &Y)
{
	X = m_iLastGoodPosition;
	Y = m_pObj->m_rPosition.Y + m_pObj->m_pPopupPoint.Y + (m_pObj->m_rPosition.Height/2);
	m_pMouseBehavior->SetMousePosition(X,Y);
	return true;
}

void VolumeMouseHandler::DrawSquare(int Notch, PlutoColor plutoColor)
{
#ifndef WIN32
	g_pPlutoLogger->Write(LV_CRITICAL,"VolumeMouseHandler::DrawSquare() : Notch=%d", Notch);

    OrbiterLinux *pOrbiterLinux = dynamic_cast<OrbiterLinux *>(m_pMouseBehavior->m_pOrbiter);
    if (pOrbiterLinux)
        pOrbiterLinux->X_LockDisplay();

    int NotchWidth = m_bTapAndRelease==false ? m_pObj->m_rPosition.Width/100 : m_pObj->m_rPosition.Width/15; // Allow for 7 repeat levels in each direction
	int NotchStart = (7 + Notch) * NotchWidth + m_pObj->m_rPosition.X + m_pObj->m_pPopupPoint.X;

    // TODO: remove these notes
    //{
    //DesignObj *pObj = m_pOrbiter->FindObject();
    //m_pOrbiter->RenderGraphic(pObj,  pObj->m_rPosition, pObj->m_bDisableAspectLock);

    //redraw screen
    //PlutoRectangle rect_screen(0, 0, m_pMouseBehavior->m_pOrbiter->m_iImageWidth, m_pMouseBehavior->m_pOrbiter->m_iImageHeight);
    //m_pMouseBehavior->m_pOrbiter->UpdateRect(rect_screen);

    //plutoColor.A(50);
    //}

    // ORIGINAL CODE
	m_pMouseBehavior->m_pOrbiter->HollowRectangle(
		NotchStart, m_pObj->m_rPosition.Y + m_pObj->m_pPopupPoint.Y,
		NotchWidth, m_pObj->m_rPosition.Height,
		plutoColor);
    m_pMouseBehavior->m_pOrbiter->UpdateRect(m_pObj->m_rPosition + m_pObj->m_pPopupPoint);

    // ORIGINAL CODE, changed to solid rectangle
	//m_pMouseBehavior->m_pOrbiter->SolidRectangle(
	//	NotchStart, m_pObj->m_rPosition.Y + m_pObj->m_pPopupPoint.Y,
	//	NotchWidth, m_pObj->m_rPosition.Height,
	//	plutoColor);
    //m_pMouseBehavior->m_pOrbiter->UpdateRect(m_pObj->m_rPosition + m_pObj->m_pPopupPoint);

    // NEW CODE, only transparency works fine
    //{
    //    PlutoRectangle rectFullImage(
    //        m_pObj->m_rPosition.X + m_pObj->m_pPopupPoint.X,
    //        m_pObj->m_rPosition.Y + m_pObj->m_pPopupPoint.Y,
    //        m_pObj->m_rPosition.Width,
    //        m_pObj->m_rPosition.Height
    //        );

    //    SDL_Rect rectFullImageSDL;
    //    rectFullImageSDL.x = rectFullImage.X;
    //    rectFullImageSDL.y = rectFullImage.Y;
    //    rectFullImageSDL.w = rectFullImage.Width;
    //    rectFullImageSDL.h = rectFullImage.Height;

    //    OrbiterSDL *pOrbiterSDL = dynamic_cast<OrbiterSDL *>(m_pMouseBehavior->m_pOrbiter);
    //    if (pOrbiterSDL == NULL)
    //        return;

    //    int x =
    //        rectFullImage.X;
    //    //NotchStart;
    //    int width =
    //        x;
    //    x + NotchStart;
    //    //NotchWidth;
    //    int height =
    //        m_pObj->m_rPosition.Height
    //        / 3;
    //    int y =
    //        m_pObj->m_rPosition.Y + m_pObj->m_pPopupPoint.Y
    //        + m_pObj->m_rPosition.Height - height;

    //    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
    //    const Uint32 rmask = 0xff000000, gmask = 0x00ff0000, bmask = 0x0000ff00, amask = 0x000000ff;
    //    #else
    //    const Uint32 rmask = 0x000000ff, gmask = 0x0000ff00, bmask = 0x00ff0000, amask = 0xff000000;
    //    #endif

    //    // save previous surface
    //    if (m_pPrevSurface == NULL)
    //    {
    //        //PlutoGraphic *pPlutoGraphic = pOrbiterSDL->GetBackground(rectFullImage);
    //        //m_pPrevSurface = dynamic_cast<SDLGraphic *>(pPlutoGraphic);
    //        //if (m_pPrevSurface == NULL)
    //        //    delete pPlutoGraphic;
    //        m_pPrevSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32, rmask, gmask, bmask, amask);
    //        SDL_BlitSurface(pOrbiterSDL->m_pScreenImage, &rectFullImageSDL, m_pPrevSurface, NULL);
    //    }

    //    // create a transparent rectangle, with the given color
    //    SDL_Surface * pSDL_Bar = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA, width, height, 32, rmask, gmask, bmask, amask);
    //    SDL_Rect rectBar;
    //    rectBar.x = 0;
    //    rectBar.y = 0;
    //    rectBar.w = width;
    //    rectBar.h = height;
    //    SDL_FillRect(pSDL_Bar, &rectBar, SDL_MapRGBA(pSDL_Bar->format, plutoColor.R(), plutoColor.G(), plutoColor.B(), 128));

    //    // prepare for redrawing
    //    rectBar.x = x;
    //    rectBar.y = y;

    //    SDL_SetAlpha(pOrbiterSDL->m_pScreenImage, 0, 0);
    //    SDL_SetAlpha(m_pPrevSurface, 0, 0);

    //    // copy the old surface
    //    SDL_BlitSurface(m_pPrevSurface, NULL, pOrbiterSDL->m_pScreenImage, &rectFullImageSDL);

    //    // combine with the new surface
    //    SDL_BlitSurface(pSDL_Bar, NULL, pOrbiterSDL->m_pScreenImage, &rectBar);

    //    SDL_Flip(pOrbiterSDL->m_pScreenImage);

    //    // done
    //    SDL_FreeSurface(pSDL_Bar);
    //    m_pMouseBehavior->m_pOrbiter->UpdateRect(m_pObj->m_rPosition + m_pObj->m_pPopupPoint);
    //}

    if (pOrbiterLinux)
        pOrbiterLinux->X_UnlockDisplay();
#endif
}
