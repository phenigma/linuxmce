#include "OrbiterRenderer_SDL_Linux.h"

#include "../Orbiter.h"
#include "OrbiterLinux.h"
using namespace DCE;

#include <stdlib.h>
#define USE_X11_LIB (! USE_WX_LIB)

#if (USE_WX_LIB)
#include "../wxAppMain/wx_all_include.cpp"
#include "../wxAppMain/wx_safe_dialog.h"
#include "../wxAppMain/wxdialog_waitgrid.h"
#include "../wxAppMain/wxdialog_waitlist.h"
#include "../wxAppMain/wxdialog_waituser.h"
#define USE_TASK_MANAGER true
#include "../Task.h"
#include "../TaskManager.h"
#endif // (USE_WX_LIB)

#include "DCE/Logger.h"
#include "pluto_main/Define_DesignObj.h"

#include <iomanip>
#include <sstream>
#include <sys/time.h>

#include <X11/Xutil.h>
#include <X11/Xproto.h>
#include <X11/keysymdef.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>

#include <SDL/SDL_syswm.h>
#include "../CallBackTypes.h"
#include "../dialog_types.h"

OrbiterRenderer_SDL_Linux::OrbiterRenderer_SDL_Linux(Orbiter *pOrbiter) : OrbiterRenderer_SDL(pOrbiter)
{

}

OrbiterRenderer_SDL_Linux::~OrbiterRenderer_SDL_Linux()
{

}

void OrbiterRenderer_SDL_Linux::RenderScreen( bool bRenderGraphicsOnly )
{
	if( bRenderGraphicsOnly )
	{
		OrbiterRenderer_SDL::RenderScreen( bRenderGraphicsOnly );
		return;
	}

	OrbiterLinux *pOrbiterLinux = dynamic_cast<OrbiterLinux *>(OrbiterLogic());
	if(NULL != pOrbiterLinux)
	{
		pOrbiterLinux->m_pWinListManager->HideAllWindows();
		pOrbiterLinux->m_bIsExclusiveMode = true;

		{
			X11_Locker lock(pOrbiterLinux->GetDisplay());
			OrbiterRenderer_SDL::RenderScreen(bRenderGraphicsOnly);
			XFlush(pOrbiterLinux->GetDisplay()); // TODO: test and remove this
		}

		if(pOrbiterLinux->m_bOrbiterReady)
			pOrbiterLinux->m_pWinListManager->ShowSdlWindow(pOrbiterLinux->m_bIsExclusiveMode);
	}
}

void OrbiterRenderer_SDL_Linux::InitializeAfterSetVideoMode()
{
	OrbiterLinux *pOrbiterLinux = dynamic_cast<OrbiterLinux *>(OrbiterLogic());
	if(NULL != pOrbiterLinux)
	{
		g_pPlutoLogger->Write(LV_WARNING, "OrbiterLinux::InitializeAfterSetVideoMode()");
		pOrbiterLinux->X11_Init();
		g_pPlutoLogger->Write(LV_STATUS, "OrbiterLinux::InitializeAfterSetVideoMode() : HideOtherWindows");
		pOrbiterLinux->HideOtherWindows();
		g_pPlutoLogger->Write(LV_STATUS, "OrbiterLinux::InitializeAfterSetVideoMode() : done");
	}
}

void OrbiterRenderer_SDL_Linux::InitializeAfterRelatives()
{
	OrbiterLinux *pOrbiterLinux = dynamic_cast<OrbiterLinux *>(OrbiterLogic());
	if(NULL != pOrbiterLinux)
	{
		// allow initial "extern" dialogs to receive clicks until this point
		g_pPlutoLogger->Write(LV_WARNING, "OrbiterLinux::InitializeAfterRelatives()");
		pOrbiterLinux->GrabPointer(true);
		pOrbiterLinux->GrabKeyboard(true);
		g_pPlutoLogger->Write(LV_WARNING, "OrbiterLinux::InitializeAfterRelatives() : done");
	}
}

bool OrbiterRenderer_SDL_Linux::DisplayProgress(string sMessage, const map<string, bool> &mapChildDevices, int nProgress)
{
    std::cout << "== DisplayProgress( " << sMessage << ", ChildDevices, " << nProgress << " );" << std::endl;

	OrbiterLinux *pOrbiterLinux = dynamic_cast<OrbiterLinux *>(OrbiterLogic());
	if(NULL ==	pOrbiterLinux)
		return false;

/////////////////////////////////////////////////////////////////
//  WX dialogs
/////////////////////////////////////////////////////////////////
#if (USE_WX_LIB)
#if (USE_TASK_MANAGER)
	static bool bDialogRunning = false;
	PositionCallBackData *pCallBackData = new WaitUserGridCallBackData(sMessage, mapChildDevices, nProgress);
    pCallBackData->m_bShowFullScreen = true;
	CallBackType callbackType = cbOnDialogRefresh;
	if(nProgress != -1)
	{
		if(!bDialogRunning)
		{
			callbackType = cbOnDialogCreate;
			bDialogRunning = true;
		}
		//else assuming it's a refresh
	}
	else
	{
		callbackType = cbOnDialogDelete;
		bDialogRunning = false;
	}
	Task *pTask = TaskManager::Instance().CreateTask(callbackType, E_Dialog_WaitGrid, pCallBackData);
    if (callbackType == cbOnDialogCreate)
    {
        TaskManager::Instance().AddTaskAndWait(pTask);
        m_pWinListManager->MaximizeWindow("dialog.dialog");
    }
    else
    {
        if (callbackType == cbOnDialogDelete)
		{
            m_pWinListManager->HideAllWindows();
			TaskManager::Instance().AddTaskAndWait(pTask);
		}
		else
        	TaskManager::Instance().AddTask(pTask);
    }
#else // (USE_TASK_MANAGER)
    if ( (m_pWaitGrid != NULL) && m_bButtonPressed_WaitGrid )
    {
        // window already closed
        m_pWaitGrid = NULL;
        // notify by return value
        return true;
    }
    if ( (m_pWaitGrid == NULL) && (nProgress >= 0) )
    {
        // create new window
        m_pWaitGrid = Safe_CreateUnique<wxDialog_WaitGrid>();
        Safe_Show<wxDialog_WaitGrid>(m_pWaitGrid);
        m_pWinListManager->MaximizeWindow("dialog.dialog");
        return false;
    }
    if ( (m_pWaitGrid != NULL) && (nProgress >= 0) )
    {
        // update
        wxDialog_WaitGrid::Data_Refresh data_refresh = { sMessage, mapChildDevices, nProgress };
        Safe_Gui_Refresh<wxDialog_WaitGrid>(m_pWaitGrid, &data_refresh);
        return false;
    }
    if ( (m_pWaitGrid != NULL) && (nProgress < 0) )
    {
        Safe_Close<wxDialog_WaitGrid>(m_pWaitGrid);
        m_pWaitGrid = NULL;
        return false;
    }
#endif // (USE_TASK_MANAGER)
#endif // (USE_WX_LIB)

/////////////////////////////////////////////////////////////////
//  NON - WX dialogs
/////////////////////////////////////////////////////////////////
#if (USE_X11_LIB)
    sMessage += ": ";
    for(map<string, bool>::const_iterator it = mapChildDevices.begin(); it != mapChildDevices.end(); ++it)
        if(!it->second)
            sMessage += StringUtils::Replace(it->first, "|", "# ") + ", ";
    std::cout << "== DisplayProgress( " << sMessage << ", ChildDevices, " << nProgress << " );" << std::endl;
    if (pOrbiterLinux->m_pProgressWnd && pOrbiterLinux->m_pProgressWnd->IsCancelled())
    {
        delete pOrbiterLinux->m_pProgressWnd;
        pOrbiterLinux->m_pProgressWnd = NULL;
        return true;
    }
    if (nProgress != -1 && !pOrbiterLinux->m_pProgressWnd)
    {
        // Create the progress window ...
        pOrbiterLinux->m_pProgressWnd = new XProgressWnd();
        pOrbiterLinux->m_pProgressWnd->UpdateProgress(sMessage, nProgress);
        pOrbiterLinux->m_pProgressWnd->Run();
        pOrbiterLinux->m_pWinListManager->MaximizeWindow(pOrbiterLinux->m_pProgressWnd->m_wndName);
        return false;
    }
    /*else*/ if (nProgress != -1)
    {
        // Update progress info
        pOrbiterLinux->m_pProgressWnd->UpdateProgress(sMessage, nProgress);
        pOrbiterLinux->m_pProgressWnd->DrawWindow();
        return false;
    }
    /*else*/ if(pOrbiterLinux->m_pProgressWnd)
    {
        // We are done here ...
        pOrbiterLinux->m_pProgressWnd->Terminate();
        pOrbiterLinux->m_pProgressWnd = NULL;
        pOrbiterLinux->reinitGraphics();
        return false;
    }
#endif // (USE_X11_LIB)

    return false;
}

bool OrbiterRenderer_SDL_Linux::DisplayProgress(string sMessage, int nProgress)
{
    std::cout << "== DisplayProgress waitlist( " << sMessage << ", " << nProgress << " );" << std::endl;

	OrbiterLinux *pOrbiterLinux = dynamic_cast<OrbiterLinux *>(OrbiterLogic());
	if(NULL ==	pOrbiterLinux)
		return false;

#if (USE_WX_LIB)
#if (USE_TASK_MANAGER)
	static bool bDialogRunning = false;
	PositionCallBackData *pCallBackData = new WaitUserListCallBackData(sMessage, nProgress);
    pCallBackData->m_bShowFullScreen = true;
	CallBackType callbackType = cbOnDialogRefresh;
	if(nProgress != -1)
	{
		if(!bDialogRunning)
		{
			callbackType = cbOnDialogCreate;
			bDialogRunning = true;
        }
		//else assuming it's a refresh
	}
	else
	{
		callbackType = cbOnDialogDelete;
		bDialogRunning = false;
	}
	Task *pTask = TaskManager::Instance().CreateTask(callbackType, E_Dialog_WaitList, pCallBackData);
    if (callbackType == cbOnDialogCreate)
    {
        TaskManager::Instance().AddTaskAndWait(pTask);
        pOrbiterLinux->m_pWinListManager->MaximizeWindow("dialog.dialog");
    }
    else
    {
        if (callbackType == cbOnDialogDelete)
		{
            pOrbiterLinux->m_pWinListManager->HideAllWindows();
			TaskManager::Instance().AddTaskAndWait(pTask);
		}
		else
        	TaskManager::Instance().AddTask(pTask);
    }
#else // (USE_TASK_MANAGER)
	//TODO:
    if ( (m_pWaitList != NULL) && m_bButtonPressed_WaitList )
    {
        // window already closed
        m_pWaitList = NULL;
        // notify by return value
        return true;
    }
    if ( (m_pWaitList == NULL) && (nProgress >= 0) )
    {
        // create new window
        m_pWaitList = Safe_CreateUnique<wxDialog_WaitList>();
        Safe_Show<wxDialog_WaitList>(m_pWaitList);
        m_pWinListManager->MaximizeWindow("dialog.dialog");
        return false;
    }
    if ( (m_pWaitList != NULL) && (nProgress >= 0) )
    {
        // update
        wxDialog_WaitList::Data_Refresh data_refresh = { sMessage, nProgress };
        Safe_Gui_Refresh<wxDialog_WaitList>(m_pWaitList, &data_refresh);
        return false;
    }
	if ( (m_pWaitList != NULL) && (nProgress < 0) )
    {
        Safe_Close<wxDialog_WaitList>(m_pWaitList);
        m_pWaitList = NULL;
        return false;
    }
#endif // (USE_TASK_MANAGER)
#endif // (USE_WX_LIB)

#if (USE_X11_LIB)
    if (pOrbiterLinux->m_pProgressWnd && pOrbiterLinux->m_pProgressWnd->IsCancelled())
    {
        delete pOrbiterLinux->m_pProgressWnd;
        pOrbiterLinux->m_pProgressWnd = NULL;
        return true;
    }
	if (nProgress != -1 && !pOrbiterLinux->m_pProgressWnd)
    {
        // Create the progress window ...
        pOrbiterLinux->m_pProgressWnd = new XProgressWnd();
        pOrbiterLinux->m_pProgressWnd->UpdateProgress(sMessage, nProgress);
        pOrbiterLinux->m_pProgressWnd->Run();
        pOrbiterLinux->m_pWinListManager->MaximizeWindow(pOrbiterLinux->m_pProgressWnd->m_wndName);
        return false;
    }
    /*else*/ if (nProgress != -1)
    {
        // Update progress info
        pOrbiterLinux->m_pProgressWnd->UpdateProgress(sMessage, nProgress);
        pOrbiterLinux->m_pProgressWnd->DrawWindow();
        return false;
    }
    /*else*/ if(pOrbiterLinux->m_pProgressWnd)
    {
        // We are done here ...
        pOrbiterLinux->m_pProgressWnd->Terminate();
        pOrbiterLinux->m_pProgressWnd = NULL;
        pOrbiterLinux->reinitGraphics();
        return false;
    }
#endif // (USE_X11_LIB)

    return false;
}

int OrbiterRenderer_SDL_Linux::PromptUser(string sPrompt, int iTimeoutSeconds, map<int,string> *p_mapPrompts)
{
    map<int,string> mapPrompts;
    mapPrompts[PROMPT_CANCEL]    = "Ok";
    if (p_mapPrompts == NULL) {
        p_mapPrompts = &mapPrompts;
    }
    std::cout << "== PromptUser( " << sPrompt << ", " << iTimeoutSeconds << ", " << p_mapPrompts << " );" << std::endl;

	OrbiterLinux *pOrbiterLinux = dynamic_cast<OrbiterLinux *>(OrbiterLogic());
	if(NULL ==	pOrbiterLinux)
		return false;

#if (USE_WX_LIB)
#if (USE_TASK_MANAGER)
	PositionCallBackData *pCallBackData = new WaitUserPromptCallBackData(sPrompt, iTimeoutSeconds, *p_mapPrompts);
    pCallBackData->m_bShowFullScreen = true;
	Task *pTask = TaskManager::Instance().CreateTask(cbOnDialogCreate, E_Dialog_WaitUser, pCallBackData);
	TaskManager::Instance().AddTaskAndWait(pTask);
    m_pWinListManager->MaximizeWindow("dialog.dialog");
	Task *pTaskWait = TaskManager::Instance().CreateTask(cbOnDialogWaitUser, E_Dialog_WaitUser, pCallBackData);
	TaskManager::Instance().AddTaskAndWait(pTaskWait);
    std::cout << "== PromptUser( " << sPrompt << ", " << iTimeoutSeconds << ", " << p_mapPrompts << " );" << std::endl;
#else // (USE_TASK_MANAGER)
    m_pWaitUser = Safe_CreateUnique<wxDialog_WaitUser>();
    m_pWinListManager->MaximizeWindow("dialog.dialog");
    int nButtonId = Safe_ShowModal<wxDialog_WaitUser>(m_pWaitUser);
    return nButtonId;
#endif // (USE_TASK_MANAGER)
#endif // (USE_WX_LIB)

#if (USE_X11_LIB)
    XPromptUser promptDlg(sPrompt, iTimeoutSeconds, p_mapPrompts);
    promptDlg.SetButtonPlacement(XPromptUser::BTN_VERT);
    promptDlg.Init();
    pOrbiterLinux->m_pWinListManager->MaximizeWindow(promptDlg.m_wndName);
    int nUserAnswer = promptDlg.RunModal();
    promptDlg.DeInit();
    return nUserAnswer;
#endif // (USE_X11_LIB)

	return 0;
}

void OrbiterRenderer_SDL_Linux::LockDisplay()
{
	OrbiterLinux *pOrbiterLinux = dynamic_cast<OrbiterLinux *>(OrbiterLogic());
    if (pOrbiterLinux)
        pOrbiterLinux->X_LockDisplay();
}

void OrbiterRenderer_SDL_Linux::UnlockDisplay()
{
	OrbiterLinux *pOrbiterLinux = dynamic_cast<OrbiterLinux *>(OrbiterLogic());
    if (pOrbiterLinux)
        pOrbiterLinux->X_UnlockDisplay();
}
