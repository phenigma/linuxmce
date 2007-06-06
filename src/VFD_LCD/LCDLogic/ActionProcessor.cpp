#include "ActionProcessor.h"
#include "MenuItemAction.h"
//--------------------------------------------------------------------------------------------------------
#include "DCE/Command_Impl.h"
#include "DCE/Logger.h"
using namespace DCE;
//--------------------------------------------------------------------------------------------------------
ActionProcessor::ActionProcessor(Command_Impl *pCommandImpl) : m_pCommand_Impl(pCommandImpl)
{
}
//--------------------------------------------------------------------------------------------------------
ActionProcessor::~ActionProcessor()
{
	m_pCommand_Impl = NULL;
}
//--------------------------------------------------------------------------------------------------------
void ActionProcessor::ProcessAction(MenuItemAction *pAction)
{
	DCE::LoggerWrapper::GetInstance()->Write(LV_WARNING, "Ready to execute action %s, type %d with %d parameters",
		pAction->Description().c_str(), pAction->Type(), pAction->Parameters().size());
}
//--------------------------------------------------------------------------------------------------------
