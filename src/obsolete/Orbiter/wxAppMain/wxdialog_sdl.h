//
// Author : Remus C.
//
// Changed by : ...
//

// SDL demo code copied here from wx-sdl tutorial

#ifndef _WXDIALOG_SDL_H_
#define _WXDIALOG_SDL_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wxdialog_sdl.h"
#endif

/*!
 * Includes
 */

////@begin includes
////@end includes
#include "wxdialog_base.h"

/*!
 * Forward declarations
 */

////@begin forward declarations
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_DIALOG_SDL 10049
#define SYMBOL_WXDIALOG_SDL_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_WXDIALOG_SDL_TITLE _T("SDL Demo")
#define SYMBOL_WXDIALOG_SDL_IDNAME ID_DIALOG_SDL
#define SYMBOL_WXDIALOG_SDL_SIZE wxSize(150, 150)
#define SYMBOL_WXDIALOG_SDL_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * wxDialog_SDL class declaration
 */

class wxDialog_SDL: public wxDialog_Base
{
    DECLARE_DYNAMIC_CLASS( wxDialog_SDL );
    DECLARE_EVENT_TABLE();

public:
    /// Constructors
    wxDialog_SDL( );
    wxDialog_SDL( wxWindow* parent, wxWindowID id = SYMBOL_WXDIALOG_SDL_IDNAME, const wxString& caption = SYMBOL_WXDIALOG_SDL_TITLE, const wxPoint& pos = SYMBOL_WXDIALOG_SDL_POSITION, const wxSize& size = SYMBOL_WXDIALOG_SDL_SIZE, long style = SYMBOL_WXDIALOG_SDL_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_WXDIALOG_SDL_IDNAME, const wxString& caption = SYMBOL_WXDIALOG_SDL_TITLE, const wxPoint& pos = SYMBOL_WXDIALOG_SDL_POSITION, const wxSize& size = SYMBOL_WXDIALOG_SDL_SIZE, long style = SYMBOL_WXDIALOG_SDL_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin wxDialog_SDL event handler declarations

    /// wxEVT_MOTION event handler for ID_DIALOG_SDL
    void OnMotion( wxMouseEvent& event );

////@end wxDialog_SDL event handler declarations

////@begin wxDialog_SDL member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end wxDialog_SDL member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin wxDialog_SDL member variables
////@end wxDialog_SDL member variables

protected:
    ~wxDialog_SDL();
    static int v_nRefCount;
    class Demo_PanelSDL *v_pDemo_PanelSDL;
};

#endif
// _WXDIALOG_SDL_H_
