#include "PopupManager.h"
#include "TaskManager.h"
#include "CallBackData.h"
#include "OrbiterPopupFactory.h"

#ifdef WIN32
	#include "Win32/OrbiterWin32Defs.h"
#endif

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
E_DIALOG_TYPE PopupManagerImpl::GetDialogTypeOfPopupObj(int nBaseObjectID)
{
#ifdef WIN32
	return E_Dialog_Dummy_Orbiter_Dialog;
#endif

	switch(nBaseObjectID)
	{
		case 12345: //speed control's design obj
			return E_Dialog_SpeedControl;
		
		default:
			break;
	}

	return E_Dialog_Undefined;
}
//--------------------------------------------------------------------------------------------------------------
bool PopupManagerImpl::CreatePopup(E_DIALOG_TYPE aDialogType, PopupCallBackData *pCallBackData)
{
	AdjustPopupPositionInfo(pCallBackData);

	pCallBackData->m_ulPopupID = 0;
	Task *pTask = TaskManager::Instance().CreateTask(cbOnDialogCreate, aDialogType, pCallBackData);
	TaskManager::Instance().AddTaskAndWait(pTask);
	mapPopups.insert(make_pair(pCallBackData->m_ulPopupID, aDialogType));

	return true;
}
//--------------------------------------------------------------------------------------------------------------
bool PopupManagerImpl::CreatePopup(E_DIALOG_TYPE aDialogType, PopupCallBackData *pCallBackData, unsigned long& ulPopupID)
{
	AdjustPopupPositionInfo(pCallBackData);

	pCallBackData->m_ulPopupID = ++m_ulPopupCounter;

	Task *pTask = TaskManager::Instance().CreateTask(cbOnDialogCreate, aDialogType, pCallBackData);
	TaskManager::Instance().AddTaskAndWait(pTask);
	mapPopups.insert(make_pair(pCallBackData->m_ulPopupID, aDialogType));

	return true;
}
//--------------------------------------------------------------------------------------------------------------
bool PopupManagerImpl::RefreshPopup(E_DIALOG_TYPE aDialogType, PopupCallBackData *pCallBackData)
{
	AdjustPopupPositionInfo(pCallBackData);

	Task *pTask = TaskManager::Instance().CreateTask(cbOnDialogRefresh, aDialogType, pCallBackData);
	TaskManager::Instance().AddTaskAndWait(pTask);

	return true;
}
//--------------------------------------------------------------------------------------------------------------
bool PopupManagerImpl::RefreshPopup(const unsigned long ulPopupID, PopupCallBackData *pCallBackData)
{
	AdjustPopupPositionInfo(pCallBackData);

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
	AdjustPopupPositionInfo(pCallBackData);

	Task *pTask = TaskManager::Instance().CreateTask(cbOnDialogDelete, aDialogType, pCallBackData);
	TaskManager::Instance().AddTask(pTask);

	return true;
}
//--------------------------------------------------------------------------------------------------------------
bool PopupManagerImpl::ClosePopup(const unsigned long ulPopupID, PopupCallBackData *pCallBackData/* = NULL*/)
{
	AdjustPopupPositionInfo(pCallBackData);

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
void PopupManagerImpl::AdjustPopupPositionInfo(PopupCallBackData *pCallBackData)
{
	if(NULL != pCallBackData)
	{
#ifdef WIN32
		PlutoPoint point;
		ORBITER_CLASS::GetInstance()->GetWindowPosition(point);
		pCallBackData->m_rectPosition.X += point.X;
		pCallBackData->m_rectPosition.Y += point.Y;
#endif 
	}
}
//--------------------------------------------------------------------------------------------------------------