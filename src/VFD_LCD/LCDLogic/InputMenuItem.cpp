#include "InputMenuItem.h"
//--------------------------------------------------------------------------------------------------------
InputMenuItem::InputMenuItem(string sDescription, MenuItem *pParent, ItemType type, MenuItemAction *pMenuItemAction) :
	MenuItem(sDescription, pParent, type, pMenuItemAction),	m_nCaretIndex(0)
{
}
//--------------------------------------------------------------------------------------------------------
InputMenuItem::~InputMenuItem()
{
}
//--------------------------------------------------------------------------------------------------------
void InputMenuItem::Mask(string sMask)
{
	m_sMask = sMask;
	m_sDescription = sMask;
}
//--------------------------------------------------------------------------------------------------------
string InputMenuItem::Mask()
{
	return m_sMask;
}
//--------------------------------------------------------------------------------------------------------
bool InputMenuItem::CanGoRight()
{
	return MenuItem::CanGoRight();
}
//--------------------------------------------------------------------------------------------------------
bool InputMenuItem::CanGoLeft()
{
	return MenuItem::CanGoLeft();
}
//--------------------------------------------------------------------------------------------------------
bool InputMenuItem::CanGoUp()
{
	return MenuItem::CanGoUp();
}
//--------------------------------------------------------------------------------------------------------
bool InputMenuItem::CanGoDown()
{
	return MenuItem::CanGoDown();
}
//--------------------------------------------------------------------------------------------------------
MenuItem *InputMenuItem::MoveUp()
{
    return MenuItem::MoveUp();
}
//--------------------------------------------------------------------------------------------------------
MenuItem *InputMenuItem::MoveDown()
{
	return MenuItem::MoveDown();
}
//--------------------------------------------------------------------------------------------------------
MenuItem *InputMenuItem::MoveRight()
{
	return MenuItem::MoveRight();
}
//--------------------------------------------------------------------------------------------------------
MenuItem *InputMenuItem::MoveLeft()
{
	return MenuItem::MoveLeft();
}
//--------------------------------------------------------------------------------------------------------
