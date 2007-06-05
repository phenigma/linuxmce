#include "LCDManager.h"
#include "MenuHolder.h"
#include "MenuItem.h"
//--------------------------------------------------------------------------------------------------------
LCDManager::LCDManager(MenuHolder *pMenuHolder, Command_Impl *pCommandImpl) : 
	IInputProcessor(), m_pMenuHolder(pMenuHolder), m_pCommandImpl(pCommandImpl),
	m_RenderMutex("render")
{
	m_display_state.m_sHeader = "Welcome to Fiire 1.0!";

	m_RenderMutex.Init(NULL);
}
//--------------------------------------------------------------------------------------------------------
LCDManager::~LCDManager()
{
	pthread_mutex_destroy(&m_RenderMutex.mutex);
}
//--------------------------------------------------------------------------------------------------------
void LCDManager::AddRenderer(IRenderer *pRenderer)
{
	if(NULL != pRenderer)
	{
		PLUTO_SAFETY_LOCK(sm, m_RenderMutex);
		m_listRenderer.push_back(pRenderer);

		Render();
	}
}
//--------------------------------------------------------------------------------------------------------
bool LCDManager::ProcessInput(const Input &input)
{
	PLUTO_SAFETY_LOCK(sm, m_RenderMutex);

	if(NULL == m_pMenuHolder)
	{
		DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Got not menu holder. The input will be ignored");
		return false;
	}

	//process the input
	switch(input.type)
	{
		case itKeyboardInput:
		{
			
			{
				switch(input.keyboardinput)
				{
					case kiUp :		
						m_display_state.m_sDescription = "up";		
						m_pMenuHolder->MoveUp();
						break;
					case kiDown :	
						m_display_state.m_sDescription = "down"; 
						m_pMenuHolder->MoveDown();
						break;
					case kiLeft :	
						m_display_state.m_sDescription = "left"; 
						m_pMenuHolder->MoveLeft();
						break;
					case kiRight :	
						m_display_state.m_sDescription = "right"; 
						m_pMenuHolder->MoveRight();
						break;
					default :	
						m_display_state.m_sDescription = "unknown"; 
						break;
				}
			}
		}
		break;

		default:
			break;
	}

	//decide which is current node
	//display on renderer
	//execute action for current node, if any

	Render();

	return true;
}
//--------------------------------------------------------------------------------------------------------
void LCDManager::Render()
{
	if(NULL != m_pMenuHolder && NULL != m_pMenuHolder->CurrentMenuItem())
		m_display_state.m_sHeader = m_pMenuHolder->CurrentMenuItem()->Description();

	for(list<IRenderer *>::iterator it = m_listRenderer.begin(), end = m_listRenderer.end(); it != end; ++it)
		(*it)->Render(m_display_state);
}
//--------------------------------------------------------------------------------------------------------