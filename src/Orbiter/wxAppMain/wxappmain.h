//
// Author : C Remus
//
// Changed by : ...
//

#ifndef _WXAPPMAIN_H_
#define _WXAPPMAIN_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wxappmain.h"
#endif

/*!
 * Includes
 */

////@begin includes
#include "wx/image.h"
#include "wxframemain.h"
////@end includes
#include "wx_extern_app.h"

/*!
 * Forward declarations
 */

////@begin forward declarations
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
////@end control identifiers

/*!
 * wxAppMain class declaration
 */

class wxAppMain: public wxApp
{
    DECLARE_CLASS( wxAppMain )
        ;
    DECLARE_EVENT_TABLE()
        ;

  public:
    /// Constructor
    wxAppMain();

    /// Initialises the application
    virtual bool OnInit();

    /// Called on exit
    virtual int OnExit();

////@begin wxAppMain event handler declarations

    /// wxEVT_IDLE event handler for ID_UNIDENTIFIED
    void OnIdle( wxIdleEvent& event );

////@end wxAppMain event handler declarations

////@begin wxAppMain member function declarations

////@end wxAppMain member function declarations

////@begin wxAppMain member variables
////@end wxAppMain member variables

public:
    ~wxAppMain();
    virtual int OnRun();

protected:
    void OnEvent_Dialog(wxCommandEvent& event);
    void OnTimer_Debug(wxTimerEvent& event);
    void OnTimer_ExternApp(wxTimerEvent& event);
    wxTimer v_oTimer_Debug;
    wxTimer v_oTimer_ExternApp;
    ExternApp *v_pExternApp;
};

/*!
 * Application instance declaration
 */

////@begin declare app
DECLARE_APP(wxAppMain)
////@end declare app
;

#endif
// _WXAPPMAIN_H_
