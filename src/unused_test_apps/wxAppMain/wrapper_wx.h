//
// Author : Remus C.
//
// Changed by : ...
//

#ifndef _WRAPPER_WX_H_
#define _WRAPPER_WX_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wrapper_wx.h"
#endif

// WX wrapper class
// not complete, not need to be finished now
class WXwrapper
{
public:
    static bool SetCursor_Image(const std::string &sPath);
    static bool SetCursor_Font(int nShape);
};

#endif
// _WRAPPER_WX_H_
