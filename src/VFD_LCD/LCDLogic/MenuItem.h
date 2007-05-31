#ifndef __MENU_ITEM_H__
#define __MENU_ITEM_H__
//--------------------------------------------------------------------------------------------------------
class MenuItem
{
private:

	MenuItem *m_pParent;
	vector<MenuItem *> m_vectChildren;

public:

	MenuItem(string sDescription, MenuItemAction *pMenuItemAction = 0);
	~MenuItem();

	string Description();
	void ExecuteAction();

	bool CanGoRight();
	bool CanGoLeft();
	bool CanGoUp();
	bool CanGoDown();

	MenuItem *Parent();
};
//--------------------------------------------------------------------------------------------------------
#endif //__MENU_ITEM_H__
