#ifndef __MENU_ITEM_ACTION_H__
#define __MENU_ITEM_ACTION_H__
//--------------------------------------------------------------------------------------------------------
#include <string>
#include <map>
using namespace std;
//--------------------------------------------------------------------------------------------------------
enum ActionType
{
	atExecuteScript,
	atExecuteCommand,
	atCallMethod
};
//--------------------------------------------------------------------------------------------------------
class MenuItemAction
{
	string m_sAction;
	ActionType m_type;
	map<int, string> m_mapParameters;

public:

	MenuItemAction(string sAction, ActionType type);
	MenuItemAction(string sAction, ActionType type, const map<int, string>& mapParameters);
	~MenuItemAction();
};
//--------------------------------------------------------------------------------------------------------
#endif //__MENU_ITEM_ACTION_H__
