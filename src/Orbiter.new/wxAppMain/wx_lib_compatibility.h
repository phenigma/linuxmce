//
// Author : C Remus
//
// Changed by : ...
//

// only for wx compatibility

#ifndef _WX_LIB_COMPATIBILITY_H_
#define _WX_LIB_COMPATIBILITY_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wx_lib_compatibility.h"
#endif

#if ! wxCHECK_VERSION(2, 6, 0)
#  ifdef __cplusplus
#   ifdef __BORLANDC__
#     define wxUnusedVar(identifier) identifier
#   else
template <class T> inline void wxUnusedVar(const T& WXUNUSED(t)) { }
#   endif
#  endif
#endif
// ! wxCHECK_VERSION(2, 6, 0)

//#ifndef WXUNUSED
//#  if 1 /*  there should be no more any compilers needing the "#else" version */
//#    define WXUNUSED(identifier) /* identifier */
//#  else  /*  stupid, broken compiler */
//#    define WXUNUSED(identifier) identifier
//#  endif
//#endif

#endif
// _WX_LIB_COMPATIBILITY_H_
