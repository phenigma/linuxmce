#ifndef __MENU_HOLDER_H__
#define __MENU_HOLDER_H__
//--------------------------------------------------------------------------------------------------------
class MenuItem;
//--------------------------------------------------------------------------------------------------------
class MenuHolder 
{
	MenuItem *m_pRootMenuItem;
	MenuItem *m_pCurrentMenuItem;

public:

	MenuHolder();
	~MenuHolder();

	MenuItem *RootMenu();
};
//--------------------------------------------------------------------------------------------------------
#endif //__MENU_HOLDER_H__
