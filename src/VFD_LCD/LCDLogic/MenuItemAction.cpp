#include "MenuItemAction.h"
//--------------------------------------------------------------------------------------------------------
MenuItemAction::MenuItemAction(string sAction, ActionType type) : 
	m_sAction(sAction), m_type(type)
{
}
//--------------------------------------------------------------------------------------------------------
MenuItemAction::MenuItemAction(string sAction, ActionType type, const map<int, string>& mapParameters)
	m_sAction(sAction), m_type(type)
{
	m_mapParameters.assign(mapParameters.begin(), mapParameters.end());
}
//--------------------------------------------------------------------------------------------------------
MenuItemAction::~MenuItemAction()
{
}
//--------------------------------------------------------------------------------------------------------
