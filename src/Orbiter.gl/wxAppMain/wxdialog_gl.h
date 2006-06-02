//
// Author : Remus C.
//
// Changed by : ...
//

// OpenGL demo code copied here from wxGLCanvas cube demo

#ifndef _WXDIALOG_GL_H_
#define _WXDIALOG_GL_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wxdialog_gl.h"
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
#define ID_DIALOG_GL 10048
#define SYMBOL_WXDIALOG_GL_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_WXDIALOG_GL_TITLE _T("OpenGL Demo")
#define SYMBOL_WXDIALOG_GL_IDNAME ID_DIALOG_GL
#define SYMBOL_WXDIALOG_GL_SIZE wxSize(150, 150)
#define SYMBOL_WXDIALOG_GL_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * wxDialog_GL class declaration
 */

class wxDialog_GL: public wxDialog_Base
{
    DECLARE_DYNAMIC_CLASS( wxDialog_GL );
    DECLARE_EVENT_TABLE();

public:
    /// Constructors
    wxDialog_GL( );
    wxDialog_GL( wxWindow* parent, wxWindowID id = SYMBOL_WXDIALOG_GL_IDNAME, const wxString& caption = SYMBOL_WXDIALOG_GL_TITLE, const wxPoint& pos = SYMBOL_WXDIALOG_GL_POSITION, const wxSize& size = SYMBOL_WXDIALOG_GL_SIZE, long style = SYMBOL_WXDIALOG_GL_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_WXDIALOG_GL_IDNAME, const wxString& caption = SYMBOL_WXDIALOG_GL_TITLE, const wxPoint& pos = SYMBOL_WXDIALOG_GL_POSITION, const wxSize& size = SYMBOL_WXDIALOG_GL_SIZE, long style = SYMBOL_WXDIALOG_GL_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin wxDialog_GL event handler declarations

////@end wxDialog_GL event handler declarations

////@begin wxDialog_GL member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end wxDialog_GL member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin wxDialog_GL member variables
////@end wxDialog_GL member variables

protected:
    ~wxDialog_GL();
    static int v_nRefCount;
    class Demo_GLCanvas *v_pDemo_GLCanvas;
};

#endif
// _WXDIALOG_GL_H_
