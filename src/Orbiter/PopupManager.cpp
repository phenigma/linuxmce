#include "PopupManager.h"
#include "TaskManager.h"
#include "CallBackData.h"
#include "OrbiterPopupFactory.h"

#include "Logger.h"
using namespace DCE;
//--------------------------------------------------------------------------------------------------------------
PopupManagerImpl::PopupManagerImpl(void) : m_ulPopupCounter(0)
{
#ifdef WIN32
	OrbiterPopupFactory::Instance().Activate();
#endif
}
//--------------------------------------------------------------------------------------------------------------
PopupManagerImpl::~PopupManagerImpl(void)
{
#ifdef WIN32
	OrbiterPopupFactory::Instance().Deactivate();
#endif
}
//--------------------------------------------------------------------------------------------------------------
bool PopupManagerImpl::CreatePopup(E_DIALOG_TYPE aDialogType, PopupCallBackData *pCallBackData)
{
	pCallBackData->m_ulPopupID = 0;
	Task *pTask = TaskManager::Instance().CreateTask(cbOnDialogCreate, aDialogType, pCallBackData);
	TaskManager::Instance().AddTaskAndWait(pTask);
	mapPopups.insert(make_pair(pCallBackData->m_ulPopupID, aDialogType));

	return true;
}
//--------------------------------------------------------------------------------------------------------------
bool PopupManagerImpl::CreatePopup(E_DIALOG_TYPE aDialogType, PopupCallBackData *pCallBackData, unsigned long& ulPopupID)
{
	pCallBackData->m_ulPopupID = ++m_ulPopupCounter;

	Task *pTask = TaskManager::Instance().CreateTask(cbOnDialogCreate, aDialogType, pCallBackData);
	TaskManager::Instance().AddTaskAndWait(pTask);
	mapPopups.insert(make_pair(pCallBackData->m_ulPopupID, aDialogType));

	return true;
}
//--------------------------------------------------------------------------------------------------------------
bool PopupManagerImpl::RefreshPopup(E_DIALOG_TYPE aDialogType, PopupCallBackData *pCallBackData)
{
	Task *pTask = TaskManager::Instance().CreateTask(cbOnDialogRefresh, aDialogType, pCallBackData);
	TaskManager::Instance().AddTaskAndWait(pTask);

	return true;
}
//--------------------------------------------------------------------------------------------------------------
bool PopupManagerImpl::RefreshPopup(const unsigned long ulPopupID, PopupCallBackData *pCallBackData)
{
	map<unsigned long, E_DIALOG_TYPE>::iterator it = mapPopups.find(ulPopupID);
	if(it == mapPopups.end())
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "No popup with id %d found!", ulPopupID);
		return false;
	}

	pCallBackData->m_ulPopupID = ulPopupID;
	Task *pTask = TaskManager::Instance().CreateTask(cbOnDialogRefresh, it->second, pCallBackData);
	TaskManager::Instance().AddTaskAndWait(pTask);

	return true;
}
//--------------------------------------------------------------------------------------------------------------
bool PopupManagerImpl::ShowPopup(E_DIALOG_TYPE aDialogType, PopupCallBackData *pCallBackData/* = NULL*/)
{
	//TODO: for window manager controller

	return true;
}
//--------------------------------------------------------------------------------------------------------------
bool PopupManagerImpl::ShowPopup(const unsigned long ulPopupID, PopupCallBackData *pCallBackData/* = NULL*/)
{
	//TODO: for window manager controller

	return true;
}
//--------------------------------------------------------------------------------------------------------------
bool PopupManagerImpl::HidePopup(E_DIALOG_TYPE aDialogType, PopupCallBackData *pCallBackData/* = NULL*/)
{
	//TODO: for window manager controller

	return true;
}
//--------------------------------------------------------------------------------------------------------------
bool PopupManagerImpl::HidePopup(const unsigned long ulPopupID, PopupCallBackData *pCallBackData/* = NULL*/)
{
	//TODO: for window manager controller

	return true;
}
//--------------------------------------------------------------------------------------------------------------
bool PopupManagerImpl::ClosePopup(E_DIALOG_TYPE aDialogType, PopupCallBackData *pCallBackData/* = NULL*/)
{
	Task *pTask = TaskManager::Instance().CreateTask(cbOnDialogDelete, aDialogType, pCallBackData);
	TaskManager::Instance().AddTask(pTask);

	return true;
}
//--------------------------------------------------------------------------------------------------------------
bool PopupManagerImpl::ClosePopup(const unsigned long ulPopupID, PopupCallBackData *pCallBackData/* = NULL*/)
{
	map<unsigned long, E_DIALOG_TYPE>::iterator it = mapPopups.find(ulPopupID);
	if(it == mapPopups.end())
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "No popup with id %d found!", ulPopupID);
		return false;
	}

	pCallBackData->m_ulPopupID = ulPopupID;
	Task *pTask = TaskManager::Instance().CreateTask(cbOnDialogDelete, it->second, pCallBackData);
	TaskManager::Instance().AddTask(pTask);

	return true;
}
//--------------------------------------------------------------------------------------------------------------
bool PopupManagerImpl::AnimatePopup(E_DIALOG_TYPE aDialogType, PopupCallBackData *pCallBackData, 
				  AnimationType aAnimationType, PopupAnimationData *pAnimationData)
{
	//TODO: for window manager controller

	return true;
}
//--------------------------------------------------------------------------------------------------------------
bool PopupManagerImpl::AnimatePopup(const unsigned long ulPopupID, PopupCallBackData *pCallBackData, 
				  AnimationType aAnimationType, PopupAnimationData *pAnimationData)
{
	//TODO: for window manager controller

	return true;
}
//--------------------------------------------------------------------------------------------------------------
