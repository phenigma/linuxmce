//
// Author : C Remus
//
// Changed by : ...
//

#ifndef _WX_CONFIG_H_
#define _WX_CONFIG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wx_config.h"
#endif

// configuration for whole project

// define USE_DEBUG_CODE to use the debug code
// else USE_RELEASE_CODE will be defined
//#define USE_DEBUG_CODE

// uncomment to use the local defined main()
// otherwise use main() from the wx libs
#define USE_MAIN_CONSOLE

// configuration ends here

#ifndef USE_DEBUG_CODE
#  define USE_RELEASE_CODE
#endif // USE_DEBUG_CODE

enum E_UNIQUE_ID
{
    ID_UNIQUE_Unused = wxID_HIGHEST,
    ID_Timer_Debug,
    ID_Timer_ExternApp,
    ID_Timer_ExpireDialog,
};

#define LOG_FILE "/tmp/wxlog"

#define WAIT_THREAD_TIMEOUT_MSEC 5000

#define INTERVAL_TIMER_DEBUG_MSEC 2000
#define INTERVAL_EXTERNAPP_LOOP_MSEC 2
#define WAIT_SEMAPHORE_MSEC 100

#define INTERVAL_TIMER_EXPIREDIALOG_MSEC 100

#endif
// _WX_CONFIG_H_
