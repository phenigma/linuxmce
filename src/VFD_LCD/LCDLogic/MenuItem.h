#ifndef __MENU_ITEM_H__
#define __MENU_ITEM_H__
//--------------------------------------------------------------------------------------------------------
#include <string>
#include <vector>
using namespace std;
//--------------------------------------------------------------------------------------------------------
class MenuItemAction;
//--------------------------------------------------------------------------------------------------------
enum ItemType
{
	itStatusItem,
	itListItem,
	itInputBox
};
//--------------------------------------------------------------------------------------------------------
class MenuItem
{
protected:

	ItemType m_type;
	string m_sDescription;
	MenuItem *m_pParent;
	vector<MenuItem *> m_vectChildren;
	MenuItemAction *m_pMenuItemAction;

public:

	MenuItem(string sDescription, MenuItem *pParent = NULL, ItemType type = itListItem, MenuItemAction *pMenuItemAction = NULL);
	virtual ~MenuItem();

	virtual string Value();
	virtual string Description();
	MenuItemAction *Action();
	ItemType Type();

	bool IsLeaf();

	MenuItem *Parent();
	void AddChild(MenuItem *pMenuItem);

	MenuItem *FirstChild();
	MenuItem *NextChild(MenuItem *pChildMenuItem);
	MenuItem *PrevChild(MenuItem *pChildMenuItem);

	virtual bool CanGoRight();
	virtual bool CanGoLeft();
	virtual bool CanGoUp();
	virtual bool CanGoDown();

	virtual MenuItem *MoveUp();
	virtual MenuItem *MoveDown();
	virtual MenuItem *MoveRight();
	virtual MenuItem *MoveLeft();
};
//--------------------------------------------------------------------------------------------------------
#endif //__MENU_ITEM_H__
