#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "wxdialog_pointer.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
////@end includes

#include "wxdialog_pointer.h"
#include <wx/filename.h>

////@begin XPM images
#include "cancel.xpm"
#include "ok.xpm"
#include "exit.xpm"
////@end XPM images

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>

bool X11_SetCursor(const std::string &sPath);
bool X11_ConstrainMouse(int nPosX, int nPosY, int nWidth, int nHeight, std::string *pStringError=NULL);

bool wx_SetCursor(const char *sPath);

struct struct_string_int
{
    std::string sName;
    int nValue;
};
extern struct_string_int g_aCursorNameValue[];
extern const int g_nCount_CursorNameValue;

/*!
 * wxDialog_Pointer type definition
 */

IMPLEMENT_DYNAMIC_CLASS( wxDialog_Pointer, wxDialog_Base );

/*!
 * wxDialog_Pointer event table definition
 */

BEGIN_EVENT_TABLE( wxDialog_Pointer, wxDialog_Base )

////@begin wxDialog_Pointer event table entries
    EVT_BUTTON( ID_BUTTON_CONSTRAINMOUSE, wxDialog_Pointer::OnButtonConstrainmouseClick )

    EVT_BUTTON( ID_BUTTON_LOADIMAGE, wxDialog_Pointer::OnButtonLoadimageClick )

    EVT_BUTTON( wxID_CLOSE, wxDialog_Pointer::OnCloseClick )

////@end wxDialog_Pointer event table entries

    END_EVENT_TABLE();

/*!
 * wxDialog_Pointer constructors
 */

wxDialog_Pointer::wxDialog_Pointer( )
{
}

wxDialog_Pointer::wxDialog_Pointer( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Create(parent, id, caption, pos, size, style);
}

/*!
 * wxDialog_Pointer creator
 */

bool wxDialog_Pointer::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin wxDialog_Pointer member initialisation
    v_pBoxV_all = NULL;
    v_pBoxH_Buttons = NULL;
    v_pButton_ConstrainMouse = NULL;
    v_pButton_LoadImage = NULL;
    v_pButton_Close = NULL;
    v_pChoice = NULL;
    v_pStaticText = NULL;
////@end wxDialog_Pointer member initialisation

////@begin wxDialog_Pointer creation
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog_Base::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    Centre();
////@end wxDialog_Pointer creation
    return true;
}

/*!
 * Control creation for wxDialog_Pointer
 */

void wxDialog_Pointer::CreateControls()
{
    _WX_LOG_NFO();
////@begin wxDialog_Pointer content construction
    wxDialog_Pointer* itemDialog_Base1 = this;

    v_pBoxV_all = new wxBoxSizer(wxVERTICAL);
    itemDialog_Base1->SetSizer(v_pBoxV_all);

    v_pBoxH_Buttons = new wxBoxSizer(wxHORIZONTAL);
    v_pBoxV_all->Add(v_pBoxH_Buttons, 0, wxGROW|wxALL, 5);

    v_pButton_ConstrainMouse = new wxButton;
    v_pButton_ConstrainMouse->Create( itemDialog_Base1, ID_BUTTON_CONSTRAINMOUSE, _T("Constrain Mouse"), wxDefaultPosition, wxDefaultSize, 0 );
    v_pBoxH_Buttons->Add(v_pButton_ConstrainMouse, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    v_pButton_LoadImage = new wxButton;
    v_pButton_LoadImage->Create( itemDialog_Base1, ID_BUTTON_LOADIMAGE, _T("Load Pointer Image"), wxDefaultPosition, wxDefaultSize, 0 );
    v_pButton_LoadImage->SetDefault();
    v_pBoxH_Buttons->Add(v_pButton_LoadImage, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBitmap v_pButton_CloseBitmap(itemDialog_Base1->GetBitmapResource(wxT("cancel.png")));
    v_pButton_Close = new wxBitmapButton;
    v_pButton_Close->Create( itemDialog_Base1, wxID_CLOSE, v_pButton_CloseBitmap, wxDefaultPosition, wxDefaultSize, 0 );
    wxBitmap v_pButton_CloseBitmapSel(itemDialog_Base1->GetBitmapResource(wxT("ok.png")));
    v_pButton_Close->SetBitmapSelected(v_pButton_CloseBitmapSel);
    wxBitmap v_pButton_CloseBitmapFocus(itemDialog_Base1->GetBitmapResource(wxT("exit.png")));
    v_pButton_Close->SetBitmapFocus(v_pButton_CloseBitmapFocus);
    v_pBoxH_Buttons->Add(v_pButton_Close, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxString v_pChoiceStrings[] = {
        _T("aa"),
        _T("bb"),
        _T("cc"),
        _T("dd")
    };
    v_pChoice = new wxChoice;
    v_pChoice->Create( itemDialog_Base1, ID_CHOICE_STD, wxDefaultPosition, wxDefaultSize, 4, v_pChoiceStrings, 0 );
    v_pChoice->SetStringSelection(_T("bb"));
    v_pBoxV_all->Add(v_pChoice, 0, wxGROW|wxALL, 5);

    v_pStaticText = new wxStaticText;
    v_pStaticText->Create( itemDialog_Base1, wxID_STATIC, _T("Static text Static text Static text Static text Static text Static text Static text Static text "), wxDefaultPosition, wxDefaultSize, 0 );
    v_pBoxV_all->Add(v_pStaticText, 0, wxGROW|wxALL|wxADJUST_MINSIZE, 5);

////@end wxDialog_Pointer content construction
    //--CaptureMouse();
    v_pStaticText->SetLabel("");
    ::wxInitAllImageHandlers();
}

/*!
 * Should we show tooltips?
 */

bool wxDialog_Pointer::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap wxDialog_Pointer::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin wxDialog_Pointer bitmap retrieval
    wxUnusedVar(name);
    if (name == _T("cancel.png"))
    {
        wxBitmap bitmap(cancel_xpm);
        return bitmap;
    }
    else if (name == _T("ok.png"))
    {
        wxBitmap bitmap(ok_xpm);
        return bitmap;
    }
    else if (name == _T("exit.png"))
    {
        wxBitmap bitmap(exit_xpm);
        return bitmap;
    }
    return wxNullBitmap;
////@end wxDialog_Pointer bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon wxDialog_Pointer::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin wxDialog_Pointer icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end wxDialog_Pointer icon retrieval
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_CONSTRAINMOUSE
 */

void wxDialog_Pointer::OnButtonConstrainmouseClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_CONSTRAINMOUSE in wxDialog_Pointer.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_CONSTRAINMOUSE in wxDialog_Pointer.

    static bool bIsActiveConstrainMouse = false;
    std::string sErrorMessage;
    bool bResult = false;
    if (! bIsActiveConstrainMouse)
    {
        bIsActiveConstrainMouse = bResult = X11_ConstrainMouse(100, 100, 200, 200, &sErrorMessage);
    }
    else
    {
        bResult = X11_ConstrainMouse(0, 0, 0, 0, &sErrorMessage);
        bIsActiveConstrainMouse = false;
    }
    _WX_LOG_NFO("bIsActiveConstrainMouse=%d, bResult=%d, sErrorMessage='%s'", bIsActiveConstrainMouse, bResult, sErrorMessage.c_str());
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LOADIMAGE
 */

void wxDialog_Pointer::OnButtonLoadimageClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LOADIMAGE in wxDialog_Pointer.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LOADIMAGE in wxDialog_Pointer.
    wxFileName oFileName("./data/pointers/");
    oFileName.Normalize();
    static wxString sMessage("Open Pointer Image");
    static wxString sDir(oFileName.GetFullPath());
    static wxString sFilename("standard_big_b.png");
    static wxString sExtension;
    static wxString sWildcard(
        wxString::Format(
            "All Files (%s)|%s|"
            "PNG files (*.png)|*.png|"
            "BMP files (*.bmp)|*.bmp|"
            "ICO files (*.ico)|*.ico|"
            "GIF files (*.gif)|*.gif|"
            "JPEG files (*.jpg)|*.jpg|"
            "PCX files (*.pcx)|*.pcx|"
            "TIFF files (*.tif)|*.tif|"
            "CURSOR files (*.cur)|*.cur"
            , wxFileSelectorDefaultWildcardStr, wxFileSelectorDefaultWildcardStr
            )
        );
    int nFlags = wxOPEN | wxFILE_MUST_EXIST;
    _WX_LOG_NFO("sDir='%s', sFilename='%s'", sDir.c_str(), sFilename.c_str());
    wxFileDialog oFileDialog(this, sMessage, sDir, sFilename, sWildcard, nFlags);
    if (wxID_CANCEL == oFileDialog.ShowModal())
        return;
    sDir = oFileDialog.GetDirectory();
    sFilename = oFileDialog.GetFilename();
    wxString sPath = oFileDialog.GetPath();
    _COND_RET(! sPath.IsEmpty());
    _WX_LOG_NFO("sPath='%s'", sPath.c_str());
    if (wx_SetCursor(sPath))
    {
        v_pStaticText->SetLabel(sPath);
    }
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CLOSE
 */

void wxDialog_Pointer::OnCloseClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CLOSE in wxDialog_Pointer.
    // Before editing this code, remove the block markers.
    Destroy();
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CLOSE in wxDialog_Pointer.
    wxUnusedVar(event);
}

wxDialog_Pointer::~wxDialog_Pointer()
{
    _WX_LOG_NFO();
    ::wxSetCursor(wxNullCursor);
    //--ReleaseMouse();
}

bool wx_SetCursor(const char *sPath)
{
    wxImage oImage(sPath);
    _COND_RET(oImage.Ok(), false);
    if (! oImage.HasAlpha())
        _WX_LOG_WRN("Not HasAlpha");
    if (! oImage.ConvertAlphaToMask())
        _WX_LOG_WRN("Not ConvertAlphaToMask");
    if (! oImage.HasAlpha())
        _WX_LOG_WRN("Again, Not HasAlpha");
    //wxBitmap oBitmap(oImage);
    //_COND_RET(oBitmap.Ok(), false);
    wxCursor oCursor(oImage);
    _COND_RET(oCursor.Ok(), false);
    ::wxSetCursor(oCursor);
    return true;
}

bool SDL_SetCursor(const std::string &sPath)
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

bool SDL_X11_SetCursor(const std::string &sPath)
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
    if (sPath == "")
        return false;
    return true;
}

// all from : /usr/include/X11/cursorfont.h
struct_string_int g_aCursorNameValue[] =
{
    {"XC_num_glyphs", XC_num_glyphs},
    {"XC_X_cursor", XC_X_cursor},
    {"XC_arrow", XC_arrow},
    {"XC_based_arrow_down", XC_based_arrow_down},
    {"XC_based_arrow_up", XC_based_arrow_up},
    {"XC_boat", XC_boat},
    {"XC_bogosity", XC_bogosity},
    {"XC_bottom_left_corner", XC_bottom_left_corner},
    {"XC_bottom_right_corner", XC_bottom_right_corner},
    {"XC_bottom_side", XC_bottom_side},
    {"XC_bottom_tee", XC_bottom_tee},
    {"XC_box_spiral", XC_box_spiral},
    {"XC_center_ptr", XC_center_ptr},
    {"XC_circle", XC_circle},
    {"XC_clock", XC_clock},
    {"XC_coffee_mug", XC_coffee_mug},
    {"XC_cross", XC_cross},
    {"XC_cross_reverse", XC_cross_reverse},
    {"XC_crosshair", XC_crosshair},
    {"XC_diamond_cross", XC_diamond_cross},
    {"XC_dot", XC_dot},
    {"XC_dotbox", XC_dotbox},
    {"XC_double_arrow", XC_double_arrow},
    {"XC_draft_large", XC_draft_large},
    {"XC_draft_small", XC_draft_small},
    {"XC_draped_box", XC_draped_box},
    {"XC_exchange", XC_exchange},
    {"XC_fleur", XC_fleur},
    {"XC_gobbler", XC_gobbler},
    {"XC_gumby", XC_gumby},
    {"XC_hand1", XC_hand1},
    {"XC_hand2", XC_hand2},
    {"XC_heart", XC_heart},
    {"XC_icon", XC_icon},
    {"XC_iron_cross", XC_iron_cross},
    {"XC_left_ptr", XC_left_ptr},
    {"XC_left_side", XC_left_side},
    {"XC_left_tee", XC_left_tee},
    {"XC_leftbutton", XC_leftbutton},
    {"XC_ll_angle", XC_ll_angle},
    {"XC_lr_angle", XC_lr_angle},
    {"XC_man", XC_man},
    {"XC_middlebutton", XC_middlebutton},
    {"XC_mouse", XC_mouse},
    {"XC_pencil", XC_pencil},
    {"XC_pirate", XC_pirate},
    {"XC_plus", XC_plus},
    {"XC_question_arrow", XC_question_arrow},
    {"XC_right_ptr", XC_right_ptr},
    {"XC_right_side", XC_right_side},
    {"XC_right_tee", XC_right_tee},
    {"XC_rightbutton", XC_rightbutton},
    {"XC_rtl_logo", XC_rtl_logo},
    {"XC_sailboat", XC_sailboat},
    {"XC_sb_down_arrow", XC_sb_down_arrow},
    {"XC_sb_h_double_arrow", XC_sb_h_double_arrow},
    {"XC_sb_left_arrow", XC_sb_left_arrow},
    {"XC_sb_right_arrow", XC_sb_right_arrow},
    {"XC_sb_up_arrow", XC_sb_up_arrow},
    {"XC_sb_v_double_arrow", XC_sb_v_double_arrow},
    {"XC_shuttle", XC_shuttle},
    {"XC_sizing", XC_sizing},
    {"XC_spider", XC_spider},
    {"XC_spraycan", XC_spraycan},
    {"XC_star", XC_star},
    {"XC_target", XC_target},
    {"XC_tcross", XC_tcross},
    {"XC_top_left_arrow", XC_top_left_arrow},
    {"XC_top_left_corner", XC_top_left_corner},
    {"XC_top_right_corner", XC_top_right_corner},
    {"XC_top_side", XC_top_side},
    {"XC_top_tee", XC_top_tee},
    {"XC_trek", XC_trek},
    {"XC_ul_angle", XC_ul_angle},
    {"XC_umbrella", XC_umbrella},
    {"XC_ur_angle", XC_ur_angle},
    {"XC_watch", XC_watch},
    {"XC_xterm", XC_xterm},
};
const int g_nCount_CursorNameValue = ( sizeof(g_aCursorNameValue) / sizeof(g_aCursorNameValue[0]) );

//bool X11_XChangePointerControl(Display *display, bool do_accel=false, bool do_threshold=false, int accel_numerator=-1, accel_denominator=-1, int threshold=-1)
//{
//    return (XChangePointerControl(display, do_accel, do_threshold, accel_numerator, accel_denominator, threshold) == 0);
//}

//bool X11_XGrabPointer(
//    Display *display,
//    Window grab_window,
//    Bool owner_events,
//    unsigned int event_mask,
//    int pointer_mode, keyboard_mode,
//    Window confine_to, // Specifies the window to confine the pointer in or None
//    Cursor cursor, // Specifies the cursor that is to be displayed during the grab or None
//    Time time
//    )
//{
//    return true;
//}

bool X11_SetCursor(const std::string &sPath)
{
    //Cursor XCreateFontCursor(Display *display, unsigned int shape);
    //Cursor XCreatePixmapCursor(Display *display, Pixmap source, Pixmap mask, XColor *foreground_color, XColor *background_color, unsigned int x, y);
    //Cursor XCreateGlyphCursor(Display *display, Font source_font, Font mask_font, unsigned int source_char, unsigned int mask_char, XColor *foreground_color, XColor *background_color);
    //int XRecolorCursor(Display *display, Cursor cursor, XColor *foreground_color, XColor *background_color);

    if (sPath == "")
        return false;
    return true;
}

static bool X11_ConstrainMouse_Helper(Display *pDisplay, Window parent_window, int nPosX, int nPosY, int nWidth, int nHeight, std::string *pStringError/*=NULL*/)
{
    _WX_LOG_DBG("(%d, %d, %d, %d)", nPosX, nPosY, nWidth, nHeight);
    static bool bIsActiveConstrainMouse = false;

    // dummy error message string, used when no such string is required
    std::string string_error_dummy;
    if (pStringError == NULL)
        pStringError = &string_error_dummy;
    std::string &sErr = *pStringError;
    sErr = "";

    // check values
    // Size (not including the border) must be nonzero (or a Value error results)
    // Note: The Xlib manual doesn't mention this restriction ?
    if ( (nWidth <= 0) || (nHeight <= 0) )
    {
        sErr = "bad arguments, size must be greater than 0";
        return false;
    }

    static Window window = 0;
    //int nScreen = XDefaultScreen(pDisplay);

    // hide window case
    if ( (nWidth == 0) && (nHeight == 0) )
    {
        if (! bIsActiveConstrainMouse)
        {
            sErr = "window not created";
            return false;
        }
        bIsActiveConstrainMouse = false;
        if (! XUngrabPointer(pDisplay, window))
        {
            sErr = "cannot ungrab the pointer";
            return false;
        }
        if (! XUnmapWindow(pDisplay, window))
        {
            sErr = "cannot hide the window";
            return false;
        }
        if (! XDestroyWindow(pDisplay, window))
        {
            sErr = "cannot destroy the window";
            return false;
        }
        //if (! XCloseDisplay(pDisplay))
        //{
        //    sErr = "cannot close the display";
        //    return false;
        //}
        return true;
    }

    // show window
    if (bIsActiveConstrainMouse)
    {
        sErr = "window is already created";
        return false;
    }

    Visual visual;
    visual.visualid = CopyFromParent;
    //Visual *visual = DefaultVisual(pDisplay, nScreen);
    int nBorderWidth = 0;
    //unsigned long mask = CWEventMask | CWBackPixel;
    //unsigned long mask = 0;
    XSetWindowAttributes win_attr;
    win_attr.event_mask = ExposureMask;    // | ButtonPressMask | KeyPressMask | ButtonReleaseMask;
    win_attr.background_pixel = 0xF0F0F0;

    if (window > 0)
    {
        _WX_LOG_ERR("closing a window previously created");
        if (! XDestroyWindow(pDisplay, window))
        {
            sErr = "cannot destroy the previously created window, it should not exist in the first place";
            return false;
        }
    }

    //window = XCreateWindow(
    //    pDisplay,
    //    parent_window,
    //    nPosX, nPosY, nWidth, nHeight,
    //    nBorderWidth,
    //    DefaultDepth (pDisplay, nScreen),
    //    InputOutput,
    //    &visual,
    //    mask,
    //    &win_attr
    //    );

    window = XCreateSimpleWindow(
        pDisplay,
        parent_window,
        nPosX, nPosY, nWidth, nHeight,
        nBorderWidth,
        0,
        0
        );

    if (window <= 0)
    {
        sErr = "cannot create window";
        return false;
    }
    XClassHint classHint;
    classHint.res_name = "grab_name";
    classHint.res_class = "grab_class";
    if (! XSetClassHint(pDisplay, window, &classHint))
    {
        sErr = "cannot set class name";
        return false;
    }
    if (! XMapWindow(pDisplay, window))
    {
        sErr = "cannot show the window";
        return false;
    }
    XSync(pDisplay, false);
    if (! XLowerWindow(pDisplay, window))
    {
        sErr = "cannot lower the window";
        return false;
    }
    XSync(pDisplay, false);
    int res = XGrabPointer(
        pDisplay, window,
        false,
        ButtonPressMask | ButtonReleaseMask | ButtonMotionMask | EnterWindowMask | LeaveWindowMask | PointerMotionMask,
        GrabModeAsync, // pointer_mode
        GrabModeAsync, // keyboard_mode
        window, // confine_to_window
        None, // cursor // TODO: This may need to be set to the cursor of this window
        CurrentTime );
    XSync(pDisplay, false);
    if (res != GrabSuccess)
    {
        sErr = "cannot grab the pointer";
        if (res == GrabNotViewable)
            sErr = "this is not a viewable window - perhaps not shown yet";
        if (res == AlreadyGrabbed)
            sErr = "pointer already grabbed";
        if (res == GrabFrozen)
            sErr = "pointer frozen by another grab";
        return false;
    }

    // correct the window position (wm may change it after create or show)
    if (! XMoveResizeWindow(pDisplay, window, nPosX, nPosY, nWidth, nHeight))
    {
        sErr = "cannot change the window position";
        return false;
    }
    XSync(pDisplay, false);

    bIsActiveConstrainMouse = true;
    return true;
}

bool X11_ConstrainMouse(int nPosX, int nPosY, int nWidth, int nHeight, std::string *pStringError/*=NULL*/)
{
    static Display *pDisplay = NULL;
    Window parent_window = 0;

    if (pDisplay == NULL)
        pDisplay = XOpenDisplay(NULL);
    parent_window = DefaultRootWindow(pDisplay);

    XLockDisplay(pDisplay);

    bool bResult = X11_ConstrainMouse_Helper(pDisplay, parent_window, nPosX, nPosY, nWidth, nHeight, pStringError);

    //if ( (nWidth == 0) && (nHeight == 0) && bResult )
    //    pDisplay = NULL;
    XSync(pDisplay, false);
    XUnlockDisplay(pDisplay);

    return bResult;
}
