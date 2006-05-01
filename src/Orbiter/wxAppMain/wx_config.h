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
    ID_EVTC_DIALOG,
    ID_EVTC_THREAD,
    ID_Timer_WakeIdle,
    ID_Timer_ExpireDialog,
};

#define LOG_FILE "/tmp/wxlog"

#define WAIT_THREAD_TIMEOUT_MSEC 2500
#define WAIT_ALL_THREADS_TIMEOUT_MSEC 5000
#define WAIT_THREAD_EXIT_MSEC 500

#define WAIT_SEMAPHORE_MSEC 100

//del : read all tasks until the queue is empty
#define INTERVAL_TIMER_WAKEIDLE_MSEC 50
#define INTERVAL_TIMER_EXPIREDIALOG_MSEC 200

#endif
// _WX_CONFIG_H_
