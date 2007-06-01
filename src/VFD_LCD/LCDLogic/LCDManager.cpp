#include "LCDManager.h"
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

	//TODO: process the input

	//decide which is current node
	//display on renderer
	//execute action for current node, if any

	switch(input.type)
	{
		case itKeyboardInput:
		{
			m_display_state.m_sDescription = 
				kiUp == input.keyboardinput ? "up" : 
					kiDown == input.keyboardinput ? "down" : 
						kiRight == input.keyboardinput ? "right" : 
							kiLeft == input.keyboardinput ? "left" : "unknown";
		}
		break;

		default:
			break;
	}

	Render();

	return true;
}
//--------------------------------------------------------------------------------------------------------
void LCDManager::Render()
{
	for(list<IRenderer *>::iterator it = m_listRenderer.begin(), end = m_listRenderer.end(); it != end; ++it)
		(*it)->Render(m_display_state);
}
//--------------------------------------------------------------------------------------------------------