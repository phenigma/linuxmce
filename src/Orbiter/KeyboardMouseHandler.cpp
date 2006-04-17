#include "KeyboardMouseHandler.h"
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

using namespace DCE;

void KeyboardMouseHandler::Start()
{
	m_cDirection=0;
	m_pMouseBehavior->SetMousePosition(m_pMouseBehavior->m_pOrbiter->m_Width/2,
		m_pMouseBehavior->m_pOrbiter->m_Height/2);
}

void KeyboardMouseHandler::Stop()
{
}

bool KeyboardMouseHandler::ButtonDown(int PK_Button)
{
	if( PK_Button==BUTTON_Mouse_1_CONST )
	{
		DCE::CMD_Simulate_Keypress CMD_Simulate_Keypress(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,
			StringUtils::itos(BUTTON_Enter_CONST),"");
		m_pMouseBehavior->m_pOrbiter->SendCommand(CMD_Simulate_Keypress);
	}
	return false; // Keep processing
}

bool KeyboardMouseHandler::ButtonUp(int PK_Button)
{
	return false; // Keep processing
}

void KeyboardMouseHandler::Move(int X,int Y)
{
	bool bHorizontal = m_pMouseBehavior->m_cLocked_Axis_Current==AXIS_LOCK_X;
	int NotchSize = (double) bHorizontal ? m_pMouseBehavior->m_pOrbiter->m_Width / 20 : m_pMouseBehavior->m_pOrbiter->m_Height / 20;
	int Notch;
	if( bHorizontal )
		Notch = X/NotchSize - 10;
	else
		Notch = Y/NotchSize - 10;
g_pPlutoLogger->Write(LV_FESTIVAL,"Notch %d  horizontal %d  %d,%d",Notch,(int) bHorizontal,X,Y);

	if( Notch<-10 )
		Notch = -10;
	if( Notch>10 )
		Notch = 10;

	if( Notch!=m_iLastNotch )
	{
		if( Notch==0 )
			m_pMouseBehavior->m_pMouseIterator->SetIterator(MouseIterator::if_None,0,0,NULL);
		else
		{
			int Frequency;
			bool bPage=false;
			if( abs(Notch)>5 )
			{
				bPage=true;
				if( Notch>0 )
					Frequency = (6-(Notch-5))*400;
				else
					Frequency = (6+(Notch+5))*400;
			}
			else
				Frequency = (6-abs(Notch))*400;

g_pPlutoLogger->Write(LV_FESTIVAL,"Frequency %d",Frequency);
			if( bHorizontal )
			{
				if( bPage )
					m_cDirection = Notch > 0 ? 'R' : 'L';
				else
					m_cDirection = Notch > 0 ? 'r' : 'l';
			}
			else
			{
				if( bPage )
					m_cDirection = Notch > 0 ? 'D' : 'U';
				else
					m_cDirection = Notch > 0 ? 'd' : 'u';
			}
			m_pMouseBehavior->m_pMouseIterator->SetIterator(MouseIterator::if_Keyboard,0,Frequency,this);
		}
		m_iLastNotch=Notch;
	}
}

void KeyboardMouseHandler::DoIteration()
{
g_pPlutoLogger->Write(LV_FESTIVAL,"Direction %c",m_cDirection);
	if( m_pObj )
		IterateObject();
	else
		IterateExternalApp();
}

void KeyboardMouseHandler::IterateObject()
{
	switch( m_cDirection )
	{
		case 'U':
		{
			m_pMouseBehavior->m_pOrbiter->CMD_Simulate_Keypress(
				StringUtils::itos(BUTTON_Scroll_Up_CONST),"");
		}
		break;

		case 'D':
		{
			m_pMouseBehavior->m_pOrbiter->CMD_Simulate_Keypress(
				StringUtils::itos(BUTTON_Scroll_Down_CONST),"");
		}
		break;

		case 'u':
		{
			m_pMouseBehavior->m_pOrbiter->CMD_Simulate_Keypress(
				StringUtils::itos(BUTTON_Up_Arrow_CONST),"");
		}
		break;

		case 'd':
		{
			m_pMouseBehavior->m_pOrbiter->CMD_Simulate_Keypress(
				StringUtils::itos(BUTTON_Down_Arrow_CONST),"");
		}
		break;

		case 'L':
		{
			m_pMouseBehavior->m_pOrbiter->CMD_Simulate_Keypress(
				StringUtils::itos(BUTTON_Scroll_Left_CONST),"");
		}
		break;

		case 'R':
		{
			m_pMouseBehavior->m_pOrbiter->CMD_Simulate_Keypress(
				StringUtils::itos(BUTTON_Scroll_Right_CONST),"");
		}
		break;

		case 'l':
		{
			m_pMouseBehavior->m_pOrbiter->CMD_Simulate_Keypress(
				StringUtils::itos(BUTTON_Left_Arrow_CONST),"");
		}
		break;

		case 'r':
		{
			m_pMouseBehavior->m_pOrbiter->CMD_Simulate_Keypress(
				StringUtils::itos(BUTTON_Right_Arrow_CONST),"");
		}
		break;
	}
}

void KeyboardMouseHandler::IterateExternalApp()
{
	switch( m_cDirection )
	{
		case 'U':
		{
			DCE::CMD_Simulate_Keypress CMD_Simulate_Keypress(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,
				StringUtils::itos(BUTTON_Scroll_Up_CONST),"");
			m_pMouseBehavior->m_pOrbiter->SendCommand(CMD_Simulate_Keypress);
		}
		break;

		case 'D':
		{
			DCE::CMD_Simulate_Keypress CMD_Simulate_Keypress(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,
				StringUtils::itos(BUTTON_Scroll_Down_CONST),"");
			m_pMouseBehavior->m_pOrbiter->SendCommand(CMD_Simulate_Keypress);
		}
		break;

		case 'u':
		{
			DCE::CMD_Simulate_Keypress CMD_Simulate_Keypress(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,
				StringUtils::itos(BUTTON_Up_Arrow_CONST),"");
			m_pMouseBehavior->m_pOrbiter->SendCommand(CMD_Simulate_Keypress);
		}
		break;

		case 'd':
		{
			DCE::CMD_Simulate_Keypress CMD_Simulate_Keypress(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,
				StringUtils::itos(BUTTON_Down_Arrow_CONST),"");
			m_pMouseBehavior->m_pOrbiter->SendCommand(CMD_Simulate_Keypress);
		}
		break;

		case 'L':
		{
			DCE::CMD_Simulate_Keypress CMD_Simulate_Keypress(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,
				StringUtils::itos(BUTTON_Scroll_Left_CONST),"");
			m_pMouseBehavior->m_pOrbiter->SendCommand(CMD_Simulate_Keypress);
		}
		break;

		case 'R':
		{
			DCE::CMD_Simulate_Keypress CMD_Simulate_Keypress(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,
				StringUtils::itos(BUTTON_Scroll_Right_CONST),"");
			m_pMouseBehavior->m_pOrbiter->SendCommand(CMD_Simulate_Keypress);
		}
		break;

		case 'l':
		{
			DCE::CMD_Simulate_Keypress CMD_Simulate_Keypress(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,
				StringUtils::itos(BUTTON_Left_Arrow_CONST),"");
			m_pMouseBehavior->m_pOrbiter->SendCommand(CMD_Simulate_Keypress);
		}
		break;

		case 'r':
		{
			DCE::CMD_Simulate_Keypress CMD_Simulate_Keypress(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,
				StringUtils::itos(BUTTON_Right_Arrow_CONST),"");
			m_pMouseBehavior->m_pOrbiter->SendCommand(CMD_Simulate_Keypress);
		}
		break;
	}
}
