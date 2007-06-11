#ifndef __LCD_MANAGER_H__
#define __LCD_MANAGER_H__
//--------------------------------------------------------------------------------------------------------
#include "IInputProcessor.h"
#include "IRenderer.h"
//--------------------------------------------------------------------------------------------------------
#include "DCE/Logger.h"
//--------------------------------------------------------------------------------------------------------
class MenuHolder;
class MenuItem;
//--------------------------------------------------------------------------------------------------------
class LCDManager : public IInputProcessor
{
private:

	DisplayState m_display_state;
	MenuHolder *m_pMenuHolder;
	pluto_pthread_mutex_t m_RenderMutex;
	list<IRenderer *> m_listRenderer;
	pthread_t m_RerendererThreadID;
	map<string, string> m_mapVariables;

	unsigned long m_ulRendererInterval;
	bool m_bQuit;

	void Prepare();
	void CancelRerender();
	void ScheduleRerender(unsigned long ulMiliseconds);
	string ListItemDescription(MenuItem *MenuItem);
	bool Expand(MenuItem *pMenuItem);

public:

	LCDManager(MenuHolder *pMenuHolder);
	~LCDManager();

	void Render();
	bool Quit();
	unsigned long RerenderInterval();

	void SetVariable(string sVar, string sValue);
	void AddRenderer(IRenderer *pRenderer);
	bool ProcessInput(const Input &input);

	void ShowStatus(string sStatus, int nType);
};
//--------------------------------------------------------------------------------------------------------
#endif //__LCD_MANAGER_H__
