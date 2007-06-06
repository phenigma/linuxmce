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
};
//--------------------------------------------------------------------------------------------------------
#endif //__MENU_HOLDER_H__
