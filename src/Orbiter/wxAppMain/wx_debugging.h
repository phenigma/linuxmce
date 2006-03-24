//
// Author : C Remus
//
// Changed by : ...
//

#ifndef _WX_DEBUGGING_H_
#define _WX_DEBUGGING_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wx_debugging.h"
#endif

void _debug_thread_block();
void _debug_thread_nonblock();
void _debug_refresh_update();

#endif
// _WX_DEBUGGING_H_
