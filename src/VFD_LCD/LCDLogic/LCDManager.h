#ifndef __LCD_MANAGER_H__
#define __LCD_MANAGER_H__
//--------------------------------------------------------------------------------------------------------
#include "IInputProcessor.h"
//--------------------------------------------------------------------------------------------------------
class IRenderer;
class MenuHolder;
class Command_Impl;
//--------------------------------------------------------------------------------------------------------
class LCDManager : public IInputProcessor
{
private:

	MenuHolder *m_pMenuHolder;
	IRenderer *m_pRenderer;
	Command_Impl *m_pCommandImpl;

public:

	LCDManager(MenuHolder *pMenuHolder, IRenderer *pRenderer, Command_Impl *pCommandImpl);
	~LCDManager();

	bool ProcessInput(IInput *pInput);
};
//--------------------------------------------------------------------------------------------------------
#endif //__LCD_MANAGER_H__
