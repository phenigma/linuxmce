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
#include "wx_thread_bag.h"

#ifdef USE_RELEASE_CODE
bool g_USE_EXTERN_APP_ON_THREAD = true;
#else
bool g_USE_EXTERN_APP_ON_THREAD = false;
#endif // USE_RELEASE_CODE

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
    EventLoop();
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

bool ExternApp::EventLoop()
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
