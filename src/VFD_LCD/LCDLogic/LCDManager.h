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
	pthread_t m_RerendererThreadID;

	unsigned long m_ulRendererInterval;
	bool m_bQuit;

	void Prepare();

	void CancelRerender();
	void ScheduleRerender(unsigned long ulMiliseconds);

public:

	LCDManager(MenuHolder *pMenuHolder);
	~LCDManager();

	void Render();
	unsigned long RerenderInterval();
	bool Quit();

	void AddRenderer(IRenderer *pRenderer);
	bool ProcessInput(const Input &input);
};
//--------------------------------------------------------------------------------------------------------
#endif //__LCD_MANAGER_H__
