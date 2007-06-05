#include "MenuHolder.h"
#include "MenuItem.h"
#include "MenuItemAction.h"

#include "DCE/Logger.h"
//--------------------------------------------------------------------------------------------------------
MenuHolder::MenuHolder() : m_pRootMenuItem(NULL), m_pCurrentMenuItem(NULL)
{

}
//--------------------------------------------------------------------------------------------------------
MenuHolder::~MenuHolder()
{
	delete m_pRootMenuItem;
}
//--------------------------------------------------------------------------------------------------------
MenuItem *MenuHolder::RootMenu()
{
	if(NULL == m_pRootMenuItem)
		m_pRootMenuItem = new MenuItem("root");

	return m_pRootMenuItem;
}
//--------------------------------------------------------------------------------------------------------
MenuItem *MenuHolder::CurrentMenuItem()
{
	if(NULL == m_pCurrentMenuItem) 
		m_pCurrentMenuItem = m_pRootMenuItem->FirstChild();

	return m_pCurrentMenuItem;
}
//--------------------------------------------------------------------------------------------------------
void MenuHolder::MoveUp()
{
	MenuItem *pNextMenuItem = m_pCurrentMenuItem->Parent()->PrevChild(m_pCurrentMenuItem);

	if(NULL != pNextMenuItem)
		m_pCurrentMenuItem = pNextMenuItem;
}
//--------------------------------------------------------------------------------------------------------
void MenuHolder::MoveDown()
{
	MenuItem *pNextMenuItem = m_pCurrentMenuItem->Parent()->NextChild(m_pCurrentMenuItem);

	if(NULL != pNextMenuItem)
		m_pCurrentMenuItem = pNextMenuItem;
}
//--------------------------------------------------------------------------------------------------------
void MenuHolder::MoveLeft()
{
	if(m_pCurrentMenuItem->Parent() != m_pRootMenuItem)
		m_pCurrentMenuItem = m_pCurrentMenuItem->Parent();
}
//--------------------------------------------------------------------------------------------------------
void MenuHolder::MoveRight()
{
	if(NULL != m_pCurrentMenuItem->FirstChild())
	{
		m_pCurrentMenuItem = m_pCurrentMenuItem->FirstChild();
	}
	else
	{
        //this is a leaf; execute any actions needed
		MenuItemAction *pAction = m_pCurrentMenuItem->Action();

		DCE::LoggerWrapper::GetInstance()->Write(LV_WARNING, "Ready to execute action %s, type %d with %d parameters",
			pAction->Description().c_str(), pAction->Type(), pAction->Parameters().size()
		);
	}
}
//--------------------------------------------------------------------------------------------------------
