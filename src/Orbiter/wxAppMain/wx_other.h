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

#include <string>
#include <map>
using namespace std;

WX_DEFINE_ARRAY(bool, wxArrayBool);

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

#define _wx_log_nfo _wx_log_nfo_
#define _wx_log_err _wx_log_err_
#define _wx_log_wrn _wx_log_wrn_
#define _wx_log_err_abort _wx_log_err_abort_
#define _wx_log_verbose _wx_log_verbose_

void _wx_log_nfo_(const char *, ...);
void _wx_log_err_(const char *, ...);
void _wx_log_wrn_(const char *, ...);
void _wx_log_err_abort_(const char *, ...);
void _wx_log_verbose_(const char *, ...);

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
