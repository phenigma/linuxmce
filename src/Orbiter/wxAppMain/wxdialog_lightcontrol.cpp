#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "wxdialog_lightcontrol.h"
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

#include "wxdialog_lightcontrol.h"

////@begin XPM images
////@end XPM images

/*!
 * wxDialog_LightControl type definition
 */

IMPLEMENT_DYNAMIC_CLASS( wxDialog_LightControl, wxDialog_Base )

/*!
 * wxDialog_LightControl event table definition
 */

    BEGIN_EVENT_TABLE( wxDialog_LightControl, wxDialog_Base )

////@begin wxDialog_LightControl event table entries
    EVT_CLOSE( wxDialog_LightControl::OnCloseWindow )

////@end wxDialog_LightControl event table entries

    END_EVENT_TABLE()

/*!
 * wxDialog_LightControl constructors
 */

    wxDialog_LightControl::wxDialog_LightControl( )
{
}

wxDialog_LightControl::wxDialog_LightControl( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Create(parent, id, caption, pos, size, style);
}

/*!
 * wxDialog_LightControl creator
 */

bool wxDialog_LightControl::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin wxDialog_LightControl member initialisation
    v_pBoxH_all = NULL;
    v_pWinLight = NULL;
////@end wxDialog_LightControl member initialisation

////@begin wxDialog_LightControl creation
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS|wxWS_EX_TRANSIENT);
    wxDialog_Base::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
////@end wxDialog_LightControl creation
    return true;
}

/*!
 * Control creation for wxDialog_LightControl
 */

void wxDialog_LightControl::CreateControls()
{
////@begin wxDialog_LightControl content construction
    wxDialog_LightControl* itemDialog_Base1 = this;

    v_pBoxH_all = new wxBoxSizer(wxHORIZONTAL);
    itemDialog_Base1->SetSizer(v_pBoxH_all);

    v_pWinLight = new wxWindow;
    v_pWinLight->Create( itemDialog_Base1, ID_WINDOW_LIGHT, wxDefaultPosition, itemDialog_Base1->ConvertDialogToPixels(wxSize(20, 200)), wxNO_BORDER|wxCLIP_CHILDREN |wxHSCROLL|wxVSCROLL );
    v_pWinLight->SetBackgroundColour(wxColour(30, 144, 255));
    v_pBoxH_all->Add(v_pWinLight, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

////@end wxDialog_LightControl content construction
}

/*!
 * Should we show tooltips?
 */

bool wxDialog_LightControl::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap wxDialog_LightControl::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin wxDialog_LightControl bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end wxDialog_LightControl bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon wxDialog_LightControl::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin wxDialog_LightControl icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end wxDialog_LightControl icon retrieval
}
/*!
 * wxEVT_CLOSE_WINDOW event handler for ID_DIALOG_LIGHTCONTROL
 */

void wxDialog_LightControl::OnCloseWindow( wxCloseEvent& WXUNUSED(event) )
{
////@begin wxEVT_CLOSE_WINDOW event handler for ID_DIALOG_LIGHTCONTROL in wxDialog_LightControl.
    // Before editing this code, remove the block markers.
    wxWindow* window = this;
    window->Destroy();
////@end wxEVT_CLOSE_WINDOW event handler for ID_DIALOG_LIGHTCONTROL in wxDialog_LightControl. 
}


