#ifndef __INPUT_MENU_ITEM_H__
#define __INPUT_MENU_ITEM_H__
//--------------------------------------------------------------------------------------------------------
#include "MenuItem.h"
//--------------------------------------------------------------------------------------------------------
class InputMenuItem : public MenuItem
{
	string m_sMask; 
	size_t m_nCaretIndex; 

public:

	InputMenuItem(string sDescription, MenuItem *pParent = NULL, ItemType type = itListItem, MenuItemAction *pMenuItemAction = NULL);
	~InputMenuItem();	

	void Mask(string sMask);
	string Mask();

	bool CanGoRight();
	bool CanGoLeft();
	bool CanGoUp();
	bool CanGoDown();

	MenuItem *MoveUp();
	MenuItem *MoveDown();
	MenuItem *MoveRight();
	MenuItem *MoveLeft();
};
//--------------------------------------------------------------------------------------------------------
#endif //__INPUT_MENU_ITEM_H__
