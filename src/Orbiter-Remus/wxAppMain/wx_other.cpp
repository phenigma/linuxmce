#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "wx_other.h"
#endif
#include <wx/wxprec.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "wx_other.h"
#include "wxthreaditem.h"
#include "wxthreadbag.h"
#include "wxthreadevent.h"

// run_message: Xlib: unexpected async reply
// enable : XInitThreads(), XLockDisplay(), XUnlockDisplay() ; Nested locking works correctly
#if defined(__WXGTK__) || defined(__WXMOTIF__) || defined(__WXX11__)
#include <X11/Xlib.h>
#endif
// g_thread_init()
// surround any calls to GTK+ not made within a signal handler with a
// gdk_threads_enter()/gdk_threads_leave()
//
// new X connection to the server => separate event queue

//==================================================

char sLogPathName[] = LOG_FILE;
FILE * g_fpLogFile = NULL;

typedef void (& type_fn_wxVLog)(const char *sformat, va_list arg_ptr);

//==================================================

// not usable directly
// log to : stderr, file
static void _log_stdarg(const char *sformat, va_list arg_ptr)
{
  vfprintf(stderr, sformat, arg_ptr);
  fprintf(stderr, "\n");
  if (g_fpLogFile)
  {
    vfprintf(g_fpLogFile, sformat, arg_ptr);
    fprintf(g_fpLogFile, "\n");
  }
}

// not usable directly
// log to : wxVLog* in a thread-safe way
static void _log_wxVLog(const char *sformat, va_list arg_ptr, type_fn_wxVLog p_wxVLog)
{
  if (! ::wxIsMainThread())
    wxMutexGuiEnter();
  p_wxVLog(sformat, arg_ptr);
  if (! ::wxIsMainThread())
    wxMutexGuiLeave();
}

// not usable directly
// log to : stderr, file, wxVLog*
static void _log_stdarg_wx(const char *sformat, va_list arg_ptr, type_fn_wxVLog p_wxVLog)
{
  _log_stdarg(sformat, arg_ptr);
  _log_wxVLog(sformat, arg_ptr, p_wxVLog);
}

//==================================================

void wx_log_nfo(const char *sformat, ...)
{
  va_list arg_ptr;
  va_start(arg_ptr, sformat);
  _log_stdarg_wx(sformat, arg_ptr, wxVLogMessage);
  va_end(arg_ptr);
}

void wx_log_err(const char *sformat, ...)
{
  va_list arg_ptr;
  va_start(arg_ptr, sformat);
  _log_stdarg_wx(sformat, arg_ptr, wxVLogError);
  va_end(arg_ptr);
}

void wx_log_wrn(const char *sformat, ...)
{
  va_list arg_ptr;
  va_start(arg_ptr, sformat);
  _log_stdarg_wx(sformat, arg_ptr, wxVLogWarning);
  va_end(arg_ptr);
}

void wx_log_err_abort(const char *sformat, ...)
{
  va_list arg_ptr;
  va_start(arg_ptr, sformat);
  _log_stdarg_wx(sformat, arg_ptr, wxVLogFatalError);
  va_end(arg_ptr);
}

void wx_log_verbose(const char *sformat, ...)
{
  va_list arg_ptr;
  va_start(arg_ptr, sformat);
  _log_stdarg_wx(sformat, arg_ptr, wxVLogVerbose);
  va_end(arg_ptr);
}

//==================================================

void wx_Sleep(unsigned long int n_seconds, unsigned long int n_milliseconds/*=0*/)
{
  if (::wxIsMainThread())
    ::wxMilliSleep(n_seconds * 1000 + n_milliseconds);
  else
    wxThread::Sleep(n_seconds * 1000 + n_milliseconds);
}

//==================================================

void global_wx_log_begin()
{
#if ( wxUSE_DEBUG_CONTEXT || wxUSE_MEMORY_TRACING )
  wxDebugContext::SetCheckpoint();
#endif
  wxLog::DontCreateOnDemand();
  wxLog::SetTimestamp("%H:%M:%S");
  wxLog::SetActiveTarget(new wxLogStderr);
  //wxDebugContext::SetDebugMode(true);
}

void global_wx_log_end()
{
  wxLog::FlushActive();
}

//==================================================

void global_begin()
{
  //freopen( sLogPathName, "w", stderr );
  g_fpLogFile = ::fopen( sLogPathName, "w" );
  ::setlinebuf(g_fpLogFile);
  _wx_log_nfo("\n\n* >>>>>>>>>>\n");
}

void global_end()
{
  _wx_log_nfo("\n* <<<<<<<<<<\n");
  ::fclose(g_fpLogFile);
  g_fpLogFile = NULL;
}

struct init_before_main
{
  init_before_main()
    {
      global_begin();
    }
  ~init_before_main()
    {
      global_end();
    }
};
init_before_main init_before_main_obj;

//==================================================

int g_nExitCode = EXIT_SUCCESS;

#ifdef USE_MAIN_CONSOLE
int main(int argc, char *argv[])
{
  _wx_log_nfo("* extern main() : STARTED");
  int nExitCode = EXIT_SUCCESS;
  // activate GUI threads
  {
    if ( XInitThreads() == 0 )
    {
      _wx_log_err("Unable to initialize multithreaded X11 code (XInitThreads failed)");
      exit( EXIT_FAILURE );
    }
  }
  // wx initialize
  {
    _wx_log_nfo("* extern main() : wxEntry() begin");
    nExitCode = wxEntry(argc, argv);
    _wx_log_nfo("* extern main() : wxTheApp %s NULL", ( (wxTheApp == NULL) ? "==" : "!=" ));
    _wx_log_nfo("* extern main() : wxEntry() end");
  }
  // be sure cleanup is ok
  {
    if (wxTheApp != NULL)
    {
      _wx_log_nfo("* extern main() : calling wxTheApp->OnExit()");
      wxTheApp->OnExit();
    }
    if (wxTheApp != NULL)
    {
      _wx_log_nfo("* extern main() : calling wxTheApp->CleanUp()");
#if wxCHECK_VERSION(2, 6, 0)
      wxTheApp->CleanUp();
#else // replacement code for old version
      wxApp::CleanUp();
#endif
    }
    _wx_log_nfo("* extern main() : wxApp::CleanUp() : wxTheApp %s NULL", ( (wxTheApp == NULL) ? "==" : "!=" ));
  }
  _wx_log_nfo("* extern main() : EXIT_SUCCESS ! CODE=%d", nExitCode);
  return nExitCode;
}
#endif // USE_MAIN_CONSOLE

//==================================================

void debug_thread_sample_block()
{
  _wx_log_nfo("debug_thread_sample_block()");
  for ( int i = 0; i < 5; i++ )
  {
    _wx_log_nfo("debug_thread_sample_block() [%d]", i);
    wxPostThreadEvent(g_pwxThreadBag, Evt_RunIdle, "debug_thread_sample_block()");
    wx_Sleep(1);
  }
  _wx_log_nfo("debug_thread_sample_block();;");
}

void debug_thread_sample_nonblock()
{
  _wx_log_nfo("debug_thread_sample_nonblock()");
  wxThreadItem *pThis = ptrThreadItem();
  for ( int i = 0; i < 10; i++ )
  {
    _wx_log_nfo("debug_thread_sample_nonblock(%p) [%d]", pThis, i);
    if ( wxIdleThreadShouldStop() )
    {
      _wx_log_nfo("debug_thread_sample_nonblock(): Should Stop Now");
      pThis->Exit(0);
      break;
    }
    wxPostThreadEvent(g_pwxThreadBag, Evt_RunIdle, "debug_thread_sample_nonblock()");
    wx_Sleep(0, 500);
  }
  _wx_log_nfo("debug_thread_sample_nonblock();;");
}

//==================================================
