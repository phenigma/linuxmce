//
// Author : C Remus
//
// Changed by : ...
//

#ifndef _WX_MAIN_H_
#define _WX_MAIN_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wx_main.h"
#endif

// is true if USE_WX_LIB environment variable is defined
extern bool g_USE_WX_LIB;

// holds the app exit code
extern int g_nExitCode;

#ifdef USE_MAIN_CONSOLE
int main(int argc, char *argv[]);
#endif // USE_MAIN_CONSOLE

#endif
// _WX_MAIN_H_
