#ifndef __MENU_HOLDER_H__
#define __MENU_HOLDER_H__
//--------------------------------------------------------------------------------------------------------
class MenuItem;
class ActionProcessor;
//--------------------------------------------------------------------------------------------------------
class MenuHolder 
{
	MenuItem *m_pRootMenuItem;
	MenuItem *m_pCurrentMenuItem;
	MenuItem *m_pLastCurrentMenuItem;

	ActionProcessor *m_pActionProcessor;

public:

	MenuHolder();
	~MenuHolder();

	void Setup(ActionProcessor *pActionProcessor);

	MenuItem *RootMenu();
	MenuItem *CurrentMenuItem();

	void MoveUp();
	void MoveDown();
	void MoveLeft();
	void MoveRight();

	void Home();
	void Restore();

	MenuItem *GetAudioSettingsNode();
	MenuItem *GetVideoSettingsNode();
};
//--------------------------------------------------------------------------------------------------------
#endif //__MENU_HOLDER_H__
