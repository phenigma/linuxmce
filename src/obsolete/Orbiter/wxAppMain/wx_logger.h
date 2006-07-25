//
// Author : Remus C.
//
// Changed by : ...
//

#ifndef _WX_LOGGER_H_
#define _WX_LOGGER_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wx_logger.h"
#endif

#define _wx_log_nfo _wx_log_nfo_
#define _wx_log_err _wx_log_err_
#define _wx_log_wrn _wx_log_wrn_
#define _wx_log_dbg _wx_log_dbg_
#define _wx_log_err_abort _wx_log_err_abort_
#define _wx_log_verbose _wx_log_verbose_

void _wx_log_nfo_(const char *, ...);
void _wx_log_err_(const char *, ...);
void _wx_log_wrn_(const char *, ...);
void _wx_log_err_abort_(const char *, ...);
void _wx_log_verbose_(const char *, ...);
void _wx_log_dbg_(const char *, ...);

void global_wx_log_enter();
void global_wx_log_leave();

#endif
// _WX_LOGGER_H_
