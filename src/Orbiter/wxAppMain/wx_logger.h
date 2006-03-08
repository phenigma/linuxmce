//
// Author : C Remus
//
// Changed by : ...
//

#ifndef _WX_LOGGER_H_
#define _WX_LOGGER_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wx_logger.h"
#endif

#if 0
#define _WX_FAIL_MSG(x) wxFAIL_MSG(x)
#else
#define _WX_FAIL_MSG(x)
#endif

#define _WX_LOG_NFO(x, ...) _wx_log_nfo(x " \033[0m\033[32m// %s\033[0m" , ## __VA_ARGS__ , __PRETTY_FUNCTION__)
#define _WX_LOG_ERR(x, ...) _wx_log_err(x " \033[0m\033[32m// %s\033[0m" , ## __VA_ARGS__ , __PRETTY_FUNCTION__)
#define _WX_LOG_WRN(x, ...) _wx_log_wrn(x " \033[0m\033[32m// %s\033[0m" , ## __VA_ARGS__ , __PRETTY_FUNCTION__)
#define _WX_LOG_DBG(x, ...) _wx_log_dbg(x " \033[0m\033[32m// %s\033[0m" , ## __VA_ARGS__ , __PRETTY_FUNCTION__)

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
