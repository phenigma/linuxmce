#include "MenuItem.h"
#include "MenuItemAction.h"
//--------------------------------------------------------------------------------------------------------
MenuItem::MenuItem(string sDescription, MenuItem *pParent, MenuItemAction *pMenuItemAction) : 
	m_sDescription(sDescription), m_pParent(pParent), m_pMenuItemAction(pMenuItemAction)
{
}
//--------------------------------------------------------------------------------------------------------
MenuItem::~MenuItem()
{
	for(vector<MenuItem *>::iterator it = m_vectChildren.begin(), end = m_vectChildren.end(); it != end; ++it)
        delete *it;

	delete m_pMenuItemAction;
}
//--------------------------------------------------------------------------------------------------------
string MenuItem::Description()
{
	return m_sDescription;
}
//--------------------------------------------------------------------------------------------------------
MenuItemAction *MenuItem::Action()
{
	return m_pMenuItemAction;
}
//--------------------------------------------------------------------------------------------------------
bool MenuItem::CanGoRight()
{
	//TODO: implemented me
	return true;
}
//--------------------------------------------------------------------------------------------------------
bool MenuItem::CanGoLeft()
{
	//TODO: implemented me
	return true;
}
//--------------------------------------------------------------------------------------------------------
bool MenuItem::CanGoUp()
{
	//TODO: implemented me
	return true;
}
//--------------------------------------------------------------------------------------------------------
bool MenuItem::CanGoDown()
{
	//TODO: implemented me
	return true;
}
//--------------------------------------------------------------------------------------------------------
MenuItem *MenuItem::Parent()
{
	return m_pParent;
}
//--------------------------------------------------------------------------------------------------------
void MenuItem::AddChild(MenuItem *pMenuItem)
{
	if(NULL != pMenuItem)
		m_vectChildren.push_back(pMenuItem);
}
//--------------------------------------------------------------------------------------------------------
