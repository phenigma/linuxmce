//
// Author : Remus C.
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

#ifdef USE_RELEASE_CODE
#  include "../SDL/StartOrbiterSDL.h"
#endif // USE_RELEASE_CODE

#if defined(__WXGTK__) || defined(__WXMOTIF__) || defined(__WXX11__)
#include <X11/Xlib.h>
#endif

#ifdef USE_MAIN_CONSOLE
int main(int argc, char *argv[])
{
    _LOG_NFO("STARTED");
    int nExitCode = EXIT_SUCCESS;
    //if (0) // already in Init_System
    //{
    //    // activate X11 GUI threads
    //    if ( XInitThreads() == 0 )
    //    {
    //        _LOG_WRN("Unable to initialize multithreaded X11 code (XInitThreads failed)");
    //    }
    //}
#ifdef USE_RELEASE_CODE
    if (! Init_System())
        return 1;
#endif // USE_RELEASE_CODE
    // wx initialize
    {
        _LOG_NFO("wxEntry() begin");
        nExitCode = wxEntry(argc, argv);
        _LOG_NFO("wxTheApp %s NULL", ( (wxTheApp == NULL) ? "==" : "!=" ));
        _LOG_NFO("wxEntry() end");
    }
    // be sure cleanup is ok
    {
        if (wxTheApp != NULL)
        {
            _LOG_NFO("calling wxApp::OnExit()");
            wxTheApp->OnExit();
        }
        if (wxTheApp != NULL)
        {
            _LOG_NFO("calling wxApp::CleanUp()");
#if wxCHECK_VERSION(2, 6, 0)
            wxTheApp->CleanUp();
#else // replacement code for old version
            wxApp::CleanUp();
#endif // wxCHECK_VERSION(2, 6, 0)
        }
        _LOG_NFO("after wxApp::CleanUp(), wxTheApp %s NULL", ( (wxTheApp == NULL) ? "==" : "!=" ));
    }
    _LOG_NFO("EXIT_SUCCESS ! CODE=%d", nExitCode);
    return nExitCode;
}
#endif // USE_MAIN_CONSOLE
