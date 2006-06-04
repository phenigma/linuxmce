#include "MouseBehavior.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/ProcessUtils.h"
#include "PlutoUtils/Other.h"
#include "MouseGovernor.h"
#include "MouseIterator.h"
#include "ScreenHistory.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "pluto_main/Define_Button.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_Screen.h"

#include "Simulator.h"
#include "OpenGL/Orbiter3DCommons.h"

#include "HorizMenuMouseHandler.h"
#include "SpeedMouseHandler.h"
#include "LightMouseHandler.h"
#include "VolumeMouseHandler.h"
#include "MediaMouseHandler.h"
#include "LockedMouseHandler.h"
#include "KeyboardMouseHandler.h"
#include "OrbiterRenderer.h"

#include "Linux/OSDCompass.h"

using namespace DCE;

/*
#include <SDL_image.h>
#include "SDL/OrbiterSDL.h"
#include "SDL/SDLGraphic.h"
#include "SDL/PlutoSDLDefs.h"
*/

//-----------------------------------------------------------------------------------------------------
MouseBehavior::MouseBehavior(Orbiter *pOrbiter)
{
	//TODO: set custom position for the compass
	m_spCompass.reset(new OSDCompass(pOrbiter, PlutoRectangle(10, 10, 100, 100)));

	m_pMouseHandler=NULL;
	ProcessUtils::ResetMsTime();
	m_pOrbiter=pOrbiter;
	m_pMouseGovernor = new MouseGovernor(this);
	m_pMouseIterator = new MouseIterator(this);
	m_iPK_Button_Mouse_Last=0;
    m_iTime_Last_Mouse_Down=m_iTime_Last_Mouse_Up=0;
	Clear();
}

MouseBehavior::~MouseBehavior()
{
	delete m_pMouseGovernor;
	delete m_pMouseIterator;
}

void MouseBehavior::Clear(bool bGotoMainMenu)
{
	PLUTO_SAFETY_LOCK(mb,m_pOrbiter->m_ScreenMutex);

	/* there's a logic flaw that when popups are put on screen they're added to the following
	vect's, but not removed when the popup is removed, although they are marked as m_bOnScreen=false.
	These were created as vect's because the concept of popups didn't exist in the beginning.
	Perhaps they should be changed to something else that's easier to remove entries from.
	For now, with the new UI, we know that whenever the mouse behavior is reset we will
	have no grids or tab stops on screen.  Reset them so the list doesn't grow indefinately. */
//	m_pOrbiter->m_vectObjs_GridsOnScreen.clear();
//	m_pOrbiter->m_vectObjs_TabStops.clear();

	m_pObj_Locked=NULL;
	ResetSamples();
	ConstrainMouse(PlutoRectangle(0,0,0,0));
	m_pLastPosition.X=m_pLastPosition.Y=-1;
	delete m_pMouseHandler;
	m_pMouseHandler=NULL;
	m_pMouseGovernor->SetBuffer(0);
	m_pMouseIterator->SetIterator(MouseIterator::if_None,0,0,NULL);
	m_dwTime_Last_Notch=0;
	m_dwLastSampleShift=0;

	if( bGotoMainMenu )
	{
		NeedToRender render( m_pOrbiter, "start speed" );
		if( m_pOrbiter->m_bIsOSD && m_pOrbiter->m_iPK_Screen_RemoteOSD && m_pOrbiter->m_iLocation_Initial==m_pOrbiter->m_pLocationInfo->iLocation)  // If we've changed locations, we're not the OSD anymore
			m_pOrbiter->CMD_Goto_Screen("",m_pOrbiter->m_iPK_Screen_RemoteOSD);
		else if( m_pOrbiter->m_iPK_Screen_Remote )
			m_pOrbiter->CMD_Goto_Screen("",m_pOrbiter->m_iPK_Screen_Remote);
		else
			m_pOrbiter->CMD_Goto_Screen("",SCREEN_Main_CONST);
	}
}

// If options contains a 's', this will be selected by default
void MouseBehavior::Set_Mouse_Behavior(string sOptions,bool bExclusive,string sDirection,string sDesignObj)
{
	PLUTO_SAFETY_LOCK(mb,m_pOrbiter->m_ScreenMutex);
g_pPlutoLogger->Write(LV_FESTIVAL,"MouseBehavior::Set_Mouse_Behavior -%s- %d -%s- -%s-",
					  sOptions.c_str(),(int) bExclusive,sDirection.c_str(),sDesignObj.c_str());
	if( sOptions.size()==0 )
	{
		Clear();
		return; 
	}

	DesignObj_Orbiter *pObj=NULL;
	if( sDesignObj.size() )
		pObj = m_pOrbiter->FindObject(sDesignObj);

	if( !pObj && sOptions[0] !='K' )  // Only the keyboard handler does not require an object
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"MouseBehavior::Set_Mouse_Behavior options %s, obj %s invalid",sOptions.c_str(),sDesignObj.c_str());
		return;
	}
	
	if( m_pMouseHandler )
		delete m_pMouseHandler;
	m_pMouseHandler=NULL;

	switch(sOptions[0])
	{
	case 'L':
	case 'M':
		m_pMouseHandler=new LockedMouseHandler(pObj,sOptions,this);
		break;
	case 'H':
		m_pMouseHandler=new HorizMenuMouseHandler(pObj,sOptions,this);
		break;
	case 'S':
		m_pMouseHandler=new SpeedMouseHandler(pObj,sOptions,this);
		break;
	case 'G':
		m_pMouseHandler=new LightMouseHandler(pObj,sOptions,this);
		break;
	case 'V':
		m_pMouseHandler=new VolumeMouseHandler(pObj,sOptions,this);
		break;
	case 'T':
		m_pMouseHandler=new MediaMouseHandler(pObj,sOptions,this);
		break;
	case 'K':
		m_pMouseHandler=new KeyboardMouseHandler(pObj,sOptions,this);
		break;
	}
	m_pObj_Locked=pObj;
	if( m_pMouseHandler )
		m_pMouseHandler->Start();
}

void MouseBehavior::Move(int X,int Y)
{
	PLUTO_SAFETY_LOCK(mb,m_pOrbiter->m_ScreenMutex);
	if( m_pMouseHandler )
		m_pMouseHandler->Move(X,Y,0);

	if(Simulator::GetInstance()->m_bUseOpenGL)
		Commons3D::Instance().SetMousePosition(X, Y);
}

DesignObj_Orbiter *MouseBehavior::FindChildObjectAtPosition(DesignObj_Orbiter *pObj_Parent,int X,int Y)
{
	if( pObj_Parent==NULL )
		pObj_Parent = m_pOrbiter->m_pScreenHistory_Current->GetObj();
	DesignObj_DataList::iterator iHao;
	for( iHao=pObj_Parent->m_ChildObjects.begin(  ); iHao != pObj_Parent->m_ChildObjects.end(  ); ++iHao )
	{
		DesignObj_Orbiter *pObj = (DesignObj_Orbiter *) *iHao;
		DesignObj_Orbiter *pObj_Child = FindChildObjectAtPosition(pObj,X,Y);
		if( pObj_Child )
			return pObj_Child;
	}
	if( pObj_Parent->m_bTabStop )
	{
		if( Y==-1 )
		{
			if( (pObj_Parent->m_rPosition+pObj_Parent->m_pPopupPoint).X<=X && (pObj_Parent->m_rPosition+pObj_Parent->m_pPopupPoint).Right()>=X )
				return pObj_Parent;
		}
		else if( (pObj_Parent->m_rPosition+pObj_Parent->m_pPopupPoint).Contains(X,Y) )
			return pObj_Parent;
	}
	return NULL;
}

void MouseBehavior::SetMediaInfo(string sTime,string sTotal,string sSpeed,string sTitle,string sSection)
{
	if( m_pMouseHandler && m_pMouseHandler->TypeOfMouseHandler()==MouseHandler::mh_Speed )
	{
		SpeedMouseHandler *pSpeedMouseHandler = (SpeedMouseHandler *) m_pMouseHandler;
		int Stop = pSpeedMouseHandler->ParseTime(sTotal);
		int Current = pSpeedMouseHandler->ParseTime(sTime);
		if( Stop==pSpeedMouseHandler->m_CurrentMedia_Stop && Current==pSpeedMouseHandler->m_CurrentMedia_Pos )
			return;
		pSpeedMouseHandler->m_CurrentMedia_Stop = Stop;
		pSpeedMouseHandler->m_CurrentMedia_Pos = Current;
NeedToRender render( m_pOrbiter, "start speed" );
m_pOrbiter->RenderObjectAsync(pSpeedMouseHandler->m_pObj);

	}
}

bool MouseBehavior::ButtonDown(int PK_Button)
{
	g_pPlutoLogger->Write(LV_FESTIVAL,"MouseBehavior::ButtonDown %d",(int) PK_Button);
	PLUTO_SAFETY_LOCK(mb,m_pOrbiter->m_ScreenMutex);
	m_iPK_Button_Mouse_Last=PK_Button;
	m_iTime_Last_Mouse_Down=ProcessUtils::GetMsTime();
	m_iTime_Last_Mouse_Up=0;
	int PK_Screen_OnScreen = m_pOrbiter->m_pScreenHistory_Current->PK_Screen();

	// Special case for the media control
	if( m_iPK_Button_Mouse_Last==BUTTON_Mouse_7_CONST && PK_Screen_OnScreen!=SCREEN_mnuMainMenu2_CONST )
	{
		g_pPlutoLogger->Write(LV_FESTIVAL,"MouseBehavior::ButtonDown showing main menu");
		NeedToRender render( m_pOrbiter, "mousebehavior" );  // Redraw anything that was changed by this command
		m_pOrbiter->CMD_Goto_Screen("",SCREEN_mnuMainMenu2_CONST);
		/*
		m_pOrbiter->CMD_Remove_Popup("",""); // Remove all popups
		m_pOrbiter->CMD_Show_Popup(StringUtils::itos(DESIGNOBJ_popMainMenu_CONST),0,0,"","left",false,false);
		Set_Mouse_Behavior("Lhu",true,"Y",StringUtils::itos(DESIGNOBJ_popMainMenu_CONST));
		*/
	}
	else if( m_iPK_Button_Mouse_Last==BUTTON_Mouse_6_CONST && PK_Screen_OnScreen!=SCREEN_mnuPlaybackControl_CONST 
		&& PK_Screen_OnScreen!=m_pOrbiter->m_iPK_Screen_OSD_Speed && PK_Screen_OnScreen!=m_pOrbiter->m_iPK_Screen_OSD_Track )
	{
		g_pPlutoLogger->Write(LV_FESTIVAL,"MouseBehavior::ButtonDown showing media menu");
		NeedToRender render( m_pOrbiter, "mousebehavior" );  // Redraw anything that was changed by this command
		if( m_pOrbiter->m_iPK_Screen_OSD_Speed && m_pOrbiter->m_iPK_Screen_OSD_Track )
			m_pOrbiter->CMD_Goto_Screen("", SCREEN_mnuPlaybackControl_CONST);
		else if( m_pOrbiter->m_iPK_Screen_OSD_Speed )
			m_pOrbiter->CMD_Goto_Screen("", m_pOrbiter->m_iPK_Screen_OSD_Speed);
		else if( m_pOrbiter->m_iPK_Screen_OSD_Track )
			m_pOrbiter->CMD_Goto_Screen("", m_pOrbiter->m_iPK_Screen_OSD_Track);
			
/*
		DesignObj_Orbiter *pObj = m_pOrbiter->FindObject(DESIGNOBJ_popSpeedControl_temp_CONST);  // Temp until the widget is done and can set this
		pObj->m_rPosition.Width=964;
		pObj->m_rPosition.Height=90;
		m_pOrbiter->CMD_Show_Popup(StringUtils::itos(DESIGNOBJ_popSpeedControl_temp_CONST),263,526,"","horiz",false,false);
		m_pOrbiter->CMD_Show_Popup(StringUtils::itos(m_pOrbiter->m_iPK_DesignObj_Remote_Popup),0,0,"","left",false,false);
		Set_Mouse_Behavior("S",false,"X",StringUtils::itos(DESIGNOBJ_popSpeedControl_temp_CONST));
		Set_Mouse_Behavior("T",false,"Y",StringUtils::itos(m_pOrbiter->m_iPK_DesignObj_Remote_Popup));
*/
	}
	else if( m_iPK_Button_Mouse_Last==BUTTON_Mouse_8_CONST && PK_Screen_OnScreen!=SCREEN_mnuAmbiance_CONST 
		&& PK_Screen_OnScreen!=SCREEN_mnuVolume_CONST && PK_Screen_OnScreen!=SCREEN_mnuLights_CONST )
	{
		g_pPlutoLogger->Write(LV_FESTIVAL,"MouseBehavior::ButtonDown showing ambiance menu");
		NeedToRender render( m_pOrbiter, "mousebehavior" );  // Redraw anything that was changed by this command
		m_pOrbiter->CMD_Goto_Screen("", SCREEN_mnuAmbiance_CONST);
/*
		DesignObj_Orbiter *pObj_Lights = m_pOrbiter->FindObject(DESIGNOBJ_popLightsInRoom_CONST);
		DesignObj_Orbiter *pObj_Volume = m_pOrbiter->FindObject(DESIGNOBJ_popVolume_CONST);
		if( pObj_Lights && pObj_Volume )
		{
			m_pOrbiter->CMD_Show_Popup(StringUtils::itos(DESIGNOBJ_popVolume_CONST),pObj_Lights->m_rPosition.Width + m_pOrbiter->m_Width/20,m_pOrbiter->m_Height*.95 - pObj_Volume->m_rPosition.Height,"","horiz",false,false);
			m_pOrbiter->CMD_Show_Popup(StringUtils::itos(DESIGNOBJ_popLightsInRoom_CONST),m_pOrbiter->m_Width/20,0,"","left",false,false);
			Set_Mouse_Behavior("V",false,"X",StringUtils::itos(DESIGNOBJ_popVolume_CONST));
			Set_Mouse_Behavior("G",false,"Y",StringUtils::itos(DESIGNOBJ_popLightsInRoom_CONST));
		}
*/
	}
	else if( (m_iPK_Button_Mouse_Last==BUTTON_Mouse_6_CONST && (PK_Screen_OnScreen==SCREEN_mnuPlaybackControl_CONST || PK_Screen_OnScreen==m_pOrbiter->m_iPK_Screen_OSD_Speed || PK_Screen_OnScreen==m_pOrbiter->m_iPK_Screen_OSD_Track)) ||
		(m_iPK_Button_Mouse_Last==BUTTON_Mouse_8_CONST && (PK_Screen_OnScreen==SCREEN_mnuAmbiance_CONST || PK_Screen_OnScreen==SCREEN_mnuVolume_CONST || PK_Screen_OnScreen==SCREEN_mnuLights_CONST)) ||
		(m_iPK_Button_Mouse_Last==BUTTON_Mouse_7_CONST && PK_Screen_OnScreen==SCREEN_mnuMainMenu2_CONST) )
	{
		g_pPlutoLogger->Write(LV_FESTIVAL,"MouseBehavior::ButtonDown removing menu");
		Clear(true);
	}
	else if( m_iPK_Button_Mouse_Last==BUTTON_Mouse_2_CONST )
	{
		if( m_pOrbiter->m_iPK_MediaType && (PK_Screen_OnScreen==m_pOrbiter->m_iPK_Screen_Remote || PK_Screen_OnScreen==m_pOrbiter->m_iPK_Screen_RemoteOSD) )
		{
			DCE::CMD_MH_Stop_Media CMD_MH_Stop_Media(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_MediaPlugIn,0,0,0,"");
			m_pOrbiter->SendCommand(CMD_MH_Stop_Media);
		}
		Clear(true);
	}

	if( m_pMouseHandler )
	{
		m_pMouseHandler->ButtonDown(PK_Button);
		return true;
	}
	return false;
}

bool MouseBehavior::ButtonUp(int PK_Button)
{
    g_pPlutoLogger->Write(LV_FESTIVAL,"MouseBehavior::ButtonUp %d %p",(int) PK_Button,m_pMouseHandler);
    PLUTO_SAFETY_LOCK(mb,m_pOrbiter->m_ScreenMutex);
    m_iPK_Button_Mouse_Last=PK_Button;
    m_iTime_Last_Mouse_Up=ProcessUtils::GetMsTime();

    if( m_pMouseHandler )
    {
		m_pMouseHandler->ButtonUp(PK_Button);
		return true;
    }
    return false;
}

void MouseBehavior::HighlightObject(DesignObj_Orbiter *pObj)
{
	DesignObj_Orbiter *pObj_Previously_Highlighted=m_pOrbiter->m_pObj_Highlighted;

	m_pOrbiter->Renderer()->UnHighlightObject();
	if( pObj_Previously_Highlighted && pObj!=pObj_Previously_Highlighted )
		m_pOrbiter->ExecuteCommandsInList( &pObj_Previously_Highlighted->m_Action_UnhighlightList, pObj_Previously_Highlighted, smHighlight, 0, 0 );
	m_pOrbiter->m_pObj_Highlighted = pObj;
	m_pOrbiter->Renderer()->DoHighlightObject();
//			m_pMouseBehavior->m_pOrbiter->RenderObjectAsync(m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted);
}

PlutoRectangle MouseBehavior::GetHighlighedObjectCoordinates()
{
	if( m_pOrbiter->m_pObj_Highlighted )
	{
		if( m_pOrbiter->m_pObj_Highlighted->m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST )
		{
			PlutoRectangle rect;
			m_pOrbiter->GetDataGridHighlightCellCoordinates((DesignObj_DataGrid *) m_pOrbiter->m_pObj_Highlighted,rect);

g_pPlutoLogger->Write(LV_FESTIVAL,"MouseBehavior::GetHighlighedObjectCoordinates %d,%d - %d,%d",rect.X,rect.Y,rect.Width,rect.Height);
			return rect + m_pOrbiter->m_pObj_Highlighted->m_pPopupPoint;
		}
		else
			return m_pOrbiter->m_pObj_Highlighted->m_rPosition + m_pOrbiter->m_pObj_Highlighted->m_pPopupPoint;
	}
	return PlutoRectangle(0,0,0,0);
}

void MouseBehavior::SelectFirstObject(char cDirection,DesignObj_Orbiter *pObj_Parent)
{
	// Select the first object to highlight if we didn't already and center over it
	DesignObj_Orbiter *pObj = m_pOrbiter->m_pObj_Highlighted = m_pOrbiter->Renderer()->FindFirstObjectByDirection(cDirection,true,pObj_Parent,NULL);
	if( pObj )
	{
		PlutoRectangle rect = GetHighlighedObjectCoordinates();
		SetMousePosition( rect.X + rect.Width/2,
			rect.Y + rect.Height/2 );
	}
}

int MouseBehavior::GetDirectionAwayFromHighlight(int X,int Y)
{
	DesignObj_Orbiter *pObj = m_pOrbiter->m_pObj_Highlighted; // Make the code look better

	PlutoRectangle plutoRectangle = GetHighlighedObjectCoordinates();
	if( plutoRectangle.Contains(X,Y) )
		return 0;  // We're still inside the object

	// If the highlighted object is a datagrid and we're totally outside the datagrid, then use the datagrid
	// as a whole for determining which side we're on, and not the individual cell, otherwise we'll always
	// think we're below a datagrid without a lot of cells even if we're to the left or right
	if( pObj->m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST && !(pObj->m_rPosition+pObj->m_pPopupPoint).Contains(X,Y) )
		plutoRectangle = pObj->m_rPosition + pObj->m_pPopupPoint;

//	g_pPlutoLogger->Write(LV_FESTIVAL,"KeyboardMouseHandler::GetDirectionAwayFromHighlight object %s at %d,%d w:%d h:%d doesn't contain %d,%d",
//		pObj->m_ObjectID.c_str(),plutoRectangle.X,plutoRectangle.Y,
//		plutoRectangle.Width,plutoRectangle.Height,X,Y);
	int FromLeft = plutoRectangle.X - X;
	int FromTop = plutoRectangle.Y - Y;
	int FromRight = X - plutoRectangle.Right();
	int FromBottom = Y - plutoRectangle.Bottom();

	if( FromLeft>0 && FromLeft>=FromTop && FromLeft>=FromBottom )
		return DIRECTION_Left_CONST;
	if( FromTop>0 && FromTop>=FromLeft && FromTop>=FromRight )
		return DIRECTION_Up_CONST;
	if( FromRight>0 && FromRight>=FromTop && FromRight>=FromBottom )
		return DIRECTION_Right_CONST;
	if( FromBottom>0 && FromBottom>=FromLeft && FromBottom>=FromRight )
		return DIRECTION_Down_CONST;

	return 0; // shouldn't happen
}

void MouseBehavior::PositionMouseAtObjectEdge(int PK_Direction)
{
	DesignObj_Orbiter *pObj = m_pOrbiter->m_pObj_Highlighted; // Make the code look better
	PlutoRectangle rect = GetHighlighedObjectCoordinates();

	switch(PK_Direction)
	{
	case DIRECTION_Left_CONST:
		SetMousePosition( rect.Right(), 
			rect.Y + rect.Height/2 );
		return;
	case DIRECTION_Up_CONST:
		SetMousePosition( rect.X + rect.Width/2, 
			rect.Bottom());
		return;
	case DIRECTION_Right_CONST:
		SetMousePosition( rect.X, 
			rect.Y + rect.Height/2 );
		return;
	case DIRECTION_Down_CONST:
		SetMousePosition( rect.X + rect.Width/2, 
			rect.Y);
		return;
	};
}

const MouseHandler* MouseBehavior::GetMouseHandler() const
{
	return m_pMouseHandler;
}

void MouseSensitivity::SetSensitivity(int Level)
{
	switch(Level)
	{
	case 2:
		SampleInterval = 350;
		DiscardSamplesOlderThanSec = 2250; // 1500 ms
		Threshhold_1_Minimum=70;  // Minimum number of pixels to move
		Threshhold_1_Ratio=400; // Minimum ratio : 100=baseline, 200=twice as much on the directional axis as the non-directional, 500=5x as much
		Threshhold_2_Minimum=100;  // 20 pixels over 5 samples
		Threshhold_2_Ratio=300;
		Threshhold_2_Samples=4; // How many samples must be taken before the ratio/minimum's are lowered to threshhold 2
		Threshhold_3_Minimum=160;
		Threshhold_3_Ratio=200;
		Threshhold_3_Samples=4;
		IgnoreMovesLessThanThisPerSample=20;
		NumNotchesForExternalApp=12;
		break;
	case 3:
		SampleInterval = 500;
		DiscardSamplesOlderThanSec = 3000; // 1500 ms
		Threshhold_1_Minimum=80;  // Minimum number of pixels to move
		Threshhold_1_Ratio=400; // Minimum ratio : 100=baseline, 200=twice as much on the directional axis as the non-directional, 500=5x as much
		Threshhold_2_Minimum=120;  // 20 pixels over 5 samples
		Threshhold_2_Ratio=300;
		Threshhold_2_Samples=3; // How many samples must be taken before the ratio/minimum's are lowered to threshhold 2
		Threshhold_3_Minimum=200;
		Threshhold_3_Ratio=200;
		Threshhold_3_Samples=4;
		IgnoreMovesLessThanThisPerSample=30;
		NumNotchesForExternalApp=5;
		break;
	case 1:
	default:
		SampleInterval = 200; //250; // 250 ms
		DiscardSamplesOlderThanSec = 1500; // 1500 ms
		Threshhold_1_Minimum=60;  // Minimum number of pixels to move
		Threshhold_1_Ratio=400; // Minimum ratio : 100=baseline, 200=twice as much on the directional axis as the non-directional, 500=5x as much
		Threshhold_2_Minimum=80;  // 20 pixels over 5 samples
		Threshhold_2_Ratio=300;
		Threshhold_2_Samples=5; // How many samples must be taken before the ratio/minimum's are lowered to threshhold 2
		Threshhold_3_Minimum=80;
		Threshhold_3_Ratio=200;
		Threshhold_3_Samples=10;
		IgnoreMovesLessThanThisPerSample=10;//30;
		NumNotchesForExternalApp=20;
		break;
	}
}

MouseHandler::MouseHandler(DesignObj_Orbiter *pObj, string sOptions, MouseBehavior *pMouseBehavior)
{
    m_bStartedMovement=false;
    m_pObj=pObj;
    m_sOptions=sOptions;
    m_pMouseBehavior=pMouseBehavior;
    m_bLockAxis=true;
}

MouseHandler::~MouseHandler()
{
}
