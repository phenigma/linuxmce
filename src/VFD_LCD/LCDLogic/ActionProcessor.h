#ifndef __ACTION_PROCESSOR_H__
#define __ACTION_PROCESSOR_H__
//--------------------------------------------------------------------------------------------------------
namespace DCE
{
	class Command_Impl;
};
//--------------------------------------------------------------------------------------------------------
class MenuItemAction;
//--------------------------------------------------------------------------------------------------------
class ActionProcessor
{
private:

	DCE::Command_Impl *m_pCommand_Impl;

public:

	ActionProcessor(DCE::Command_Impl *pCommandImpl);
	~ActionProcessor();

	void ProcessAction(MenuItemAction *pAction);
};
//--------------------------------------------------------------------------------------------------------
#endif //__ACTION_PROCESSOR_H__
