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

void _debug_init();
void * _debug_RoomWizard();
void _debug_refresh_update();

void _debug_thread_block();
void _debug_thread_nonblock();

#endif
// _WX_DEBUGGING_H_
