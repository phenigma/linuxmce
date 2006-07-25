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
#include "wrapper_x11.h"
#include "wrapper_sdl.h"

////@begin XPM images
#include "cancel.xpm"
#include "ok.xpm"
#include "exit.xpm"
////@end XPM images

bool wx_SetCursor_Image(const wxString &sPath);
bool SDL_SetCursor_Image(const std::string &sPath);

bool wx_SetCursor_Font(int nShape);
bool SDL_SetCursor_Font(int nShape);

struct struct_string_int
{
    std::string sName;
    int nValue;
};
extern struct_string_int g_aCursorNameValues[];
extern const int g_nCount_CursorNameValues;

// return -1 on error
int X11Cursor_GetValueByName(const std::string &sName);

/*!
 * wxDialog_Pointer type definition
 */

IMPLEMENT_DYNAMIC_CLASS( wxDialog_Pointer, wxDialog_Base );

/*!
 * wxDialog_Pointer event table definition
 */

BEGIN_EVENT_TABLE( wxDialog_Pointer, wxDialog_Base )

////@begin wxDialog_Pointer event table entries
    EVT_BUTTON( ID_BUTTON_MOUSE_CONSTRAIN, wxDialog_Pointer::OnButtonMouseConstrainClick )

    EVT_BUTTON( wxID_CLOSE, wxDialog_Pointer::OnCloseClick )

    EVT_BUTTON( ID_BUTTON_LOADIMAGE, wxDialog_Pointer::OnButtonLoadimageClick )

    EVT_LISTBOX( ID_LISTBOX, wxDialog_Pointer::OnListboxSelected )

    EVT_BUTTON( ID_BUTTON_ROOTWINDOW, wxDialog_Pointer::OnButtonRootwindowClick )

////@end wxDialog_Pointer event table entries

    END_EVENT_TABLE();

/*!
 * wxDialog_Pointer constructors
 */

wxDialog_Pointer::wxDialog_Pointer( )
        : v_pX11(NULL)
        , v_pX11_Locker_NewDisplay(NULL)
{
    X11_Init();
}

wxDialog_Pointer::wxDialog_Pointer( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
        : v_pX11(NULL)
        , v_pX11_Locker_NewDisplay(NULL)
{
    X11_Init();
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
    v_pButton_Mouse_Constrain = NULL;
    v_pButton_Close = NULL;
    v_pSizerV_Label = NULL;
    v_pSizerV_Pointer = NULL;
    v_pButton_LoadImage = NULL;
    v_pStaticText_Message = NULL;
    v_pListBox = NULL;
    v_pButton_RootWindow = NULL;
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
    _LOG_NFO();
////@begin wxDialog_Pointer content construction
    wxDialog_Pointer* itemDialog_Base1 = this;

    v_pBoxV_all = new wxBoxSizer(wxVERTICAL);
    itemDialog_Base1->SetSizer(v_pBoxV_all);

    v_pBoxH_Buttons = new wxBoxSizer(wxHORIZONTAL);
    v_pBoxV_all->Add(v_pBoxH_Buttons, 0, wxGROW|wxALL, 5);

    v_pButton_Mouse_Constrain = new wxButton;
    v_pButton_Mouse_Constrain->Create( itemDialog_Base1, ID_BUTTON_MOUSE_CONSTRAIN, _T("Constrain Mouse"), wxDefaultPosition, wxDefaultSize, 0 );
    v_pBoxH_Buttons->Add(v_pButton_Mouse_Constrain, 1, wxGROW|wxALL, 5);

    wxBitmap v_pButton_CloseBitmap(itemDialog_Base1->GetBitmapResource(wxT("cancel.png")));
    v_pButton_Close = new wxBitmapButton;
    v_pButton_Close->Create( itemDialog_Base1, wxID_CLOSE, v_pButton_CloseBitmap, wxDefaultPosition, wxDefaultSize, 0 );
    wxBitmap v_pButton_CloseBitmapSel(itemDialog_Base1->GetBitmapResource(wxT("ok.png")));
    v_pButton_Close->SetBitmapSelected(v_pButton_CloseBitmapSel);
    wxBitmap v_pButton_CloseBitmapFocus(itemDialog_Base1->GetBitmapResource(wxT("exit.png")));
    v_pButton_Close->SetBitmapFocus(v_pButton_CloseBitmapFocus);
    v_pBoxH_Buttons->Add(v_pButton_Close, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    v_pSizerV_Label = new wxStaticBox(itemDialog_Base1, wxID_ANY, _T("Pointer Shape"));
    v_pSizerV_Pointer = new wxStaticBoxSizer(v_pSizerV_Label, wxVERTICAL);
    v_pBoxV_all->Add(v_pSizerV_Pointer, 1, wxGROW|wxALL, 5);

    v_pButton_LoadImage = new wxButton;
    v_pButton_LoadImage->Create( itemDialog_Base1, ID_BUTTON_LOADIMAGE, _T("Load Pointer Image"), wxDefaultPosition, wxDefaultSize, 0 );
    v_pButton_LoadImage->SetDefault();
    v_pSizerV_Pointer->Add(v_pButton_LoadImage, 0, wxGROW|wxALL, 5);

    v_pStaticText_Message = new wxStaticText;
    v_pStaticText_Message->Create( itemDialog_Base1, wxID_STATIC, _T("OR Select a standard X11 cursor :"), wxDefaultPosition, wxDefaultSize, 0 );
    v_pSizerV_Pointer->Add(v_pStaticText_Message, 0, wxGROW|wxALL|wxADJUST_MINSIZE, 5);

    wxString v_pListBoxStrings[] = {
        _T("11"),
        _T("22"),
        _T("33"),
        _T("44")
    };
    v_pListBox = new wxListBox;
    v_pListBox->Create( itemDialog_Base1, ID_LISTBOX, wxDefaultPosition, wxDefaultSize, 4, v_pListBoxStrings, wxLB_SINGLE );
    v_pListBox->SetStringSelection(_T("22"));
    v_pSizerV_Pointer->Add(v_pListBox, 1, wxGROW|wxALL, 5);

    v_pButton_RootWindow = new wxButton;
    v_pButton_RootWindow->Create( itemDialog_Base1, ID_BUTTON_ROOTWINDOW, _T("Root Window Pointer Shape"), wxDefaultPosition, wxDefaultSize, 0 );
    v_pButton_RootWindow->Show(false);
    v_pSizerV_Pointer->Add(v_pButton_RootWindow, 0, wxGROW|wxALL, 5);

    v_pStaticText = new wxStaticText;
    v_pStaticText->Create( itemDialog_Base1, wxID_STATIC, _T("Static text Static text Static text Static text Static text Static text Static text Static text "), wxDefaultPosition, wxDefaultSize, 0 );
    v_pSizerV_Pointer->Add(v_pStaticText, 0, wxGROW|wxALL|wxADJUST_MINSIZE, 5);

////@end wxDialog_Pointer content construction
    ::wxInitAllImageHandlers();
    v_pStaticText->SetLabel("");
    v_pListBox->Clear();
    for (int i=0; i<g_nCount_CursorNameValues; ++i)
    {
        v_pListBox->Append(g_aCursorNameValues[i].sName);
    }
    v_pListBox->SetSelection(-1);
    // wxFileDialog
    wxFileName oFileName("./data/pointers_bw/");
    oFileName.Normalize();
    v_sMessage = "Open Pointer Image";
    v_sDir = oFileName.GetFullPath();
    v_sFilename = "verybig.xbm";
    v_sWildcard = wxString::Format(
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
        );
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_CONSTRAINMOUSE
 */

void wxDialog_Pointer::OnButtonMouseConstrainClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_CONSTRAINMOUSE in wxDialog_Pointer.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_CONSTRAINMOUSE in wxDialog_Pointer.
    if (! v_pX11->Mouse_IsConstrainActive())
    {
        v_pX11->Mouse_Constrain_ReactivateIfActive();
        v_pX11->Mouse_Constrain(100, 100, 200, 100);
    }
    else
    {
        v_pX11->Mouse_Constrain_ReactivateIfActive();
    }
    //v_pX11->Mouse_SetPosition(500, 500);
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

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LOADIMAGE
 */

void wxDialog_Pointer::OnButtonLoadimageClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LOADIMAGE in wxDialog_Pointer.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LOADIMAGE in wxDialog_Pointer.
    v_pListBox->SetSelection(-1);
    int nFlags = wxOPEN | wxFILE_MUST_EXIST;
    _LOG_NFO("v_sDir='%s', v_sFilename='%s'", v_sDir.c_str(), v_sFilename.c_str());
    wxFileDialog oFileDialog(this, v_sMessage, v_sDir, v_sFilename, v_sWildcard, nFlags);
    if (wxID_CANCEL == oFileDialog.ShowModal())
        return;
    v_sDir = oFileDialog.GetDirectory();
    v_sFilename = oFileDialog.GetFilename();
    wxString sPath = oFileDialog.GetPath();
    wxString sPathMask = sPath + ".msk";
    _COND_RET(! sPath.IsEmpty());
    _LOG_NFO("sPath='%s'", sPath.c_str());
    if (wx_SetCursor_Image(sPath))
    {
        v_pStaticText->SetLabel(sPath);
    }
    if (v_pX11->Mouse_SetCursor_Image(v_pX11->Debug_Window(), sPath.c_str(), sPathMask.c_str()))
    {
        v_pStaticText->SetLabel(sPath);
    }
}

/*!
 * wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_LISTBOX
 */

void wxDialog_Pointer::OnListboxSelected( wxCommandEvent& event )
{
    _LOG_NFO("%s", _str_event(event));
    if (! event.IsSelection())
    {
        return;
    }
////@begin wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_LISTBOX in wxDialog_Pointer.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_LISTBOX in wxDialog_Pointer.
    wxString sItem = event.GetString();
    int nShape = X11Cursor_GetValueByName(sItem.c_str());
    if (nShape < 0)
    {
        _LOG_ERR("Not a choice string : '%s'", sItem.c_str());
        return;
    }
    _LOG_NFO("X11 shape : %d", nShape);
    if (v_pX11->Mouse_SetCursor_Font(v_pX11->Debug_Window(), nShape))
    {
        v_pStaticText->SetLabel(sItem);
    }
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_ROOTWINDOW
 */

void wxDialog_Pointer::OnButtonRootwindowClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_ROOTWINDOW in wxDialog_Pointer.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_ROOTWINDOW in wxDialog_Pointer.
    v_pListBox->SetSelection(-1);
    v_pX11->Mouse_SetCursor_Inherit(v_pX11->Debug_Window());
    v_pStaticText->SetLabel("Inherit from parent window");
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

wxDialog_Pointer::~wxDialog_Pointer()
{
    _LOG_NFO();
    ::wxSetCursor(wxNullCursor);
    wxDELETE(v_pX11);
    wxDELETE(v_pX11_Locker_NewDisplay);
}

void wxDialog_Pointer::X11_Init()
{
    //v_pX11 = new X11wrapper((Display *)wxGetDisplay());
    v_pX11_Locker_NewDisplay = new X11_Locker_NewDisplay();
    v_pX11 = new X11wrapper(v_pX11_Locker_NewDisplay->GetDisplay());
}

bool wx_SetCursor_Image(const wxString &sPath)
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

bool wx_SetCursor_Font(int nShape)
{
    wxCursor oCursor(nShape);
    _COND_RET(oCursor.Ok(), false);
    ::wxSetCursor(oCursor);
    return true;
}

bool SDL_SetCursor_Image(const std::string &sPath)
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

bool SDL_SetCursor_Font(const std::string &sPath)
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
struct_string_int g_aCursorNameValues[] =
{
    // bad value : {"XC_num_glyphs", XC_num_glyphs},
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
const int g_nCount_CursorNameValues = ( sizeof(g_aCursorNameValues) / sizeof(g_aCursorNameValues[0]) );

int X11Cursor_GetValueByName(const std::string &sName)
{
    for (int i=0; i<g_nCount_CursorNameValues; ++i)
    {
        if (g_aCursorNameValues[i].sName == sName)
            return g_aCursorNameValues[i].nValue;
    }
    return -1;
}
