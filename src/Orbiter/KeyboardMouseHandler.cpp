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
	m_bHorizontal = m_pMouseBehavior->m_pMouseHandler_Horizontal==this;
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
	int NotchSize = (double) m_bHorizontal ? m_pMouseBehavior->m_pOrbiter->m_Width / 20 : m_pMouseBehavior->m_pOrbiter->m_Height / 20;
	int Notch;
	if( m_bHorizontal )
		Notch = X/NotchSize - 10;
	else
		Notch = Y/NotchSize - 10;
g_pPlutoLogger->Write(LV_FESTIVAL,"Notch %d  horizontal %d  %d,%d",Notch,(int) m_bHorizontal,X,Y);

	if( Notch<-10 )
		Notch = -10;
	if( Notch>10 )
		Notch = 10;

	if( Notch!=m_iLastNotch )
	{
		if( Notch==0 )
			m_pMouseBehavior->m_pMouseIterator->SetIterator(MouseIterator::if_None,0,0);
		else
		{
			int Frequency;
			bool bPage=false;
			if( abs(Notch)>5 )
			{
				bPage=true;
				if( Notch>0 )
					Frequency = (6-(Notch-5))*100;
				else
					Frequency = (6+(Notch+5))*100;
			}
			else
				Frequency = (6-abs(Notch))*100;

if( Frequency<1 )
int k=2;
g_pPlutoLogger->Write(LV_FESTIVAL,"Frequency %d",Frequency);
			if( m_bHorizontal )
			{
				if( bPage )
					m_pMouseBehavior->m_pMouseIterator->SetIterator(MouseIterator::if_Keyboard,Notch > 0 ? 'L' : 'R',Frequency);
				else
					m_pMouseBehavior->m_pMouseIterator->SetIterator(MouseIterator::if_Keyboard,Notch > 0 ? 'l' : 'r',Frequency);
			}
			else
			{
				if( bPage )
					m_pMouseBehavior->m_pMouseIterator->SetIterator(MouseIterator::if_Keyboard,Notch > 0 ? 'D' : 'U',Frequency);
				else
					m_pMouseBehavior->m_pMouseIterator->SetIterator(MouseIterator::if_Keyboard,Notch > 0 ? 'd' : 'u',Frequency);
			}
		}
		m_iLastNotch=Notch;
//remus  m_pObj->SetSpeed(Speed);
	}
}
