#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "wxdialog_volumecontrol.h"
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

#include "wxdialog_volumecontrol.h"

////@begin XPM images
////@end XPM images

/*!
 * wxDialog_VolumeControl type definition
 */

IMPLEMENT_DYNAMIC_CLASS( wxDialog_VolumeControl, wxDialog_Base )

/*!
 * wxDialog_VolumeControl event table definition
 */

    BEGIN_EVENT_TABLE( wxDialog_VolumeControl, wxDialog_Base )

////@begin wxDialog_VolumeControl event table entries
    EVT_CLOSE( wxDialog_VolumeControl::OnCloseWindow )

////@end wxDialog_VolumeControl event table entries

    END_EVENT_TABLE()

/*!
 * wxDialog_VolumeControl constructors
 */

    wxDialog_VolumeControl::wxDialog_VolumeControl( )
{
}

wxDialog_VolumeControl::wxDialog_VolumeControl( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Create(parent, id, caption, pos, size, style);
}

/*!
 * wxDialog_VolumeControl creator
 */

bool wxDialog_VolumeControl::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin wxDialog_VolumeControl member initialisation
    v_pBoxV_all = NULL;
    v_pWinVOLUME = NULL;
////@end wxDialog_VolumeControl member initialisation

////@begin wxDialog_VolumeControl creation
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS|wxWS_EX_TRANSIENT);
    wxDialog_Base::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
////@end wxDialog_VolumeControl creation
    return true;
}

/*!
 * Control creation for wxDialog_VolumeControl
 */

void wxDialog_VolumeControl::CreateControls()
{
////@begin wxDialog_VolumeControl content construction
    wxDialog_VolumeControl* itemDialog_Base1 = this;

    v_pBoxV_all = new wxBoxSizer(wxVERTICAL);
    itemDialog_Base1->SetSizer(v_pBoxV_all);

    v_pWinVOLUME = new wxWindow;
    v_pWinVOLUME->Create( itemDialog_Base1, ID_WINDOW_VOLUME, wxDefaultPosition, itemDialog_Base1->ConvertDialogToPixels(wxSize(200, 20)), wxNO_BORDER|wxCLIP_CHILDREN |wxHSCROLL|wxVSCROLL );
    v_pWinVOLUME->SetBackgroundColour(wxColour(30, 144, 255));
    v_pBoxV_all->Add(v_pWinVOLUME, 1, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

////@end wxDialog_VolumeControl content construction
}

/*!
 * Should we show tooltips?
 */

bool wxDialog_VolumeControl::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap wxDialog_VolumeControl::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin wxDialog_VolumeControl bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end wxDialog_VolumeControl bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon wxDialog_VolumeControl::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin wxDialog_VolumeControl icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end wxDialog_VolumeControl icon retrieval
}
/*!
 * wxEVT_CLOSE_WINDOW event handler for ID_DIALOG_VOLUMECONTROL
 */

void wxDialog_VolumeControl::OnCloseWindow( wxCloseEvent& WXUNUSED(event) )
{
////@begin wxEVT_CLOSE_WINDOW event handler for ID_DIALOG_VOLUMECONTROL in wxDialog_VolumeControl.
    // Before editing this code, remove the block markers.
    wxWindow* window = this;
    window->Destroy();
////@end wxEVT_CLOSE_WINDOW event handler for ID_DIALOG_VOLUMECONTROL in wxDialog_VolumeControl. 
}


