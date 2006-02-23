#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "wxappmain.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
////@end includes

#include "wxappmain.h"
#include "wx_other.h"
#include "wxthreadbag.h"

////@begin XPM images
////@end XPM images

/*!
 * Application instance implementation
 */

#ifdef USE_MAIN_CONSOLE

IMPLEMENT_APP_NO_MAIN( wxAppMain );
IMPLEMENT_WX_THEME_SUPPORT;

#else

////@begin implement app
IMPLEMENT_APP( wxAppMain )
////@end implement app
  ;

#endif // USE_MAIN_CONSOLE

/*!
 * wxAppMain type definition
 */

IMPLEMENT_CLASS( wxAppMain, wxApp )
  ;

/*!
 * wxAppMain event table definition
 */

BEGIN_EVENT_TABLE( wxAppMain, wxApp )

////@begin wxAppMain event table entries
////@end wxAppMain event table entries

  END_EVENT_TABLE()
  ;

/*!
 * Constructor for wxAppMain
 */

wxAppMain::wxAppMain()
{
  global_wx_log_begin();
  _wx_log_nfo("wxAppMain::wxAppMain()");
////@begin wxAppMain member initialisation
////@end wxAppMain member initialisation
}

/*!
 * Initialisation for wxAppMain
 */

bool wxAppMain::OnInit()
{
  _wx_log_nfo("wxAppMain::OnInit()");
  wxELSE_RET_false(this == wxTheApp);
////@begin wxAppMain initialisation
    // Remove the comment markers above and below this block
    // to make permanent changes to the code.

#if wxUSE_XPM
    wxImage::AddHandler(new wxXPMHandler);
#endif
#if wxUSE_LIBPNG
    wxImage::AddHandler(new wxPNGHandler);
#endif
#if wxUSE_LIBJPEG
    wxImage::AddHandler(new wxJPEGHandler);
#endif
#if wxUSE_GIF
    wxImage::AddHandler(new wxGIFHandler);
#endif
    wxFrameMain* mainWindow = new wxFrameMain;
    mainWindow->Create( NULL, ID_FRAMEMAIN );
    mainWindow->Show(true);
////@end wxAppMain initialisation

  SetTopWindow(mainWindow);
  _wx_log_nfo("wxAppMain::OnInit();;");
  return true;
}

/*!
 * Cleanup for wxAppMain
 */
int wxAppMain::OnExit()
{
  // NO _wx_log_nfo() if NO DontCreateOnDemand()
  _wx_log_nfo("wxAppMain::OnExit()");
////@begin wxAppMain cleanup
    return wxApp::OnExit();
////@end wxAppMain cleanup
}

wxAppMain::~wxAppMain()
{
  _wx_log_nfo("wxAppMain::~wxAppMain()");
  global_wx_log_end();
  this->OnExit();
}

int wxAppMain::OnRun()
{
  _wx_log_nfo("wxAppMain::OnRun()");
  wxApp::OnRun();
  _wx_log_nfo("wxAppMain::OnRun() : exit code : %d", g_nExitCode);
  return g_nExitCode;
}
