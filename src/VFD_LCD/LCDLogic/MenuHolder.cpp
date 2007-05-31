#include "MenuHolder.h"
#include "MenuItem.h"
//--------------------------------------------------------------------------------------------------------
MenuHolder::MenuHolder() : m_pRootMenuItem(NULL), m_pCurrentMenuItem(NULL)
{
}
//--------------------------------------------------------------------------------------------------------
MenuHolder::~MenuHolder()
{
}
//--------------------------------------------------------------------------------------------------------
MenuItem *MenuHolder::RootMenu()
{
	if(NULL == m_pRootMenuItem)
		m_pRootMenuItem = new MenuItem("root");

	return m_pRootMenuItem;
}
//--------------------------------------------------------------------------------------------------------
