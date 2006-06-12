//
// Author : Remus C.
//
// Changed by : ...
//

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "wrapper_x11.h"
#endif

#include "wrapper_x11.h"
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
  X11_Locker x11_locker(v_pDisplay);
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
    //_LOG_NFO("pDisplay==%p", v_pDisplay);
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
    //_LOG_NFO("pDisplay==%p", v_pDisplay);
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
    //_LOG_NFO("pDisplay==%p", v_pDisplay);
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
}

X11wrapper::~X11wrapper()
{
    _LOG_NFO();
    Clean_Exit();
}

bool X11wrapper::IsInitialized()
{
    _LOG_NFO();
    return (v_pDisplay != NULL);
}

bool X11wrapper::Clean_Exit()
{
    if (v_pDisplay == NULL)
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
    _LOG_NFO("pDisplay==%p", v_pDisplay);
    v_bIsAssigned_Display = (v_pDisplay != NULL);
    if (g_bXSynchronize_All)
        XSynchronize(v_pDisplay, true);
    if (g_bXAfterFunction)
        AfterFunction_Set();
}

Display * X11wrapper::Display_Open()
{
    if (v_pDisplay != NULL)
    {
        _LOG_WRN("pDisplay==%p", v_pDisplay);
        return v_pDisplay;
    }
    v_pDisplay = XOpenDisplay(NULL);
    if (v_pDisplay == NULL)
        _LOG_ERR("pDisplay==%p", v_pDisplay);
    else
        _LOG_NFO("pDisplay==%p", v_pDisplay);
    return v_pDisplay;
}

bool X11wrapper::Display_Close()
{
    if (v_pDisplay == NULL)
    {
        _LOG_ERR("pDisplay==%p", v_pDisplay);
        return false;
    }
    if (v_bIsAssigned_Display)
    {
        _LOG_NFO("pDisplay==%p was assigned, not closing", v_pDisplay);
        return true;
    }
    _LOG_NFO("closing pDisplay==%p", v_pDisplay);
    return XCloseDisplay(v_pDisplay);
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
    //if (v_pDisplay == NULL)
    //{
    //    _LOG_NFO("open a new one");
    //    Display_Open();
    //}
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
    return XDisplayWidth(GetDisplay(), XDefaultScreen(GetDisplay()));
}

int X11wrapper::GetDisplayHeight()
{
    return XDisplayHeight(GetDisplay(), XDefaultScreen(GetDisplay()));
}

bool X11wrapper::GetDisplaySize(int &nWidth, int &nHeight)
{
    Display *pDisplay = GetDisplay();
    int nScreen = XDefaultScreen(pDisplay);
    nWidth = XDisplayWidth(pDisplay, nScreen);
    nHeight = XDisplayHeight(pDisplay, nScreen);
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
    X11_Sync(v_pDisplay);
}

void X11wrapper::Lock()
{
    X11_Lock(v_pDisplay);
}

void X11wrapper::Unlock()
{
    X11_Unlock(v_pDisplay);
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
    X11_Locker x11_locker(v_pDisplay);
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
    X11_Locker x11_locker(v_pDisplay);
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
    X11_Locker x11_locker(v_pDisplay);
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
    X11_Locker x11_locker(v_pDisplay);
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
    X11_Locker x11_locker(v_pDisplay);
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
    X11_Locker x11_locker(v_pDisplay);
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
    X11_Locker x11_locker(v_pDisplay);
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
    X11_Locker x11_locker(v_pDisplay);
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
    X11_Locker x11_locker(v_pDisplay);
    do
    {
        code = XLowerWindow(GetDisplay(), window);
        _COND_XERROR_LOG_BREAK(code);
    } while (0);
    return IsReturnCodeOk(code);
}

bool X11wrapper::Keyboard_Grab(Window window_grab)
{
    _LOG_NFO("window_grab==%d", window_grab);
    int code = -1;
    X11_Locker x11_locker(v_pDisplay);
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
    X11_Locker x11_locker(v_pDisplay);
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
    X11_Locker x11_locker(v_pDisplay);
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

bool X11wrapper::Mouse_Grab(Window window_grab, Window window_confine_to/*=None*/)
{
    _LOG_NFO("window_grab==%d, window_confine_to==%d", window_grab, window_confine_to);
    int code = -1;
    X11_Locker x11_locker(v_pDisplay);
    do
    {
        code = XGrabPointer(
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
        _COND_XERROR_LOG_BREAK(code);
    } while (0);
    return IsReturnCodeOk(code);
}

bool X11wrapper::Mouse_Ungrab()
{
    _LOG_NFO();
    int code = -1;
    X11_Locker x11_locker(v_pDisplay);
    do
    {
        code = XUngrabPointer(GetDisplay(), CurrentTime);
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
    X11_Locker x11_locker(v_pDisplay);
    do
    {
        Pixmap pixmap = XCreateBitmapFromData( v_pDisplay, window, dataNoCursor, 1, 1 );
        if (pixmap == None)
            _LOG_XERROR_BREAK("cannot create the pixmap");
        XColor color;
        memset(&color, 0, sizeof(color));
        Cursor cursor = XCreatePixmapCursor(v_pDisplay, pixmap, pixmap, &color, &color, 0, 0);
        if (! cursor)
            _LOG_XERROR_BREAK("Cannot create pixmap cursor");
        code = XDefineCursor( v_pDisplay, window, cursor );
        _COND_XERROR_LOG_BREAK(code);
    } while (0);
    return IsReturnCodeOk(code);
}

bool X11wrapper::Mouse_SetCursor_Inherit(Window window)
{
    _LOG_NFO("window==%d", window);
    int code = -1;
    X11_Locker x11_locker(v_pDisplay);
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
    X11_Locker x11_locker(v_pDisplay);
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
    X11_Locker x11_locker(v_pDisplay);
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
        int nScreen = XDefaultScreen(GetDisplay());
        Colormap colormap = XDefaultColormap(GetDisplay(), nScreen);
        XColor color_fg;
        color_fg.pixel = BlackPixel(GetDisplay(), nScreen);
        code = XQueryColor(GetDisplay(), colormap, &color_fg);
        _COND_XERROR_LOG_BREAK(code);
        Sync();
        XColor color_bg;
        color_bg.pixel = WhitePixel(GetDisplay(), nScreen);
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
    int nScreen = XDefaultScreen(GetDisplay());
    unsigned int width_max = XDisplayWidth(GetDisplay(), nScreen);
    unsigned int height_max = XDisplayHeight(GetDisplay(), nScreen);
    bool bResult = XQueryBestCursor(GetDisplay(), window, width_max, height_max, &width_return, &height_return);
    _LOG_NFO("window==%d, width_return==%d, height_return==%d", window, width_return, height_return);
    return bResult;
}

bool X11wrapper::Mouse_SetSpeed(int accel_numerator/*=-1*/, int accel_denominator/*=-1*/, int threshold/*=-1*/)
{
    _LOG_NFO("accel_numerator==%d, accel_denominator==%d, threshold==%d", accel_numerator, accel_denominator, threshold);
    int code = -1;
    X11_Locker x11_locker(v_pDisplay);
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
    X11_Locker x11_locker(v_pDisplay);
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
    X11_Locker x11_locker(v_pDisplay);
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
    X11_Locker x11_locker(v_pDisplay);
    do
    {
        if (bRelative)
        {
            //XDGAMotionEvent xdgamotionevent;
            //XDGAEvent xdgaevent;
            //XDGASelectInput(GetDisplay(), GetScreen(), PointerMotionMask | PointerMotionHintMask);
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
                v_pDisplay,
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
    X11_Locker x11_locker(v_pDisplay);
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
        if (! Window_ClassName(window, "constrain_mouse", "constrain_mouse"))
            _LOG_XERROR_BREAK("cannot set window class");
        if (! Window_Name(window, "constrain_mouse"))
            _LOG_XERROR_BREAK("cannot set window name");
        if (! Window_Lower(window))
            _LOG_WRN("cannot lower window");
        if (! Mouse_Grab(window_grab, window))
        {
            _LOG_ERR("cannot grab pointer, closing window %d", window);
            Window_Destroy(window);
            window = 0;
            break;
        }
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
    X11_Locker x11_locker(v_pDisplay);
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

bool X11wrapper::Pixmap_ReadFile(Window window, const string &sPath, Pixmap &pixmap_return, unsigned int &width_return, unsigned int &height_return, int &x_hot_return, int &y_hot_return)
{
    _LOG_NFO("window==%d, sPath=='%s'", window, sPath.c_str());
    int code = -1;
    X11_Locker x11_locker(v_pDisplay);
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
    v_pOld_X_AfterFunction = XSetAfterFunction(v_pDisplay, X11wrapper::AfterFunction_Grabber);
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
    XSetAfterFunction(v_pDisplay, v_pOld_X_AfterFunction);
    v_bIsChanged_X_AfterFunction = false;
    return true;
}
