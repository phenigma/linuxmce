#if !defined(__PLUTO_DEFS_H__)
#define __PLUTO_DEFS_H__

#define PLUTO_SAFE_DELETE(p) \
{ \
    if(NULL != (p)) { delete (p); (p) = NULL; } \
}

#define PLUTO_SAFE_DELETE_ARRAY(pArray) \
{ \
    if(NULL != (pArray)) { delete [] (pArray); (pArray) = NULL; } \
}

#define PLUTO_SAFE_DELETE_ARRAY_OF_ARRAYS(ppArray, nArrayLen) \
{ \
    for(int nArrayIndex = 0; nArrayIndex < nArrayLen; nArrayIndex++) \
        PLUTO_SAFE_DELETE_ARRAY(ppArray[nArrayIndex]); \
    PLUTO_SAFE_DELETE_ARRAY(ppArray); \
}

#endif
