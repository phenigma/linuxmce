#include "OrbiterPopupFactory.h"
#include "TaskManager.h"
#include "CallBackData.h"
#include "Logger.h"
using namespace DCE;

#include "Win32/Win32Popup.h"
//--------------------------------------------------------------------------------------------------------------
void *OrbiterPopupFactoryWorkerThread(void *p);
//--------------------------------------------------------------------------------------------------------------
OrbiterPopupFactoryImpl::OrbiterPopupFactoryImpl(void) : m_WorkerThreadID(0), m_bAbortWorkerThread(false)
{
}
//--------------------------------------------------------------------------------------------------------------
OrbiterPopupFactoryImpl::~OrbiterPopupFactoryImpl(void)
{
}
//--------------------------------------------------------------------------------------------------------------
void OrbiterPopupFactoryImpl::Activate()
{
	pthread_create(&m_WorkerThreadID, NULL, OrbiterPopupFactoryWorkerThread, NULL);
}
//--------------------------------------------------------------------------------------------------------------
void OrbiterPopupFactoryImpl::Deactivate()
{
	m_bAbortWorkerThread = true;

	if(m_WorkerThreadID > 0)
		pthread_join(m_WorkerThreadID, NULL);

	m_WorkerThreadID = 0;
}
//--------------------------------------------------------------------------------------------------------------
bool OrbiterPopupFactoryImpl::WorkerThreadAborted()
{
	return m_bAbortWorkerThread;
}
//--------------------------------------------------------------------------------------------------------------
void *OrbiterPopupFactoryWorkerThread(void *p)
{
	while(!OrbiterPopupFactory::Instance().WorkerThreadAborted())
	{
		if(
			TaskManager::Instance().GetTopTaskType() == cbOnDialogCreate &&
			TaskManager::Instance().GetDialogTypeForTopTask() > E_Dialog_Last_Wx_Dialog
		)
		{
			Task *pTask = TaskManager::Instance().PopTask();
            if(NULL != pTask)
			{
				PopupCallBackData *pPopupCallBackData = dynamic_cast<PopupCallBackData *>(pTask->pCallBackData);
				if(NULL == pPopupCallBackData)
				{
					g_pPlutoLogger->Write(LV_CRITICAL, "OrbiterPopupFactoryWorkerThread: got task with invalid callback data!");	
					delete pTask;
					continue;
				}

				//it will selfdestruct on close dialog
				(new Win32Popup(pTask->DialogType, pPopupCallBackData))->Initialize();

				TaskManager::Instance().TaskProcessed(pTask->TaskId);

				delete pTask->pCallBackData;
				pTask->pCallBackData = NULL;
				delete pTask;
			}
			else
				g_pPlutoLogger->Write(LV_CRITICAL, "OrbiterPopupFactoryWorkerThread: got invalid task!");
		}
		else
			Sleep(100);
	}

	return NULL;
}
//--------------------------------------------------------------------------------------------------------------