#ifndef _WX_DEFINES_H_
#define _WX_DEFINES_H_

// only for wx compatibility

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
// _WX_DEFINES_H_
