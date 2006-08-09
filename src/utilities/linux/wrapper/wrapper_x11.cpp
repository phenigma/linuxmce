//
// Author : Remus C.
//
// Changed by : ...
//

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "wrapper_x11.h"
#endif

#include "wrapper_x11.h"
#include <X11/extensions/shape.h>

#ifdef USE_DEBUG_CODE
#include "define_all.h"
#else
#include "../../defines/define_all.h"
#endif // USE_DEBUG_CODE

// do not use the error handler grabber
static const bool g_bNoErrorHandler    = getenv("X11_NO_ERRORHANDLER");

// synchronize the entire app, for debug purposes
// WARNING: the app will run slowly
static const bool g_bXSynchronize_All  = getenv("X11_XSYNCHRONIZE_ALL");

// synchronize each lock-unlock calls, for debug purposes
// WARNING: the app will run slowly
static const bool g_bXSynchronize_Each = getenv("X11_XSYNCHRONIZE_EACH");

// call a log function after all Xlib functions that generate protocol requests
// for debug purposes
// WARNING: the app will run very very slow
static const bool g_bXAfterFunction  = getenv("X11_XAFTERFUNCTION");

// WARNING: the app will behave very badly
static const bool g_bXNoLock  = getenv("X11_NO_XLOCK");

// algorithm
/*
  {
  // X11 result or status
  int code = -1;
  X11_Locker x11_locker(GetDisplay());
  do
  {
  .......
  code = Xfunction(args);
  _COND_XERROR_LOG_BREAK(code);
  .......
  _LOG_XERROR_BREAK("error");
  .......
  code = 0;
  } while (0);
  return IsReturnCodeOk(code);
  }
*/

// code : X11 result or status
#define _COND_XERROR_LOG_BREAK(code) \
    if (! IsReturnCodeOk(code)) \
    { \
        _LOG_ERR("%s", GetErrorText(GetDisplay(), code).c_str()); \
        break; \
    } \
    do {} while (0)

#define _LOG_XERROR_BREAK(...) \
    if (true) \
    { \
        _LOG_ERR(__VA_ARGS__); \
        break; \
    } \
    else do {} while (0)

// sync and lock functions implementation

bool X11_Sync(Display *pDisplay)
{
    if (pDisplay == NULL)
    {
        _LOG_ERR("NULL Display pointer");
        return false;
    }
    //_LOG_NFO("pDisplay==%p", pDisplay);
    //XFlush(pDisplay); // already called by XSync
    XSync(pDisplay, false);
    return true;
}

bool X11_Lock(Display *pDisplay)
{
    if (pDisplay == NULL)
    {
        _LOG_ERR("NULL Display pointer");
        return false;
    }
    //_LOG_NFO("pDisplay==%p", pDisplay);
    if (g_bXSynchronize_Each)
        XSynchronize(pDisplay, true);
    if (! g_bXNoLock)
        XLockDisplay(pDisplay);
    return true;
}

bool X11_Unlock(Display *pDisplay)
{
    if (pDisplay == NULL)
    {
        _LOG_ERR("NULL Display pointer");
        return false;
    }
    //_LOG_NFO("pDisplay==%p", pDisplay);
    X11_Sync(pDisplay);
    if (! g_bXNoLock)
        XUnlockDisplay(pDisplay);
    if (g_bXSynchronize_Each)
        XSynchronize(pDisplay, false);
    return true;
}

// class X11_Locker implementation

X11_Locker::X11_Locker(Display *pDisplay)
        : v_pDisplay(pDisplay)
{
    X11_Lock(v_pDisplay);
}

X11_Locker::~X11_Locker()
{
    X11_Unlock(v_pDisplay);
}

// class X11_Locker_NewDisplay implementation

X11_Locker_NewDisplay::X11_Locker_NewDisplay()
        : v_pDisplay(NULL)
{
    v_pDisplay = XOpenDisplay(NULL);
    X11_Lock(v_pDisplay);
}

X11_Locker_NewDisplay::~X11_Locker_NewDisplay()
{
    X11_Unlock(v_pDisplay);
    if (v_pDisplay)
        XCloseDisplay(v_pDisplay);
}

Display * X11_Locker_NewDisplay::GetDisplay()
{
    return v_pDisplay;
}

// class X11wrapper implementation

X11wrapper::X11wrapper(Display * pDisplay/*=NULL*/)
        : v_pDisplay(NULL)
        , v_bIsAssigned_Display(false)
        , v_oMainWindow(0)
        , v_pOld_XErrorHandler(NULL)
        , v_bIsChanged_XErrorHandler(false)
        , v_bIsActive_Mouse_Constrain(false)
        , v_window_Mouse_Constrain(0)
        , v_bExtension_Shape_IsInitialized(false)
        , v_bExtension_Shape_IsAvailable(false)
        , v_nExtension_Shape_Version_Major(0)
        , v_nExtension_Shape_Version_Minor(0)
        , v_pOld_X_AfterFunction(NULL)
        , v_bIsChanged_X_AfterFunction(false)
{
    _LOG_WRN("env : X11_NO_ERRORHANDLER==%d", g_bNoErrorHandler);
    _LOG_WRN("env : X11_XSYNCHRONIZE_ALL==%d", g_bXSynchronize_All);
    _LOG_WRN("env : X11_XSYNCHRONIZE_EACH==%d", g_bXSynchronize_Each);
    _LOG_WRN("env : X11_XAFTERFUNCTION==%d", g_bXAfterFunction);
    _LOG_WRN("env : X11_NO_XLOCK==%d", g_bXNoLock);
    _LOG_NFO("pDisplay==%p", pDisplay);
    if (pDisplay)
        Assign_Display(pDisplay);
    if (! g_bNoErrorHandler)
        ErrorHandler_Set();
#ifdef USE_XDGA
    _COND(XDGA_Init());
#endif // USE_XDGA
}

X11wrapper::~X11wrapper()
{
    _LOG_NFO("pDisplay==%p", GetDisplay());
#ifdef USE_XDGA
    _COND(XDGA_Exit());
#endif // USE_XDGA
    Clean_Exit();
}

bool X11wrapper::IsInitialized()
{
    _LOG_NFO("pDisplay==%p", GetDisplay());
    return (GetDisplay() != NULL);
}

bool X11wrapper::Clean_Exit()
{
    if (GetDisplay() == NULL)
        return true;
    _LOG_NFO();
    if (v_bIsChanged_X_AfterFunction)
        AfterFunction_Restore();
    if (v_bIsChanged_XErrorHandler)
        ErrorHandler_Restore();
    Mouse_Constrain_Release();
    Display_Close();
    v_pDisplay = NULL;
    return true;
}

void X11wrapper::Assign_Display(Display * pDisplay)
{
    if (v_pDisplay && pDisplay)
    {
        _LOG_WRN("will overwrite the display");
    }
    v_pDisplay = pDisplay;
    _LOG_NFO("pDisplay==%p", GetDisplay());
    v_bIsAssigned_Display = (GetDisplay() != NULL);
    if (g_bXSynchronize_All)
        XSynchronize(GetDisplay(), true);
    if (g_bXAfterFunction)
        AfterFunction_Set();
}

Display * X11wrapper::Display_Open()
{
    if (GetDisplay() != NULL)
    {
        _LOG_WRN("pDisplay==%p", GetDisplay());
        return GetDisplay();
    }
    v_pDisplay = XOpenDisplay(NULL);
    if (GetDisplay() == NULL)
        _LOG_ERR("pDisplay==%p", GetDisplay());
    else
        _LOG_NFO("pDisplay==%p", GetDisplay());
    return GetDisplay();
}

bool X11wrapper::Display_Close()
{
    if (GetDisplay() == NULL)
    {
        _LOG_ERR("pDisplay==%p", GetDisplay());
        return false;
    }
    if (v_bIsAssigned_Display)
    {
        _LOG_NFO("pDisplay==%p was assigned, not closing", GetDisplay());
        return true;
    }
    _LOG_NFO("closing pDisplay==%p", GetDisplay());
    return XCloseDisplay(GetDisplay());
}

void X11wrapper::Assign_MainWindow(Window window)
{
    if (v_oMainWindow && window)
    {
        _LOG_WRN("will overwrite main window id");
    }
    _LOG_NFO("window==%d", window);
    v_oMainWindow = window;
}

Window X11wrapper::GetMainWindow()
{
    return v_oMainWindow;
}

Display * X11wrapper::GetDisplay()
{
    return v_pDisplay;
}

int X11wrapper::GetScreen()
{
    return XDefaultScreen(GetDisplay());
}

string X11wrapper::GetDisplayString()
{
    return XDisplayString(GetDisplay());
}

int X11wrapper::GetDisplayWidth()
{
    return XDisplayWidth(GetDisplay(), GetScreen());
}

int X11wrapper::GetDisplayHeight()
{
    return XDisplayHeight(GetDisplay(), GetScreen());
}

bool X11wrapper::GetDisplaySize(int &nWidth, int &nHeight)
{
    nWidth = GetDisplayWidth();
    nHeight = GetDisplayHeight();
    return true;
}

Window X11wrapper::Window_GetRoot()
{
    return XDefaultRootWindow(GetDisplay());
}

inline bool X11wrapper::IsReturnCodeOk(int code)
{
    return ( (code == 0) || (code == 1) );
}

string X11wrapper::GetErrorText(Display * pDisplay, int error_code)
{
    if (error_code == 0)
        return "";
    static const int size_buffer = 10000;
    char buffer[size_buffer];
    buffer[0] = '\0';
    buffer[0] = 0;
    XGetErrorText(pDisplay, error_code, buffer, size_buffer);
    return buffer;
}

int X11wrapper::ErrorHandler_Grabber(Display *pDisplay, XErrorEvent *pXErrorEvent)
{
    static const char s_header[] = "Xlib: ErrorHandler_Grabber()";
    _LOG_ERR("%s", s_header);
    cerr << "\n" << s_header << "\n";
    cerr << "  type : " << pXErrorEvent->type << "\n";
    cerr << "  display : " << pXErrorEvent->display << "\n";
    cerr << "  serial : " << pXErrorEvent->serial << "\n";
    cerr << "  error_code : " << pXErrorEvent->error_code << "\n";
    cerr << "  request_code : " << pXErrorEvent->request_code << "\n";
    cerr << "  minor_code : " << pXErrorEvent->minor_code << "\n";
    cerr << "  resourceid : " << pXErrorEvent->resourceid << "\n";
    cerr << "ErrorText : " << GetErrorText(pDisplay, pXErrorEvent->error_code) << "\n";
    cerr << "\n";
    return 0;
}

bool X11wrapper::ErrorHandler_Set()
{
    _LOG_NFO();
    if (v_bIsChanged_XErrorHandler)
    {
        _LOG_ERR("already changed");
        return false;
    }
    v_pOld_XErrorHandler = XSetErrorHandler(X11wrapper::ErrorHandler_Grabber);
    _LOG_NFO("pOld_XErrorHandler==%p", v_pOld_XErrorHandler);
    v_bIsChanged_XErrorHandler = true;
    return true;
}

bool X11wrapper::ErrorHandler_Restore()
{
    _LOG_NFO("pOld_XErrorHandler==%p", v_pOld_XErrorHandler);
    if (! v_bIsChanged_XErrorHandler)
    {
        _LOG_WRN("nothing to restore");
        return true;
    }
    XSetErrorHandler(v_pOld_XErrorHandler);
    v_bIsChanged_XErrorHandler = false;
    return true;
}

void X11wrapper::Sync()
{
    X11_Sync(GetDisplay());
}

void X11wrapper::Lock()
{
    X11_Lock(GetDisplay());
}

void X11wrapper::Unlock()
{
    X11_Unlock(GetDisplay());
}

Window X11wrapper::Window_Create(int nPosX, int nPosY, unsigned int nWidth, unsigned int nHeight, Window parent_window/*=None*/)
{
    if (parent_window == None)
    {
        _LOG_NFO("changing the parent window to root window");
        parent_window = Window_GetRoot();
    }
    _LOG_NFO("pos==(%d, %d, %d, %d), parent_window==%d", nPosX, nPosY, nWidth, nHeight, parent_window);
    Window window = 0;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        window = XCreateSimpleWindow(
            GetDisplay(),
            parent_window,
            nPosX, nPosY, nWidth, nHeight,
            0, // BorderWidth,
            0, // BorderColor
            0 // BgColor
            );
        _COND_XERROR_LOG_BREAK(window != 0);
    } while (0);
    return window;
}

Window X11wrapper::Window_Create_Show(int nPosX, int nPosY, unsigned int nWidth, unsigned int nHeight, Window parent_window/*=None*/)
{
    _LOG_NFO("pos==(%d, %d, %d, %d), parent_window==%d", nPosX, nPosY, nWidth, nHeight, parent_window);
    Window window = 0;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        window = Window_Create(nPosX, nPosY, nWidth, nHeight, parent_window);
        _COND_XERROR_LOG_BREAK(window != 0);
        if (! Window_Show(window))
            _LOG_XERROR_BREAK("Window_Show");
        if (! Window_MoveResize(window, nPosX, nPosY, nWidth, nHeight))
            _LOG_XERROR_BREAK("Window_MoveResize");
    } while (0);
    return window;
}

bool X11wrapper::Window_Destroy(Window window)
{
    _LOG_NFO("window==%d", window);
    int code = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        code = XDestroyWindow(GetDisplay(), window);
        _COND_XERROR_LOG_BREAK(code);
    } while (0);
    return IsReturnCodeOk(code);
}

bool X11wrapper::Window_Show(Window window, bool bShow/*=true*/)
{
    _LOG_NFO("window==%d, bShow==%d", window, bShow);
    int code = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        if (bShow)
        {
            // We want to get MapNotify events
            XSelectInput(GetDisplay(), window, StructureNotifyMask);
            // show here
            code = XMapWindow(GetDisplay(), window);
            _COND_XERROR_LOG_BREAK(code);
            // Wait for the MapNotify event
            for(;;)
            {
                XEvent xevent;
                XNextEvent(GetDisplay(), &xevent);
                if (xevent.type == MapNotify)
                    break;
            }
        }
        else
        {
            code = XUnmapWindow(GetDisplay(), window);
            _COND_XERROR_LOG_BREAK(code);
        }
    } while (0);
    return IsReturnCodeOk(code);
}

bool X11wrapper::Window_MoveResize(Window window, int nPosX, int nPosY, unsigned int nWidth, unsigned int nHeight)
{
    _LOG_NFO("window==%d, pos==(%d, %d, %d, %d)", window, nPosX, nPosY, nWidth, nHeight);
    int code = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        code = XMoveResizeWindow(GetDisplay(), window, nPosX, nPosY, nWidth, nHeight);
        _COND_XERROR_LOG_BREAK(code);
    } while (0);
    return IsReturnCodeOk(code);
}

bool X11wrapper::Window_ClassName(Window window, const char *s_class, const char *s_name)
{
    _LOG_NFO("window==%d, s_class=='%s', s_name=='%s')", window, s_class, s_name);
    int code = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        XClassHint classHint;
        classHint.res_name = const_cast<char *>(s_name);
        classHint.res_class = const_cast<char *>(s_class);
        code = XSetClassHint(GetDisplay(), window, &classHint);
        _COND_XERROR_LOG_BREAK(code);
    } while (0);
    return IsReturnCodeOk(code);
}

bool X11wrapper::Window_Name(Window window, const char *s_name)
{
    _LOG_NFO("window==%d, s_name==%s", window, s_name);
    int code = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        code = XStoreName(GetDisplay(), window, s_name);
        _COND_XERROR_LOG_BREAK(code);
    } while (0);
    return IsReturnCodeOk(code);
}

bool X11wrapper::Window_Raise(Window window)
{
    _LOG_NFO("window==%d", window);
    int code = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        code = XRaiseWindow(GetDisplay(), window);
        _COND_XERROR_LOG_BREAK(code);
    } while (0);
    return IsReturnCodeOk(code);
}

bool X11wrapper::Window_Lower(Window window)
{
    _LOG_NFO("window==%d", window);
    int code = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        code = XLowerWindow(GetDisplay(), window);
        _COND_XERROR_LOG_BREAK(code);
    } while (0);
    return IsReturnCodeOk(code);
}

bool X11wrapper::Window_SetOpacity_Helper(Window window, unsigned long nOpacity)
{
    _LOG_NFO("window==%d, nOpacity==0x%lx", window, nOpacity);
    if (nOpacity == 0)
        _LOG_WRN("Opacity value should not be 0, using it anyway, you have been warned");
    int code = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        Display *pDisplay = GetDisplay();
        // create property if does not exist
        Atom atom_property = XInternAtom(pDisplay, "_NET_WM_WINDOW_OPACITY", False);
        if (atom_property == None)
            _LOG_XERROR_BREAK("cannot use opacity property");
        if (nOpacity == WINDOW_OPAQUE)
        {
            _LOG_NFO("deleting opacity property");
            code = XDeleteProperty(pDisplay, window, atom_property);
            _COND_XERROR_LOG_BREAK(code);
        }
        else
        {
            //_LOG_NFO("changing opacity property");
            code = XChangeProperty(pDisplay, window, atom_property,
                                   XA_CARDINAL, 32, PropModeReplace,
                                   (unsigned char *) &nOpacity, 1L
                                   );
            _COND_XERROR_LOG_BREAK(code);
        }
    } while (0);
    return IsReturnCodeOk(code);
}

bool X11wrapper::Window_SetOpacity(Window window, unsigned long nOpacity)
{
    Window root_window = Window_GetRoot();
    _LOG_NFO("window==%d, root_window==%d, nOpacity==0x%lx", window, root_window, nOpacity);
    bool bResult = true;
    while ( (window) && (window != root_window) )
    {
        bResult = Window_SetOpacity_Helper(window, nOpacity);
        if (! bResult)
            _LOG_XERROR_BREAK("stopping tree-walk");
        window = Window_GetParent(window);
    }
    return bResult;
}

Window X11wrapper::Window_GetParent(Window window)
{
    //_LOG_NFO("window==%d", window);
    Window root_return = 0;
    Window parent_return = 0;
    Window *children_return = NULL;
    unsigned int nchildren_return = 0;
    int code = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        code = XQueryTree(
            GetDisplay(), window,
            &root_return, &parent_return, &children_return, &nchildren_return
            );
        //_LOG_NFO("window==%d, root_return==%d, parent_return==%d, children_return==%p, nchildren_return==%d", window, root_return, parent_return, children_return, nchildren_return);
        if (code == 0)
            _LOG_XERROR_BREAK("cannot read window tree");
    } while (0);
    // cleanup
    if (children_return)
        XFree(children_return);
    if (! IsReturnCodeOk(code))
        return 0;
    _LOG_NFO("window==%d, parent==%d", window, parent_return);
    return parent_return;
}

bool X11wrapper::Keyboard_Grab(Window window_grab)
{
    _LOG_NFO("window_grab==%d", window_grab);
    int code = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        code = XGrabKeyboard(
            GetDisplay(),
            window_grab, // window_grab
            false, // owner_events
            GrabModeAsync, // pointer_mode
            GrabModeAsync, // keyboard_mode
            CurrentTime // time
            );
        _COND_XERROR_LOG_BREAK(code);
    } while (0);
    return IsReturnCodeOk(code);
}

bool X11wrapper::Keyboard_Ungrab()
{
    _LOG_NFO();
    int code = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        code = XUngrabKeyboard(GetDisplay(), CurrentTime);
        _COND_XERROR_LOG_BREAK(code);
    } while (0);
    return IsReturnCodeOk(code);
}

bool X11wrapper::Keyboard_SetAutoRepeat(bool bOn)
{
    _LOG_NFO("bOn==%d", bOn);
    int code = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        if (bOn)
            code = XAutoRepeatOn(GetDisplay());
        else
            code = XAutoRepeatOff(GetDisplay());
        _COND_XERROR_LOG_BREAK(code);
    } while (0);
    return IsReturnCodeOk(code);
}

int X11wrapper::Mouse_Grab_Helper(Window window_grab, Window window_confine_to)
{
    int code = XGrabPointer(
        GetDisplay(),
        window_grab, // window_grab
        false, // owner_events
        ButtonPressMask | ButtonReleaseMask | ButtonMotionMask | EnterWindowMask | LeaveWindowMask | PointerMotionMask | PointerMotionHintMask, // event_mask
        GrabModeAsync, // pointer_mode
        GrabModeAsync, // keyboard_mode
        window_confine_to, // confine_to_window
        None, // cursor
        CurrentTime // time
        );
    return code;
}

bool X11wrapper::Mouse_Grab(Window window_grab, Window window_confine_to/*=None*/)
{
    _LOG_NFO("window_grab==%d, window_confine_to==%d", window_grab, window_confine_to);
    int code = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        code = Mouse_Grab_Helper(window_grab, window_confine_to);
        _COND_XERROR_LOG_BREAK(code);
    } while (0);
    return IsReturnCodeOk(code);
}

int X11wrapper::Mouse_Ungrab_Helper()
{
    int code = XUngrabPointer(GetDisplay(), CurrentTime);
    return code;
}

bool X11wrapper::Mouse_Ungrab()
{
    _LOG_NFO();
    int code = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        code = Mouse_Ungrab_Helper();
        _COND_XERROR_LOG_BREAK(code);
    } while (0);
    return IsReturnCodeOk(code);
}

bool X11wrapper::Mouse_ShowStandardCursor(Window window)
{
    return Mouse_SetCursor_Font(window, XC_left_ptr);
}

bool X11wrapper::Mouse_HideCursor(Window window)
{
    _LOG_NFO("window==%d", window);
    static const char dataNoCursor[] = { 0x00 };
    int code = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        Pixmap pixmap = XCreateBitmapFromData(GetDisplay(), window, dataNoCursor, 1, 1);
        if (pixmap == None)
            _LOG_XERROR_BREAK("cannot create the pixmap");
        XColor color;
        memset(&color, 0, sizeof(color));
        Cursor cursor = XCreatePixmapCursor(GetDisplay(), pixmap, pixmap, &color, &color, 0, 0);
        if (! cursor)
            _LOG_XERROR_BREAK("Cannot create pixmap cursor");
        code = XDefineCursor(GetDisplay(), window, cursor);
        _COND_XERROR_LOG_BREAK(code);
    } while (0);
    return IsReturnCodeOk(code);
}

bool X11wrapper::Mouse_SetCursor_Inherit(Window window)
{
    _LOG_NFO("window==%d", window);
    int code = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        code = XUndefineCursor(GetDisplay(), window);
        _COND_XERROR_LOG_BREAK(code);
    } while (0);
    return IsReturnCodeOk(code);
}

bool X11wrapper::Mouse_SetCursor_Font(Window window, int nShape)
{
    _LOG_NFO("window==%d, nShape==%d", window, nShape);
    // default cursor case
    if (nShape < 0)
    {
        return Mouse_SetCursor_Inherit(window);
    }
    int code = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        // create the cursor
        Cursor cursor = XCreateFontCursor(GetDisplay(), nShape);
        if (! cursor)
            _LOG_XERROR_BREAK("cannot create the font cursor");
        // change the cursor
        code = XDefineCursor(GetDisplay(), window, cursor);
        _COND_XERROR_LOG_BREAK(code);
    } while (0);
    return IsReturnCodeOk(code);
}

bool X11wrapper::Mouse_SetCursor_Image(Window window, const string &sPath, const string &sPathMask)
{
    _LOG_NFO("window==%d, sPath=='%s', sPathMask=='%s'", window, sPath.c_str(), sPathMask.c_str());
    Pixmap pixmap = None;
    Pixmap pixmap_mask = None;
    int code = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        // read pixmap
        unsigned int width_return = 0;
        unsigned int height_return = 0;
        int x_hot_return = 0;
        int y_hot_return = 0;
        if (! Pixmap_ReadFile(window, sPath.c_str(), pixmap, width_return, height_return, x_hot_return, y_hot_return))
            _LOG_XERROR_BREAK("cannot read x-bitmap");
        // read pixmap mask
        int x_hot_return_mask = 0;
        int y_hot_return_mask = 0;
        unsigned int width_return_mask = 0;
        unsigned int height_return_mask = 0;
        if (! Pixmap_ReadFile(window, sPathMask.c_str(), pixmap_mask, width_return_mask, height_return_mask, x_hot_return_mask, y_hot_return_mask))
            _LOG_XERROR_BREAK("cannot read x-bitmap mask");
        // check values
        if ( ( width_return != width_return_mask ) || ( height_return != height_return_mask ) )
            _LOG_XERROR_BREAK("size of the x-bitmap and his mask differ");
        // prepare extra data
        Colormap colormap = XDefaultColormap(GetDisplay(), GetScreen());
        XColor color_fg;
        color_fg.pixel = BlackPixel(GetDisplay(), GetScreen());
        code = XQueryColor(GetDisplay(), colormap, &color_fg);
        _COND_XERROR_LOG_BREAK(code);
        Sync();
        XColor color_bg;
        color_bg.pixel = WhitePixel(GetDisplay(), GetScreen());
        code = XQueryColor(GetDisplay(), colormap, &color_bg);
        _COND_XERROR_LOG_BREAK(code);
        Sync();
        // create the cursor
        Cursor cursor = XCreatePixmapCursor(GetDisplay(), pixmap, pixmap_mask, &color_fg, &color_bg, x_hot_return, y_hot_return);
        if (! cursor)
            _LOG_XERROR_BREAK("Cannot create pixmap cursor");
        // change the cursor
        code = XDefineCursor(GetDisplay(), window, cursor);
        _COND_XERROR_LOG_BREAK(code);
    } while (0);
    // cleanup
    if (pixmap != None)
        XFreePixmap(GetDisplay(), pixmap);
    if (pixmap_mask != None)
        XFreePixmap(GetDisplay(), pixmap_mask);
    return IsReturnCodeOk(code);
}

bool X11wrapper::Mouse_QueryMaxCursorSize(Window window, unsigned int &width_return, unsigned int &height_return)
{
    unsigned int width_max = GetDisplayWidth();
    unsigned int height_max = GetDisplayWidth();
    bool bResult = XQueryBestCursor(GetDisplay(), window, width_max, height_max, &width_return, &height_return);
    _LOG_NFO("window==%d, width_return==%d, height_return==%d", window, width_return, height_return);
    return bResult;
}

bool X11wrapper::Mouse_SetSpeed(int accel_numerator/*=-1*/, int accel_denominator/*=-1*/, int threshold/*=-1*/)
{
    _LOG_NFO("accel_numerator==%d, accel_denominator==%d, threshold==%d", accel_numerator, accel_denominator, threshold);
    int code = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        code = XChangePointerControl(GetDisplay(), true, true, accel_numerator, accel_denominator, threshold);
        _COND_XERROR_LOG_BREAK(code);
    } while (0);
    return IsReturnCodeOk(code);
}

bool X11wrapper::Mouse_GetSpeed(int &accel_numerator_return, int &accel_denominator_return, int &threshold_return)
{
    int code = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        code = XGetPointerControl(GetDisplay(), &accel_numerator_return, &accel_denominator_return, &threshold_return);
        _COND_XERROR_LOG_BREAK(code);
        _LOG_NFO("accel_numerator==%d, accel_denominator==%d, threshold==%d", accel_numerator_return, accel_denominator_return, threshold_return);
    } while (0);
    return IsReturnCodeOk(code);
}

bool X11wrapper::Mouse_SetPosition(int nPosX, int nPosY)
{
    _LOG_NFO("pos==(%d, %d)", nPosX, nPosY);
    Window window = Window_GetRoot();
    int code = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        code = XWarpPointer(GetDisplay(), window, window, 0, 0, 0, 0, nPosX, nPosY);
        _COND_XERROR_LOG_BREAK(code);
    } while (0);
    return IsReturnCodeOk(code);
}

bool X11wrapper::Mouse_GetPosition(int &nPosX, int &nPosY, bool bRelative/*=false*/)
{
    //_LOG_NFO("previous pos==(%d, %d)", nPosX, nPosY);
    Window window = Window_GetRoot();
    int code = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        if (bRelative || !bRelative) break;
        if (bRelative)
        {
#ifdef USE_XDGA
            XDGA_Mouse_GetPosition(nPosX, nPosY);
            _LOG_NFO("nPosX==%d, nPosY==%d", nPosX, nPosY);
#endif // USE_XDGA
        }
        else
        {
            Window root_return;
            Window child_return;
            int root_x_return;
            int root_y_return;
            int win_x_return;
            int win_y_return;
            unsigned int mask_return;
            code = XQueryPointer(
                GetDisplay(),
                window,
                &root_return,
                &child_return,
                &root_x_return,
                &root_y_return,
                &win_x_return,
                &win_y_return,
                &mask_return
                );
            _COND_XERROR_LOG_BREAK(code);
            _LOG_NFO(
                "root_return==%d, child_return==%d, root_x_return==%d, root_y_return==%d, win_x_return==%d, win_y_return==%d, mask_return==%d",
                root_return, child_return, root_x_return, root_y_return, win_x_return, win_y_return, mask_return
                );
            nPosX = root_x_return;
            nPosY = root_y_return;
        }
    } while (0);
    return IsReturnCodeOk(code);
}

bool X11wrapper::Mouse_Constrain(int nPosX, int nPosY, unsigned int nWidth, unsigned int nHeight, Window window_grab/*=None*/)
{
    if (window_grab == None)
    {
        _LOG_NFO("changing the grab window to root window");
        window_grab = Window_GetRoot();
    }
    _LOG_NFO("pos==(%d, %d, %d, %d), window_grab==%d", nPosX, nPosY, nWidth, nHeight, window_grab);

    // hide window case
    if ( (nWidth == 0) && (nHeight == 0) )
    {
        return Mouse_Constrain_Release();
    }

    // show window case
    Window &window = v_window_Mouse_Constrain;
    int code = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        // check values
        // Size (not including the border) must be nonzero (or a Value error results)
        // Note: The Xlib manual doesn't mention this restriction ?
        if ( (nWidth <= 0) || (nHeight <= 0) )
            _LOG_XERROR_BREAK("bad arguments, size must be greater than 0");
        if (v_bIsActive_Mouse_Constrain)
            _LOG_XERROR_BREAK("window is already created");
        if (window > 0)
        {
            _LOG_NFO("closing previous window");
            if (! Window_Destroy(window))
                _LOG_XERROR_BREAK("cannot destroy window %d", window);
        }
        window = Window_Create_Show(nPosX, nPosY, nWidth, nHeight, 0);
        if (window <= 0)
            _LOG_XERROR_BREAK("cannot create window");
        // (! Mouse_Grab(window_grab, window)) // needs double-checking
        code = Mouse_Grab_Helper(window_grab, window);
        Sync();
        if (! IsReturnCodeOk(code))
        {
            _LOG_WRN("creating second window");
            Window_Destroy(window);
            window = Window_Create_Show(nPosX, nPosY, nWidth, nHeight, 0);
            if (window <= 0)
                _LOG_XERROR_BREAK("cannot create second window");
            code = Mouse_Grab_Helper(window_grab, window);
            Sync();
            if (! IsReturnCodeOk(code))
            {
                _LOG_ERR("cannot grab pointer, closing window %d", window);
                Window_Destroy(window);
                window = 0;
                break;
            }
        }
        if (! Window_ClassName(window, "constrain_mouse", "constrain_mouse"))
            _LOG_XERROR_BREAK("cannot set window class");
        if (! Window_Name(window, "constrain_mouse"))
            _LOG_XERROR_BREAK("cannot set window name");
        if (! Window_SetOpacity(window, 1))
            _LOG_WRN("cannot set opacity");
        if (! Window_Lower(window))
            _LOG_WRN("cannot lower window");
        // done
        code = 0;
        v_bIsActive_Mouse_Constrain = true;
    } while (0);
    previous_mouse_constrain.window_grab = window_grab;
    previous_mouse_constrain.nPosX = nPosX;
    previous_mouse_constrain.nPosY = nPosY;
    previous_mouse_constrain.nWidth = nWidth;
    previous_mouse_constrain.nHeight = nHeight;
    return IsReturnCodeOk(code);
}

bool X11wrapper::Mouse_Constrain_Release()
{
    _LOG_NFO();
    Window &window = v_window_Mouse_Constrain;
    int code = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        if (! v_bIsActive_Mouse_Constrain)
        {
            _LOG_WRN("constrain not active");
            code = 0;
            break;
        }
        if (! Mouse_Ungrab())
            _LOG_WRN("Mouse_Ungrab");
        if (! Window_Destroy(window))
            _LOG_WRN("Window_Destroy");
        // done
        code = 0;
        window = 0;
        v_bIsActive_Mouse_Constrain = false;
    } while (0);
    // not deleting the previous info
    return IsReturnCodeOk(code);
}

bool X11wrapper::Mouse_Constrain_ReactivateIfActive()
{
    if (! v_bIsActive_Mouse_Constrain)
    {
        _LOG_NFO("constrain is not active, nothing to do");
        return true;
    }
    _LOG_NFO("constrain is active, reactivating");
    Mouse_Constrain_Release();
    Mouse_Constrain(previous_mouse_constrain.nPosX, previous_mouse_constrain.nPosY, previous_mouse_constrain.nWidth, previous_mouse_constrain.nHeight, previous_mouse_constrain.window_grab);
    return v_bIsActive_Mouse_Constrain;
}

bool X11wrapper::Mouse_IsConstrainActive()
{
    return v_bIsActive_Mouse_Constrain;
}

Window X11wrapper::Mouse_Constrain_GetWindow()
{
    return v_window_Mouse_Constrain;
}

// all from : /usr/include/X11/cursorfont.h
const X11wrapper::string_int X11wrapper::v_aStdCursors_NameValue[] =
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
const int X11wrapper::v_aStdCursors_Count = ( sizeof(v_aStdCursors_NameValue) / sizeof(v_aStdCursors_NameValue[0]) );

int X11wrapper::StdCursor_GetValueByName(const std::string &sName)
{
    for (int i=0; i<v_aStdCursors_Count; ++i)
    {
        if (v_aStdCursors_NameValue[i].sName == sName)
            return v_aStdCursors_NameValue[i].nValue;
    }
    return -1;
}

Pixmap X11wrapper::Pixmap_Create(Window window, unsigned int width, unsigned int height, unsigned int depth)
{
    _LOG_NFO("window==%d, width==%d, height==%d, depth==%d", window, width, height, depth);
    Pixmap pixmap = XCreatePixmap(GetDisplay(), window, width, height, depth);
    if (pixmap == 0)
        _LOG_ERR("cannot create pixmap: window==%d, width==%d, height==%d, depth==%d", window, width, height, depth);
    else
        _LOG_NFO("window==%d, width==%d, height==%d, depth==%d => pixmap==%d", window, width, height, depth, pixmap);
    return pixmap;
}

bool X11wrapper::Pixmap_Delete(Pixmap &pixmap)
{
    _LOG_NFO("pixmap==%d", pixmap);
    X11_Locker x11_locker(GetDisplay());
    int code = -1;
    do
    {
        code = XFreePixmap(GetDisplay(), pixmap);
        _COND_XERROR_LOG_BREAK(code);
    } while (0);
    return IsReturnCodeOk(code);
}

bool X11wrapper::Pixmap_ReadFile(Window window, const string &sPath, Pixmap &pixmap_return, unsigned int &width_return, unsigned int &height_return, int &x_hot_return, int &y_hot_return)
{
    _LOG_NFO("window==%d, sPath=='%s'", window, sPath.c_str());
    int code = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        // read x-bitmap from file
        code = XReadBitmapFile(GetDisplay(), window, sPath.c_str(), &width_return, &height_return, &pixmap_return, &x_hot_return, &y_hot_return);
        _COND_XERROR_LOG_BREAK(code);
        if (x_hot_return < 0)
            x_hot_return = 0;
        if (y_hot_return < 0)
            y_hot_return = 0;
    } while (0);
    return IsReturnCodeOk(code);
}

bool X11wrapper::Object_GetGeometry(Drawable drawable, int *x_return, int *y_return, unsigned int *width_return, unsigned int *height_return)
{
    _LOG_NFO("Drawable==%d, x_return==%p, y_return==%p, width_return==%p, height_return==%p",
             drawable, x_return, y_return, width_return, height_return
             );
    int code = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        Window root_ret = 0;
        int x_ret = 0;
        int y_ret = 0;
        unsigned int width_ret = 0;
        unsigned int height_ret = 0;
        unsigned int border_width_ret = 0;
        unsigned int depth_ret = 0;
        code = XGetGeometry(
            GetDisplay(), drawable,
            &root_ret,
            &x_ret, &y_ret, &width_ret, &height_ret,
            &border_width_ret,
            &depth_ret
            );
        if (code == 0)
            _LOG_XERROR_BREAK("cannot read window geometry");
        if (x_return)
        {
            *x_return = x_ret;
            _LOG_NFO("x_return->%d", *x_return);
        }
        if (y_return)
        {
            *y_return = y_ret;
            _LOG_NFO("y_return->%d", *y_return);
        }
        if (width_return)
        {
            *width_return = width_ret;
            _LOG_NFO("width_return->%d", *width_return);
        }
        if (height_return)
        {
            *height_return = height_ret;
            _LOG_NFO("height_return->%d", *height_return);
        }
    } while (0);
    return IsReturnCodeOk(code);
}

bool X11wrapper::Extension_Shape_IsAvailable()
{
    if (! v_bExtension_Shape_IsInitialized)
        Extension_Shape_Initialize();
    return v_bExtension_Shape_IsAvailable;
}

bool X11wrapper::Extension_Shape_GetVersion_Major()
{
    if (! v_bExtension_Shape_IsInitialized)
        Extension_Shape_Initialize();
    return v_nExtension_Shape_Version_Major;
}

bool X11wrapper::Extension_Shape_GetVersion_Minor()
{
    if (! v_bExtension_Shape_IsInitialized)
        Extension_Shape_Initialize();
    return v_nExtension_Shape_Version_Minor;
}

bool X11wrapper::Shape_Context_Enter(Window window, unsigned int width, unsigned int height, Pixmap &bitmap_mask, GC &gc, Display * &pDisplay)
{
    _LOG_NFO("window==%d, width==%d, height==%d", window, width, height);
    bool bResult = false;
    bool b_gc_created = false;
    int code = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        if (! Extension_Shape_IsAvailable())
            _LOG_XERROR_BREAK("X11 SHAPE extension: not available");
        pDisplay = GetDisplay();
        bitmap_mask = XCreatePixmap(pDisplay, window, width, height, 1);
        if (bitmap_mask == None)
            _LOG_XERROR_BREAK("cannot create bitmap for mask");
        gc = XCreateGC(pDisplay, bitmap_mask, 0, NULL);
        b_gc_created = true;
        // clean background
        code = XFillRectangle(pDisplay, bitmap_mask, gc, 0, 0, width, height);
        _COND_XERROR_LOG_BREAK(code);
        code = XSetForeground(pDisplay, gc, 1);
        _COND_XERROR_LOG_BREAK(code);
        // done
        bResult = true;
    } while (0);
    // cleanup
    if (! bResult)
    {
        if (bitmap_mask != None)
            XFreePixmap(GetDisplay(), bitmap_mask);
        if (b_gc_created)
            XFreeGC(GetDisplay(), gc);
    }
    if (bResult)
    {
        _LOG_NFO("window==%d, width==%d, height==%d => bitmap_mask==%d, gc==%d, pDisplay==%p", window, width, height, bitmap_mask, gc, pDisplay);
    }
    return (IsReturnCodeOk(code) && bResult);
}

bool X11wrapper::Shape_Context_Leave(Window window, Pixmap &bitmap_mask, GC &gc, Display * &pDisplay)
{
    _LOG_NFO("window==%d, bitmap_mask==%d, gc==%d, pDisplay==%p", window, bitmap_mask, gc, pDisplay);
    bool bResult = Shape_Window_Apply(window, bitmap_mask);
    if (! bResult)
        _LOG_ERR("cannot apply shape");
    int code = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        code = XFreeGC(pDisplay, gc);
        if (! IsReturnCodeOk(code))
            _LOG_WRN("XFreeGC() retCode==%d", code);
        code = XFreePixmap(GetDisplay(), bitmap_mask);
        if (! IsReturnCodeOk(code))
            _LOG_WRN("XFreePixmap() retCode==%d", code);
        code = 0;
    } while (0);
    // cleanup
    return (IsReturnCodeOk(code) && bResult);
}

bool X11wrapper::Shape_Window_Apply(Window window, Pixmap &pixmap)
{
    _LOG_NFO("window==%d, pixmap==%d", window, pixmap);
    int code = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        if (! Extension_Shape_IsAvailable())
            _LOG_XERROR_BREAK("X11 SHAPE extension: not available");
        XShapeCombineMask(GetDisplay(), window, ShapeBounding, 0, 0, pixmap, ShapeSet);
        code = 0;
    } while (0);
    return IsReturnCodeOk(code);
}

bool X11wrapper::Shape_Window_Reset(Window window)
{
    _LOG_NFO("window==%d", window);
    int code = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        if (! Extension_Shape_IsAvailable())
            _LOG_XERROR_BREAK("X11 SHAPE extension: not available");
        XShapeCombineMask(GetDisplay(), window, ShapeBounding, 0, 0, None, ShapeSet);
    } while (0);
    return IsReturnCodeOk(code);
}

bool X11wrapper::Shape_Window_Hide(Window window)
{
    _LOG_NFO("window==%d", window);
    int code = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        if (! Extension_Shape_IsAvailable())
            _LOG_XERROR_BREAK("X11 SHAPE extension: not available");
        XRectangle aXRectangle[1];
        aXRectangle[0].x = 0;
        aXRectangle[0].y = 0;
        aXRectangle[0].width = 0;
        aXRectangle[0].height = 0;
        XShapeCombineRectangles(GetDisplay(), window, ShapeBounding, 0, 0, aXRectangle, 0, ShapeSet, Unsorted);
        code = 0;
    } while (0);
    return IsReturnCodeOk(code);
}

bool X11wrapper::Extension_Shape_Initialize()
{
    _LOG_NFO();
    int code = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        v_bExtension_Shape_IsInitialized = true;
        int major_opcode = 0;
        int first_event = 0;
        int first_error = 0;
        code = XQueryExtension(GetDisplay(),"SHAPE",&major_opcode,&first_event,&first_error);
        if (! code)
            _LOG_XERROR_BREAK("X11 SHAPE extension not available : major_opcode==%d, first_event==%d, first_error==%d",
                              major_opcode, first_event, first_error
                              );
        code = XShapeQueryVersion(GetDisplay(), &v_nExtension_Shape_Version_Major, &v_nExtension_Shape_Version_Minor);
        if (code == 0)
            _LOG_XERROR_BREAK("X11 SHAPE extension: not available");
        v_bExtension_Shape_IsAvailable = true;
        _LOG_NFO("X11 SHAPE extension: version %d.%d", v_nExtension_Shape_Version_Major, v_nExtension_Shape_Version_Minor);
    } while (0);
    return v_bExtension_Shape_IsAvailable;
}

Window X11wrapper::Debug_Window(bool bCreate_NotClose/*=true*/)
{
    _LOG_NFO("bCreate_NotClose==%d", bCreate_NotClose);
    static Window windowDebug = 0;
    if (bCreate_NotClose)
    {
        if (windowDebug == 0)
        {
            windowDebug = Window_Create_Show(400, 100, 400, 300);
            Window_ClassName(windowDebug, "Debug_Class", "Debug_Name");
            Window_Name(windowDebug, "Debug_Window");
            _LOG_NFO("windowDebug==%d", windowDebug);
        }
    }
    else
    {
        if (windowDebug != 0)
        {
            Window_Destroy(windowDebug);
            windowDebug = 0;
        }
    }
    return windowDebug;
}

bool X11wrapper::Debug_Shape_Context_Example(Window window, unsigned int width, unsigned int height)
{
    // remembered values
    Pixmap bitmap_mask = 0;
    GC gc;
    Display *pDisplay = NULL;
    // initialize
    bool bResult = Shape_Context_Enter(window, width, height, bitmap_mask, gc, pDisplay);
    if (! bResult)
    {
        _LOG_ERR("Shape_Context_Enter() retCode==%d, window==%d, width==%d, height==%d, bitmap_mask==%d, gc==%d, pDisplay==%p",
             bResult, window, width, height, bitmap_mask, gc, pDisplay);
        return false;
    }
    // creating the desired shape
    for (unsigned int x=0; x<width; ++x)
        for (unsigned int y=0; y<height; ++y)
        {
            if (x<y)
                XDrawPoint(pDisplay, bitmap_mask, gc, x, y);
        }
    // done
    bResult = Shape_Context_Leave(window, bitmap_mask, gc, pDisplay);
    if (! bResult)
    {
        _LOG_WRN("Shape_Context_Leave() retCode==%d, window==%d, width==%d, height==%d, bitmap_mask==%d, gc==%d, pDisplay==%p",
             bResult, window, width, height, bitmap_mask, gc, pDisplay);
    }
    return true;
}

int X11wrapper::AfterFunction_Grabber(Display *pDisplay)
{
    static const char s_header[] = "Xlib: AfterFunction_Grabber()";
    _LOG_WRN("%s : pDisplay==%p", s_header, pDisplay);
    return 0;
}

bool X11wrapper::AfterFunction_Set()
{
    _LOG_NFO();
    if (v_bIsChanged_X_AfterFunction)
    {
        _LOG_ERR("already changed");
        return false;
    }
    v_pOld_X_AfterFunction = XSetAfterFunction(GetDisplay(), X11wrapper::AfterFunction_Grabber);
    _LOG_NFO("pOld_X_AfterFunction==%p", v_pOld_X_AfterFunction);
    v_bIsChanged_X_AfterFunction = true;
    return true;
}

bool X11wrapper::AfterFunction_Restore()
{
    _LOG_NFO("pOld_X_AfterFunction==%p", v_pOld_X_AfterFunction);
    if (! v_bIsChanged_X_AfterFunction)
    {
        _LOG_WRN("nothing to restore");
        return true;
    }
    XSetAfterFunction(GetDisplay(), v_pOld_X_AfterFunction);
    v_bIsChanged_X_AfterFunction = false;
    return true;
}

bool X11wrapper::Shape_Window_Alpha(Window window, unsigned char nAlphaThreshold/*=128*/)
{
    _LOG_NFO("window==%d, nAlphaThreshold==%d", window, nAlphaThreshold);
    XImage *pXImage = NULL;
    int code = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        if (! Extension_Shape_IsAvailable())
            _LOG_XERROR_BREAK("X11 SHAPE extension: not available");
        pXImage = Window_GetImage(window);
        if (! pXImage)
            _LOG_XERROR_BREAK("GetImage");
        Pixmap pixmap = ConvertImageToPixmap(pXImage, window);
        XShapeCombineMask(GetDisplay(), window, ShapeBounding, 0, 0, pixmap, ShapeSet);
    } while (0);
    // cleanup
    if (pXImage)
        XDestroyImage(pXImage);
    return IsReturnCodeOk(code);
}

XImage * X11wrapper::Window_GetImage(Window window, bool bOnlyMask/*=false*/)
{
    // Note that bytes_per_line in concert with offset can be used to extract a subset of the image
    // The first byte of the first line of plane n must be located at the address
    // (data + (n * height * bytes_per_line))
    _LOG_NFO("window==%d", window);
    XImage *pXImage = NULL;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        int x = 0;
        int y = 0;
        unsigned int width = 0;
        unsigned int height = 0;
        if (! Window_GetPosition(window, x, y, width, height))
            _LOG_XERROR_BREAK("Window_GetImage() : GetPosition");
        if (bOnlyMask)
            pXImage = XGetImage(GetDisplay(), window, 0, 0, width, height, 1, XYPixmap);
        else
            pXImage = XGetImage(GetDisplay(), window, 0, 0, width, height, AllPlanes, ZPixmap);
        if (! pXImage)
            _LOG_XERROR_BREAK("Window_GetImage() : GetImage");
    } while (0);
    return pXImage;
}

bool X11wrapper::Window_GetPosition(Window window, int &nPosX, int &nPosY, unsigned int &nWidth, unsigned int &nHeight)
{
    _LOG_NFO("window==%d", window);
    int code = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        XWindowAttributes attr;
        code = XGetWindowAttributes(GetDisplay(), window, &attr);
        _COND_XERROR_LOG_BREAK(code);
        nPosX = attr.x;
        nPosY = attr.y;
        nWidth = attr.width;
        nHeight = attr.height;
    } while (0);
    return IsReturnCodeOk(code);
}

Pixmap X11wrapper::ConvertImageToPixmap(XImage *pXImage, Window window)
{
    _LOG_NFO("window==%d", window);
    Pixmap pixmap = None;
    //int code = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        GC gc = XDefaultGC(GetDisplay(), GetScreen());
        //Pixmap pixmap = XCreatePixmap(GetDisplay(), Window_GetRoot(), width, height, depth);
        //XPutImage(GetDisplay(), pixmap, gc, image, 0, 0, 0, 0, width, height);
        pixmap = XCreatePixmapFromBitmapData(GetDisplay(), window, pXImage->data, pXImage->width, pXImage->height, 0, 0, pXImage->depth);
        XFlushGC(GetDisplay(), gc);
    } while (0);
    return pixmap;
}

bool X11wrapper::Window_PerPixel_Transparency(Window window, unsigned long nAlphaValue/*=1*/)
{
    _LOG_NFO();
    int code = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        Display *pDisplay = GetDisplay();
        //int screen = GetScreen();
        //int blackColor = BlackPixel(pDisplay, screen);
        //int whiteColor = WhitePixel(pDisplay, screen);
        // draw something
        GC gc = XCreateGC(pDisplay, window, 0, NULL);
        XSetForeground(pDisplay, gc, nAlphaValue);
        XSetBackground(pDisplay, gc, (unsigned long)-nAlphaValue);
        for (int i=0; i<10; i++)
            XDrawLine(pDisplay, window, gc, 10, i+60, 180, i+20);
        XFlush(pDisplay);
    } while (0);
    return IsReturnCodeOk(code);
}
