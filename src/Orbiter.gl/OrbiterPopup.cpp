#include "OrbiterPopup.h"
#include "Task.h"
#include "TaskManager.h"
#include "CallBackData.h"
//--------------------------------------------------------------------------------------------------------------
OrbiterPopup::OrbiterPopup(E_DIALOG_TYPE aDialogType, PopupCallBackData *pCallBackData) :
	m_aDialogType(aDialogType),
	m_ulPopupID(0)
{
	if(NULL != pCallBackData)
	{
		m_ulPopupID = pCallBackData->m_ulPopupID;
		m_rect = pCallBackData->m_rectPosition;
	}
}
//--------------------------------------------------------------------------------------------------------------
/*virtual*/ OrbiterPopup::~OrbiterPopup(void)
{

}
//--------------------------------------------------------------------------------------------------------------
/*virtual*/ Task *OrbiterPopup::GetTask()
{
	//my type of dialog and with my id? it must not be a create, only the factory can handle it
	if(
		cbOnDialogCreate != TaskManager::Instance().GetTopTaskType()		&&
		m_aDialogType == TaskManager::Instance().GetDialogTypeForTopTask()  &&
		m_ulPopupID == TaskManager::Instance().GetPopupIDForTopTask()		
	)
	{
		return TaskManager::Instance().PopTask();
	}

	return NULL;
}
//--------------------------------------------------------------------------------------------------------------