//
// Author : C Remus
//
// Changed by : ...
//

#ifndef _WX_MACRO_H_
#define _WX_MACRO_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wx_macro.h"
#endif

// check condition, log on error
#define _COND(x) \
  do \
  { \
    if (!(x)) \
    { \
      _WX_LOG_ERR("_COND(" #x ")"); \
      _WX_FAIL_MSG("_COND(" #x ")"); \
    } \
  } while (0)

// check condition, log and return on error
#define _COND_RET(x, ...) \
  do \
  { \
    if (!(x)) \
    { \
      _WX_LOG_ERR("_COND_RET(" #x ", " #__VA_ARGS__ ")"); \
      _WX_FAIL_MSG("_COND_RET(" #x ", " #__VA_ARGS__ ")"); \
      return __VA_ARGS__; \
    } \
  } while (0)

// check (var == value_old), log on error, assign (var = value_new) in any case
#define _COND_ASSIGN(var, value_old, value_new) \
  do \
  { \
    if (!(var == value_old)) \
    { \
      _WX_LOG_ERR("_COND_ASSIGN(" #var ", " #value_old ", " #value_new ")"); \
      _WX_FAIL_MSG("_COND_ASSIGN(" #var ", " #value_old ", " #value_new ")"); \
    } \
    var = value_new; \
  } while (0)

// case CONST : return "CONST"
#define CASE_const_ret_str(x) case x: return #x

#endif
// _WX_MACRO_H_
