#include "LCDManager.h"
//--------------------------------------------------------------------------------------------------------
LCDManager::LCDManager(MenuHolder *pMenuHolder, IRenderer *pRenderer) : 
	IInputProcessor(), m_pMenuHolder(pMenuHolder), m_pRenderer(pRenderer), m_pCommandImpl(pCommandImpl)
{
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
}
//--------------------------------------------------------------------------------------------------------

