//
// Author : C Remus
//
// Changed by : ...
//

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "wx_extern_app.h"
#endif
#include "wx/wxprec.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx_extern_app.h"
#include "wxthread_bag.h"

#ifdef USE_RELEASE_CODE
#  include "../Main.h"
#  include "../SDL/StartOrbiterSDL.h"
#endif // USE_RELEASE_CODE

ExternApp::ExternApp()
    : wxThread_Cmd(wxTHREAD_JOINABLE, "ExternApp")
{
    v_eStatus = E_UNINITIALIZED;
#ifdef USE_RELEASE_CODE
    v_pSDL_Event_Loop_Data = NULL;
#endif // USE_RELEASE_CODE
}

ExternApp::~ExternApp()
{
    Destroy();
}

bool ExternApp::Initialize()
{
    _WX_LOG_NFO();
#ifdef USE_RELEASE_CODE
    v_pSDL_Event_Loop_Data = new SDL_Event_Loop_Data;
    CommandLineParams commandlineparams;
    bool bStartedOK = ParseCommandLineParams(wxTheApp->argc, wxTheApp->argv, commandlineparams);
    if (! bStartedOK)
    {
        _WX_LOG_ERR("error returned by : ParseCommandLineParams()");
        return false;
    }
	v_pSDL_Event_Loop_Data->pOrbiter = CreateOrbiter(commandlineparams.PK_Device, commandlineparams.PK_DeviceTemplate, commandlineparams.sRouter_IP, commandlineparams.sLocalDirectory, commandlineparams.bLocalMode, commandlineparams.Width, commandlineparams.Height, commandlineparams.bFullScreen);
    if (v_pSDL_Event_Loop_Data->pOrbiter == NULL)
    {
        _WX_LOG_ERR("error returned by : CreateOrbiter()");
        return false;
    }
#endif // USE_RELEASE_CODE
    return true;
}

bool ExternApp::EventProcess()
{
    _WX_LOG_NFO();
    if (wxIdleThreadShouldStop())
    {
        _WX_LOG_NFO("Event process loop stopped");
        return false;
    }
#ifdef USE_RELEASE_CODE
    // if created, process events
    if (v_eStatus == E_CREATE_SUCCESS)
    {
        if (v_pSDL_Event_Loop_Data->pOrbiter)
        {
            if (! SDL_Event_Process(*v_pSDL_Event_Loop_Data))
                return false;
        }
        return true;
    }
    // create the app if needed, and process events on the next iteration
    if (v_eStatus == E_UNINITIALIZED)
    {
        v_eStatus = Initialize() ? E_CREATE_SUCCESS : E_CREATE_ERROR;
        return v_eStatus;
    }
#endif // USE_RELEASE_CODE
    return true;
}

bool ExternApp::Destroy()
{
    _WX_LOG_NFO();
    if (v_eStatus != E_CREATE_SUCCESS)
        return false;
#ifdef USE_RELEASE_CODE
    if (v_pSDL_Event_Loop_Data->pOrbiter && v_pSDL_Event_Loop_Data->pOrbiter->m_bReload)
        g_nExitCode = 2;
    SDL_Event_Loop_End(*v_pSDL_Event_Loop_Data);
    if (g_pPlutoLogger)
        delete g_pPlutoLogger;
#endif // USE_RELEASE_CODE
    v_eStatus = E_RUN_DONE;
    return true;
}

void ExternApp::Run()
{
    _WX_LOG_NFO();
    if (! Initialize())
        return;
    while (EventProcess())
    {
        wx_sleep(0, INTERVAL_EXTERNAPP_LOOP_MSEC);
    }
    Destroy();
}
