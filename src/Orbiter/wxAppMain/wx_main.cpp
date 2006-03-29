//
// Author : C Remus
//
// Changed by : ...
//

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "wx_main.h"
#endif

#include "wx/wxprec.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx_main.h"
#include "wx_extern_app.h"
#if defined(__WXGTK__) || defined(__WXMOTIF__) || defined(__WXX11__)
#include <X11/Xlib.h>
#endif

int g_nExitCode = EXIT_SUCCESS;

#ifdef USE_MAIN_CONSOLE
int main(int argc, char *argv[])
{
    _WX_LOG_NFO("STARTED");
    // activate X11 GUI threads
    {
        if ( XInitThreads() == 0 )
        {
            _WX_LOG_WRN("Unable to initialize multithreaded X11 code (XInitThreads failed)");
        }
    }
    int nExitCode = EXIT_SUCCESS;
    // wx initialize
    {
        _WX_LOG_NFO("wxEntry() begin");
        nExitCode = wxEntry(argc, argv);
        _WX_LOG_NFO("wxTheApp %s NULL", ( (wxTheApp == NULL) ? "==" : "!=" ));
        _WX_LOG_NFO("wxEntry() end");
    }
    // be sure cleanup is ok
    {
        if (wxTheApp != NULL)
        {
            _WX_LOG_NFO("calling wxApp::OnExit()");
            wxTheApp->OnExit();
        }
        if (wxTheApp != NULL)
        {
            _WX_LOG_NFO("calling wxApp::CleanUp()");
#if wxCHECK_VERSION(2, 6, 0)
            wxTheApp->CleanUp();
#else // replacement code for old version
            wxApp::CleanUp();
#endif // wxCHECK_VERSION(2, 6, 0)
        }
        _WX_LOG_NFO("after wxApp::CleanUp(), wxTheApp %s NULL", ( (wxTheApp == NULL) ? "==" : "!=" ));
    }
    _WX_LOG_NFO("EXIT_SUCCESS ! CODE=%d", nExitCode);
    return nExitCode;
}
#endif // USE_MAIN_CONSOLE
