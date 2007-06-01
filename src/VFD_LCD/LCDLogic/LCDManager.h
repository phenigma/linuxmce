#ifndef __LCD_MANAGER_H__
#define __LCD_MANAGER_H__
//--------------------------------------------------------------------------------------------------------
#include "IInputProcessor.h"
#include "IRenderer.h"
//--------------------------------------------------------------------------------------------------------
#include "DCE/Logger.h"
//--------------------------------------------------------------------------------------------------------
class MenuHolder;
class Command_Impl;
//--------------------------------------------------------------------------------------------------------
class LCDManager : public IInputProcessor
{
private:

	DisplayState m_display_state;
	MenuHolder *m_pMenuHolder;
	Command_Impl *m_pCommandImpl;

	pluto_pthread_mutex_t m_RenderMutex;

	list<IRenderer *> m_listRenderer;

	void Render();

public:

	LCDManager(MenuHolder *pMenuHolder, Command_Impl *pCommandImpl);
	~LCDManager();

	void AddRenderer(IRenderer *pRenderer);
	bool ProcessInput(const Input &input);
};
//--------------------------------------------------------------------------------------------------------
#endif //__LCD_MANAGER_H__
