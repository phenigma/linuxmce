#include "LCDManager.h"
//--------------------------------------------------------------------------------------------------------
LCDManager::LCDManager(MenuHolder *pMenuHolder, IRenderer *pRenderer, Command_Impl *pCommandImpl) : 
	IInputProcessor(), m_pMenuHolder(pMenuHolder), m_pRenderer(pRenderer), m_pCommandImpl(pCommandImpl),
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
void LCDManager::Renderer(IRenderer *pRenderer)
{
	{
		PLUTO_SAFETY_LOCK(sm, m_RenderMutex);
		m_pRenderer = pRenderer;
	}

	if(NULL != m_pRenderer)
		m_pRenderer->Render(m_display_state);
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

	if(NULL != m_pRenderer)
		m_pRenderer->Render(m_display_state);

	return true;
}
//--------------------------------------------------------------------------------------------------------

