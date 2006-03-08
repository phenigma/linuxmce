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

extern int g_nExitCode;
#ifdef USE_MAIN_CONSOLE
int main(int argc, char *argv[]);
#endif // USE_MAIN_CONSOLE

#endif
// _WX_MAIN_H_
