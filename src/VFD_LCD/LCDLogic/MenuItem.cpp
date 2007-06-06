#include "MenuItem.h"
#include "MenuItemAction.h"

#include <algorithm>

#include "DCE/Logger.h"
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
	return !m_vectChildren.empty();
}
//--------------------------------------------------------------------------------------------------------
bool MenuItem::CanGoLeft()
{
	return NULL != m_pParent && m_pParent->Description() != "root";
}
//--------------------------------------------------------------------------------------------------------
bool MenuItem::CanGoUp()
{
	return NULL != m_pParent && m_pParent->PrevChild(this);
}
//--------------------------------------------------------------------------------------------------------
bool MenuItem::CanGoDown()
{
	return NULL != m_pParent && m_pParent->NextChild(this);
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
MenuItem* MenuItem::FirstChild()
{
	if(!m_vectChildren.empty())
		return m_vectChildren[0];

	return NULL;
}
//--------------------------------------------------------------------------------------------------------
MenuItem* MenuItem::NextChild(MenuItem *pChildMenuItem)
{
	vector<MenuItem *>::iterator it = std::find(m_vectChildren.begin(), m_vectChildren.end(), pChildMenuItem);

	if(it == m_vectChildren.end())
	{
		DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "%p/%s is not child for %p/%s",
			pChildMenuItem, pChildMenuItem->Description().c_str(),
			this, Description().c_str()
		);

		return NULL;
	}

	++it;

	if(it == m_vectChildren.end())
		return NULL;

	return *it;
}
//--------------------------------------------------------------------------------------------------------
MenuItem* MenuItem::PrevChild(MenuItem *pChildMenuItem)
{
	vector<MenuItem *>::iterator it = std::find(m_vectChildren.begin(), m_vectChildren.end(), pChildMenuItem);

	if(it == m_vectChildren.end())
	{
		DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "%p/%s is not child for %p/%s",
			pChildMenuItem, pChildMenuItem->Description().c_str(),
			this, Description().c_str()
		);

		return NULL;
	}

	if(!m_vectChildren.empty() && pChildMenuItem == *m_vectChildren.begin())
		return NULL;

	--it;

	return *it;
}
//--------------------------------------------------------------------------------------------------------