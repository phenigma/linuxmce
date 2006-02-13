#ifndef _WX_OTHER_H_
#define _WX_OTHER_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wx_other.h"
#endif

// defines >>

// uncomment to use the local defined main()
// otherwise use main() from the wx libs
#define USE_MAIN_CONSOLE

// uncomment to use the debug code
//#define USE_DEBUG_CODE

#define LOG_FILE "/tmp/wxlog"

#define WAIT_THREAD_TIMEOUT_MILLISECONDS 5000

// defines <<

#include "wx_defines.h"

#define WARN_IF(x) do { if (x) wxFAIL_MSG("Warning: " #x "\n"); } while (0)

// if not, return
#define wxELSE_RET(x)   do { if (!(x)) { wxFAIL; return; } } while (0)

// if not, return 0
#define wxELSE_RET_0(x)   do { if (!(x)) { wxFAIL; return 0; } } while (0)

// if not, return NULL
#define wxELSE_RET_NULL(x)   do { if (!(x)) { wxFAIL; return NULL; } } while (0)

// if not, return false
#define wxELSE_RET_false(x)  do { if (!(x)) { wxFAIL; return false; } } while (0)

// case CONST : return "CONST"
#define CASE_const_ret_str(x) case x: return #x

#define _wx_log_nfo wx_log_nfo
#define _wx_log_err wx_log_err
#define _wx_log_wrn wx_log_wrn
#define _wx_log_err_abort wx_log_err_abort
#define _wx_log_verbose wx_log_verbose

void wx_log_nfo(const char *, ...);
void wx_log_err(const char *, ...);
void wx_log_wrn(const char *, ...);
void wx_log_err_abort(const char *, ...);
void wx_log_verbose(const char *, ...);

inline wxString Str(int val)
{
  return wxString::Format("%d", val);
}

// can be used in threads
void wx_Sleep(unsigned long int n_seconds, unsigned long int n_add_milliseconds=0);

void global_wx_log_begin();
void global_wx_log_end();

extern int g_nExitCode;

#ifdef USE_MAIN_CONSOLE
int main(int argc, char *argv[]);
#endif // USE_MAIN_CONSOLE

#if (! defined USE_DEBUG_CODE)
#  include "../Main.h"
#  include "../SDL/StartOrbiterSDL.h"
#endif // (! defined USE_DEBUG_CODE)

void debug_thread_sample_block();
void debug_thread_sample_nonblock();

#endif
// _WX_OTHER_H_
