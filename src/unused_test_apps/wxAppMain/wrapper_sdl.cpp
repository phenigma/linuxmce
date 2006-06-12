//
// Author : Remus C.
//
// Changed by : ...
//

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "wrapper_sdl.h"
#endif

#include "wx/wxprec.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wrapper_sdl.h"

bool SDLwrapper::SetCursor_Image(const std::string &sPath)
{
    // SDL_Cursor *SDL_CreateCursor(Uint8 *data, Uint8 *mask, int w, int h, int hot_x, int hot_y);
    //
    // If you want to have color cursor, then this function is not for
    // you; instead, you must hide normal system cursor
    // (SDL_ShowCursor) and in your main loop, when you draw graphics,
    // also draw a SDL_Surface at the location of the mouse cursor.
    //
    // void SDL_SetCursor(SDL_Cursor *cursor);
    //
    // Sets the currently active cursor to the specified one. If the
    // cursor is currently visible, the change will be immediately
    // represented on the display. SDL_SetCursor(NULL) can be used to
    // force cursor redraw, if this is desired for any reason.
    //
    // int SDL_ShowCursor(int toggle);
    //
    // Toggle whether or not the cursor is shown on the
    // screen. Passing SDL_ENABLE displays the cursor and passing
    // SDL_DISABLE hides it. The current state of the mouse cursor can
    // be queried by passing SDL_QUERY, either SDL_DISABLE or
    // SDL_ENABLE will be returned.
    if (sPath == "")
        return false;
    return true;
}

bool SDLwrapper::SetCursor_Font(int nShape)
{
//#ifdef HAVE_XCURSOR
//#include <X11/Xlib.h>
//#include <X11/Xcursor/Xcursor.h>
//#include <SDL/SDL_syswm.h>
//#endif
//    // Load a cursor theme of your choice
//#ifdef HAVE_XCURSOR
//    Cursor cursor;
//    SDL_SysWMinfo info;
//    int ret;
//    //...
//    info.version.major = 1;
//    ret = SDL_GetWMInfo(&info);
//    if(ret == 1)
//    {
//        info.info.x11.lock_func();
//        cursor = XcursorLibraryLoadCursor(info.info.x11.display, "redglass");
//        XDefineCursor(info.info.x11.display, info.info.x11.window, cursor);
//        info.info.x11.unlock_func();
//    }
//#endif
    if (nShape < 0)
        return false;
    return true;
}
