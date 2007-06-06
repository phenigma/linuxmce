#ifndef __LCD_MANAGER_H__
#define __LCD_MANAGER_H__
//--------------------------------------------------------------------------------------------------------
#include "IInputProcessor.h"
#include "IRenderer.h"
//--------------------------------------------------------------------------------------------------------
#include "DCE/Logger.h"
//--------------------------------------------------------------------------------------------------------
class MenuHolder;
//--------------------------------------------------------------------------------------------------------
class LCDManager : public IInputProcessor
{
private:

	DisplayState m_display_state;
	MenuHolder *m_pMenuHolder;

	pluto_pthread_mutex_t m_RenderMutex;

	list<IRenderer *> m_listRenderer;

	void Render();
	void Prepare();

public:

	LCDManager(MenuHolder *pMenuHolder);
	~LCDManager();

	void AddRenderer(IRenderer *pRenderer);
	bool ProcessInput(const Input &input);
};
//--------------------------------------------------------------------------------------------------------
#endif //__LCD_MANAGER_H__
