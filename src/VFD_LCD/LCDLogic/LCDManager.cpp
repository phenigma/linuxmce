#include "LCDManager.h"
//--------------------------------------------------------------------------------------------------------
LCDManager::LCDManager(MenuHolder *pMenuHolder, IRenderer *pRenderer, Command_Impl *pCommandImpl) : 
	IInputProcessor(), m_pMenuHolder(pMenuHolder), m_pRenderer(pRenderer), m_pCommandImpl(pCommandImpl)
{
	m_display_state.m_sHeader = "Welcome to Fiire 1.0!";
	m_pRenderer->Render(m_display_state);
}
//--------------------------------------------------------------------------------------------------------
LCDManager::~LCDManager()
{
}
//--------------------------------------------------------------------------------------------------------
bool LCDManager::ProcessInput(IInput *pInput)
{
	//TODO: process the input

	//decide which is current node
	//display on renderer
	//execute action for current node, if any

	m_pRenderer->Render(m_display_state);
	return true;
}
//--------------------------------------------------------------------------------------------------------

