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

bool g_USE_EXTERN_APP_ON_IDLE = false;
bool g_USE_EXTERN_APP_ON_TIMER = false;
bool g_USE_EXTERN_APP_ON_THREAD = false;

#ifdef USE_RELEASE_CODE
#  include "../Main.h"
#  include "../SDL/StartOrbiterSDL.h"
#endif // USE_RELEASE_CODE

ExternApp::ExternApp(int argc, char *argv[])
        : wxThread_Cmd(wxTHREAD_JOINABLE, "ExternApp")
        , argc(argc)
        , argv(argv)
{
    _WX_LOG_NFO();
#ifdef USE_RELEASE_CODE
    v_pSDL_App_Object = NULL;
#endif // USE_RELEASE_CODE
}

ExternApp::~ExternApp()
{
    _WX_LOG_NFO();
#ifdef USE_RELEASE_CODE
    wxDELETE(v_pSDL_App_Object);
#endif // USE_RELEASE_CODE
}

void ExternApp::Run()
{
    _WX_LOG_NFO();
    if (! Create())
        return;
    EventProcess();
    Destroy();
}

bool ExternApp::Create()
{
    _WX_LOG_NFO();
#ifdef USE_RELEASE_CODE
    v_pSDL_App_Object = new SDL_App_Object(argc, argv);
    if (! v_pSDL_App_Object->LoadConfig())
    {
        _WX_LOG_ERR("error returned by : LoadConfig()");
        wxDELETE(v_pSDL_App_Object);
        return false;
    }
    if (! v_pSDL_App_Object->Create())
    {
        _WX_LOG_ERR("error returned by : Create()");
        wxDELETE(v_pSDL_App_Object);
        return false;
    }
#endif // USE_RELEASE_CODE
    return true;
}

bool ExternApp::EventProcess()
{
    _WX_LOG_NFO("Event Loop");
#ifdef USE_RELEASE_CODE
    while (v_pSDL_App_Object->EventProcess())
    {
        if (wxIdleThreadShouldStop())
        {
            _WX_LOG_WRN("Event process loop stopped");
            return false;
        }
    }
#endif // USE_RELEASE_CODE
    return true;
}

int ExternApp::Destroy()
{
    int nExitCode = 0;
#ifdef USE_RELEASE_CODE
    v_pSDL_App_Object->Destroy();
    wxDELETE(v_pSDL_App_Object);
#endif // USE_RELEASE_CODE
    return nExitCode;
}

int ExternApp_Run_NoWx(int argc, char *argv[])
{
    _WX_LOG_NFO("wx not running");
#ifdef USE_DEBUG_CODE
    wxUnusedVar(argc);
    wxUnusedVar(argv);
#endif // USE_DEBUG_CODE
#ifdef USE_RELEASE_CODE
    // CreateObjects
    SDL_Event_Loop_Data *pSDL_Event_Loop_Data = new SDL_Event_Loop_Data;
    CommandLineParams commandlineparams;
    bool bStartedOK = ParseCommandLineParams(argc, argv, commandlineparams);
    if (! bStartedOK)
    {
        _WX_LOG_ERR("error returned by : ParseCommandLineParams()");
        return false;
    }
	pSDL_Event_Loop_Data->pOrbiter = CreateOrbiter(commandlineparams.PK_Device, commandlineparams.PK_DeviceTemplate, commandlineparams.sRouter_IP, commandlineparams.sLocalDirectory, commandlineparams.bLocalMode, commandlineparams.Width, commandlineparams.Height, commandlineparams.bFullScreen);
    if (pSDL_Event_Loop_Data->pOrbiter == NULL)
    {
        _WX_LOG_ERR("error returned by : CreateOrbiter()");
        return false;
    }
    // EventProcess
    if (pSDL_Event_Loop_Data->pOrbiter)
    {
        while (SDL_Event_Process(*pSDL_Event_Loop_Data))
            ;
    }
    // Destroy
    if (pSDL_Event_Loop_Data->pOrbiter && pSDL_Event_Loop_Data->pOrbiter->m_bReload)
        g_nExitCode = 2;
    SDL_Event_Loop_End(*pSDL_Event_Loop_Data);
    if (g_pPlutoLogger)
        delete g_pPlutoLogger;
#endif // USE_RELEASE_CODE
    return g_nExitCode;
}
