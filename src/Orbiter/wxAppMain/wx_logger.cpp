//
// Author : C Remus
//
// Changed by : ...
//

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "wx_logger.h"
#endif

#include "wx/wxprec.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx_logger.h"
#include "wx_helpers.h"

//==================================================

const char sLogPathName[] = LOG_FILE;
FILE * g_fpLogFile = NULL;

typedef void (& type_fn_wxVLog)(const char *sformat, va_list arg_ptr);

//==================================================

enum E_LOG_TYPE
{
    STD = 0,
    NFO,
    ERR,
    WRN,
    DBG,
};

// not usable directly
// log to : stderr, file
static void _log_stdarg(const char *sformat, va_list arg_ptr, E_LOG_TYPE logtype)
{
    char * s_beg;
    char * s_end;
    switch(logtype)
    {
        case NFO:
            s_beg = "\033[0;34mINFO:\033[0m\033[1;36m";
            s_end = "\n\033[0m";
            break;
        case ERR:
            s_beg = "\033[0;31mERROR:\033[0m\033[1;31m";
            s_end = "\n\033[0m";
            break;
        case WRN:
            s_beg = "\033[0;35mWARNING:\033[0m\033[1;35m";
            s_end = "\n\033[0m";
            break;
        case DBG:
            s_beg = "\033[0;33mDBG:\033[0m\033[1;33m";
            s_end = "\n\033[0m";
            break;
        default:
            s_beg = "";
            s_end = "\n";
    }
    fprintf(stderr, s_beg);
    vfprintf(stderr, sformat, arg_ptr);
    fprintf(stderr, s_end);
    if (g_fpLogFile)
    {
        fprintf(g_fpLogFile, s_beg);
        vfprintf(g_fpLogFile, sformat, arg_ptr);
        fprintf(g_fpLogFile, s_end);
    }
}

// not usable directly
// log to : wxVLog* in a thread-safe way
static void _log_wxVLog(const char *sformat, va_list arg_ptr, type_fn_wxVLog fn_wxVLog)
{
    if (! ::wxIsMainThread())
    {
        return;
    }
    wxCondLocker lock(! ::wxIsMainThread(), wxMutexGuiEnter, wxMutexGuiLeave);
    fn_wxVLog(sformat, arg_ptr);
}

// not usable directly
// log to : stderr, file, wxVLog*
static void _log_stdarg_wx(const char *sformat, va_list arg_ptr, type_fn_wxVLog fn_wxVLog, E_LOG_TYPE logtype)
{
    _log_stdarg(sformat, arg_ptr, logtype);
    _log_wxVLog(NoEsc(sformat), arg_ptr, fn_wxVLog);
}

//==================================================

void _wx_log_nfo_(const char *sformat, ...)
{
    va_list arg_ptr;
    va_start(arg_ptr, sformat);
    _log_stdarg_wx(sformat, arg_ptr, wxVLogMessage, NFO);
    va_end(arg_ptr);
}

void _wx_log_err_(const char *sformat, ...)
{
    va_list arg_ptr;
    va_start(arg_ptr, sformat);
    _log_stdarg_wx(sformat, arg_ptr, wxVLogError, ERR);
    va_end(arg_ptr);
}

void _wx_log_wrn_(const char *sformat, ...)
{
    va_list arg_ptr;
    va_start(arg_ptr, sformat);
    _log_stdarg_wx(sformat, arg_ptr, wxVLogWarning, WRN);
    va_end(arg_ptr);
}

void _wx_log_err_abort_(const char *sformat, ...)
{
    va_list arg_ptr;
    va_start(arg_ptr, sformat);
    _log_stdarg_wx(sformat, arg_ptr, wxVLogFatalError, ERR);
    va_end(arg_ptr);
}

void _wx_log_verbose_(const char *sformat, ...)
{
    va_list arg_ptr;
    va_start(arg_ptr, sformat);
    _log_stdarg_wx(sformat, arg_ptr, wxVLogVerbose, NFO);
    va_end(arg_ptr);
}

#ifdef USE_DEBUG_CODE
bool g_WX_LOG_DBG = true;
#else
bool g_WX_LOG_DBG = (getenv("WX_LOG_DBG"));
#endif // USE_DEBUG_CODE

void _wx_log_dbg_(const char *sformat, ...)
{
    if (! g_WX_LOG_DBG)
        return;
    va_list arg_ptr;
    va_start(arg_ptr, sformat);
    _log_stdarg_wx(sformat, arg_ptr, wxVLogMessage, DBG);
    va_end(arg_ptr);
}

//==================================================

struct logger_obj
{
    logger_obj()
        {
            //freopen( sLogPathName, "w", stderr );
            g_fpLogFile = ::fopen( sLogPathName, "w" );
            ::setlinebuf(g_fpLogFile);
            _wx_log_nfo("\n\n* >>>>>>>>>>\n");
        }
    ~logger_obj()
        {
            _wx_log_nfo("\n* <<<<<<<<<<\n");
            ::fclose(g_fpLogFile);
            g_fpLogFile = NULL;
        }
};
logger_obj _global_logger_obj;

//==================================================

void global_wx_log_enter()
{
#if ( wxUSE_DEBUG_CONTEXT || wxUSE_MEMORY_TRACING )
    wxDebugContext::SetCheckpoint();
#endif
    wxLog::DontCreateOnDemand();
    wxLog::SetTimestamp("%H:%M:%S");
    wxLog::SetActiveTarget(new wxLogStderr);
    //wxDebugContext::SetDebugMode(true);
}

void global_wx_log_leave()
{
    wxLog::FlushActive();
}

//==================================================
