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
#include "wrapper_wx.h"

////@begin XPM images
#include "cancel.xpm"
#include "ok.xpm"
#include "exit.xpm"
////@end XPM images

#define INTERVAL_TIMER_CHECKMOUSE_MSEC 100

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

    EVT_BUTTON( ID_BUTTON_INHERIT, wxDialog_Pointer::OnButtonInheritClick )

    EVT_BUTTON( ID_BUTTON_HIDE, wxDialog_Pointer::OnButtonHideClick )

    EVT_LISTBOX( ID_LISTBOX, wxDialog_Pointer::OnListboxSelected )

////@end wxDialog_Pointer event table entries

    EVT_TIMER(ID_Timer_CheckMouse, wxDialog_Pointer::OnTimer_CheckMouse)

    END_EVENT_TABLE();

/*!
 * wxDialog_Pointer constructors
 */

wxDialog_Pointer::wxDialog_Pointer( )
        : v_pX11(NULL)
        , v_pX11_Locker_NewDisplay(NULL)
        , v_oTimer_CheckMouse(this, ID_Timer_CheckMouse)
{
    X11_Init();
}

wxDialog_Pointer::wxDialog_Pointer( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
        : v_pX11(NULL)
        , v_pX11_Locker_NewDisplay(NULL)
        , v_oTimer_CheckMouse(this, ID_Timer_CheckMouse)
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
    v_pButton_Inherit = NULL;
    v_pButton_Hide = NULL;
    v_pStaticText_Message = NULL;
    v_pListBox = NULL;
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

    v_pButton_Inherit = new wxButton;
    v_pButton_Inherit->Create( itemDialog_Base1, ID_BUTTON_INHERIT, _T("Inherit Pointer Shape"), wxDefaultPosition, wxDefaultSize, 0 );
    v_pButton_Inherit->Show(false);
    v_pSizerV_Pointer->Add(v_pButton_Inherit, 0, wxGROW|wxALL, 5);

    v_pButton_Hide = new wxButton;
    v_pButton_Hide->Create( itemDialog_Base1, ID_BUTTON_HIDE, _T("Hide Pointer"), wxDefaultPosition, wxDefaultSize, 0 );
    v_pSizerV_Pointer->Add(v_pButton_Hide, 0, wxGROW|wxALL, 5);

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

    v_pStaticText = new wxStaticText;
    v_pStaticText->Create( itemDialog_Base1, wxID_STATIC, _T("Static text Static text Static text Static text Static text Static text Static text Static text "), wxDefaultPosition, wxDefaultSize, 0 );
    v_pSizerV_Pointer->Add(v_pStaticText, 0, wxGROW|wxALL|wxADJUST_MINSIZE, 5);

////@end wxDialog_Pointer content construction
    ::wxInitAllImageHandlers();
    v_pStaticText->SetLabel("");
    v_pListBox->Clear();
    for (int i=0; i<X11wrapper::v_aStdCursors_Count; ++i)
    {
        v_pListBox->Append(X11wrapper::v_aStdCursors_NameValue[i].sName);
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
    v_oTimer_CheckMouse.Start(INTERVAL_TIMER_CHECKMOUSE_MSEC);
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
    if (WXwrapper::SetCursor_Image(sPath.c_str()))
    {
        v_pStaticText->SetLabel(sPath);
    }
    if (v_pX11->Mouse_SetCursor_Image(v_pX11->Debug_Window(), sPath.c_str(), sPathMask.c_str()))
    {
        v_pStaticText->SetLabel(sPath);
    }
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_INHERIT
 */

void wxDialog_Pointer::OnButtonInheritClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_INHERIT in wxDialog_Pointer.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_INHERIT in wxDialog_Pointer.
    v_pListBox->SetSelection(-1);
    if (v_pX11->Mouse_SetCursor_Inherit(v_pX11->Debug_Window()))
    {
        v_pStaticText->SetLabel("Inherit from parent window");
    }
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_HIDE
 */

void wxDialog_Pointer::OnButtonHideClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_HIDE in wxDialog_Pointer.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_HIDE in wxDialog_Pointer.
    v_pListBox->SetSelection(-1);
    if (v_pX11->Mouse_HideCursor(v_pX11->Debug_Window()))
    {
        v_pStaticText->SetLabel("Hidden pointer");
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
    int nShape = X11wrapper::StdCursor_GetValueByName(sItem.c_str());
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
    v_oTimer_CheckMouse.Stop();
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

void wxDialog_Pointer::OnTimer_CheckMouse(wxTimerEvent& event)
{
    _COND_RET(event.GetId() == ID_Timer_CheckMouse);
    int x=0;
    int y=0;
    v_pX11->Mouse_GetPosition(x, y);
    _LOG_NFO("x==%d, y==%d", x, y);
}
