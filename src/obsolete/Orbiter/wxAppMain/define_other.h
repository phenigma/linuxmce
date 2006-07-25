//
// Author : Remus C.
//
// Changed by : ...
//
// Description : other macros
//
// Notes :
//     currently tested only for gcc 3.4
//

#ifndef _DEFINE_OTHER_H_
#define _DEFINE_OTHER_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "define_other.h"
#endif

// log an error and seg-fault
#define _SEG_FAULT(...) \
    do \
    { \
        _LOG_ERR("seg-fault here : " __VA_ARGS__); \
        int *pSegFault = NULL; \
        *pSegFault = 0; \
    } while (0)

// case CONST : return "CONST"
#define CASE_const_ret_str(x) case x: return #x

#endif
// _DEFINE_OTHER_H_
