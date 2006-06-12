//
// Author : Remus C.
//
// Changed by : ...
//

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "wrapper_wx.h"
#endif

#include "wx/wxprec.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wrapper_wx.h"

bool WXwrapper::SetCursor_Image(const std::string &sPath)
{
    wxImage oImage(sPath);
    _COND_RET(oImage.Ok(), false);
    if (! oImage.HasAlpha())
        _LOG_WRN("Not HasAlpha");
    if (! oImage.ConvertAlphaToMask())
        _LOG_WRN("Not ConvertAlphaToMask");
    if (! oImage.HasAlpha())
        _LOG_WRN("Again, Not HasAlpha");
    //wxBitmap oBitmap(oImage);
    //_COND_RET(oBitmap.Ok(), false);
    wxCursor oCursor(oImage);
    _COND_RET(oCursor.Ok(), false);
    ::wxSetCursor(oCursor);
    return true;
}

bool WXwrapper::SetCursor_Font(int nShape)
{
    wxCursor oCursor(nShape);
    _COND_RET(oCursor.Ok(), false);
    ::wxSetCursor(oCursor);
    return true;
}
