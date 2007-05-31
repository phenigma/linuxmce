#ifndef __MENU_ITEM_H__
#define __MENU_ITEM_H__
//--------------------------------------------------------------------------------------------------------
#include <string>
#include <vector>
using namespace std;
//--------------------------------------------------------------------------------------------------------
class MenuItemAction;
//--------------------------------------------------------------------------------------------------------
class MenuItem
{
private:

	string m_sDescription;
	MenuItem *m_pParent;
	vector<MenuItem *> m_vectChildren;
	MenuItemAction *m_pMenuItemAction;

public:

	MenuItem(string sDescription, MenuItem *pParent = NULL, MenuItemAction *pMenuItemAction = NULL);
	~MenuItem();

	string Description();
	MenuItemAction *Action();

	bool CanGoRight();
	bool CanGoLeft();
	bool CanGoUp();
	bool CanGoDown();

	MenuItem *Parent();
};
//--------------------------------------------------------------------------------------------------------
#endif //__MENU_ITEM_H__
