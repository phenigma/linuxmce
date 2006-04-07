//
// Author : C Remus
//
// Changed by : ...
//

#ifndef _WX_HELPERS_H_
#define _WX_HELPERS_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wx_helpers.h"
#endif

WX_DEFINE_ARRAY(bool, wxArrayBool);

typedef void (* type_ptr_void_fn_void)();
typedef void (& type_ref_void_fn_void)();

// (HasScrollbar(wxHORIZONTAL)) ? GetScrollThumb(wxHORIZONTAL) : 0
// (HasScrollbar(wxVERTICAL)) ? GetScrollThumb(wxVERTICAL) : 0
// does not work for every window
const int WX_ScrollBarSize = 15;
const int WX_GridLineSize = 1;

// remove escape sequences
wxString NoEsc(const wxString &str);

// conversion function
wxString Str(int val);

const char * _str_enum(wxSemaError value);

const char * _str_event(wxEvent &event);

// can be used in threads, or in main thread
void wx_sleep(unsigned long int n_seconds, unsigned long int n_add_milliseconds=0);

void wx_post_event(wxEvtHandler *pDestEvtHandler, WXTYPE event_type, int event_id, const char *sInfo="", void *pData=NULL, int nInt=0);

wxSemaError wx_semaphore_post(wxSemaphore &rSemaphore);
wxSemaError wx_semaphore_wait(wxSemaphore &rSemaphore);

// only if condition is true
// launches the given functions
// when the object is created, respectively destroyed
class wxCondLocker
{
  public:
    wxCondLocker(bool bCondition, type_ref_void_fn_void fn_launch_enter, type_ref_void_fn_void fn_launch_leave);
    ~wxCondLocker();
  protected:
    bool v_bLocked; // functions called
    type_ref_void_fn_void v_fn_launch_enter;
    type_ref_void_fn_void v_fn_launch_leave;
};

// wxapp is ready
// controls created
// event loop started
bool App_IsReady();

// set ready status
void App_SetReady();

int App_GetExitCode();               // get application exit code
void App_SetExitCode(int nExitCode); // set application exit code

bool App_ShouldExit();                 // get signal : app should exit
void App_SetShouldExit(bool bShouldExit); // set signal : app should exit

// should be called from the thread
// return true if the thread should be stopped
bool wxIdleThreadShouldStop();

class wxGrid;
void wx_Clear(wxGrid *pGrid);
void wx_Align(wxGrid *pGrid, wxAlignment horiz=wxALIGN_CENTRE, wxAlignment vert=wxALIGN_CENTRE);
void wx_Set_Color(wxGrid *pGrid, wxColour bg, wxColour fg);
void wx_Set_Color(wxGrid *pGrid, wxColour bg, wxColour fg, wxColour line);
int wx_GetTextExtent(wxWindow *pWindow, const wxArrayString &aStrings, int *pReturnHeight=NULL);
void wx_Grid_Resize_Column(wxGrid *pGrid, int idxResizableColumn);

#endif
// _WX_HELPERS_H_
