#if !defined(__PLUTO_DEFS_H__)
#define __PLUTO_DEFS_H__

#define PLUTO_SAFE_DELETE(x) { if(NULL != (x)) { delete (x); (x) = NULL; } }

#endif 