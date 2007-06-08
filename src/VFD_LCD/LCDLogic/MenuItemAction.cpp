#include "MenuItemAction.h"
//--------------------------------------------------------------------------------------------------------
MenuItemAction::MenuItemAction(string sAction, ActionType type) : 
	m_sAction(sAction), m_type(type)
{
}
//--------------------------------------------------------------------------------------------------------
MenuItemAction::MenuItemAction(string sAction, ActionType type, const map<int, string>& mapParameters) :
	m_sAction(sAction), m_type(type), m_mapParameters(mapParameters)
{

}
//--------------------------------------------------------------------------------------------------------
MenuItemAction::~MenuItemAction()
{
}
//--------------------------------------------------------------------------------------------------------
string MenuItemAction::Description()
{
	return m_sAction;
}
//--------------------------------------------------------------------------------------------------------
ActionType MenuItemAction::Type()
{
	return m_type;
}
//--------------------------------------------------------------------------------------------------------
const map<int, string>& MenuItemAction::Parameters() const
{
	return m_mapParameters;
}
//--------------------------------------------------------------------------------------------------------
void MenuItemAction::UpdateInputBoxParam(string sValue)
{
	m_mapParameters[ptInputBox] = sValue;
}
//--------------------------------------------------------------------------------------------------------
