//
// Author : Remus C.
//
// Changed by : ...
//

// notes:
//        use Lock() and Unlock()
//        use a single instance
//        initialize it from the start

#ifndef _WRAPPER_X11_H_
#define _WRAPPER_X11_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wrapper_x11.h"
#endif

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/Xatom.h>

#include <string>
#include <iostream>
using namespace std;

// definition from in Xlib.h
// WARNING, this type not in Xlib spec
typedef int (*XErrorHandler) (Display*, XErrorEvent*);

// new type
typedef int (*X_AfterFunction) (Display*);

// constants
const unsigned long WINDOW_OPAQUE = 0xFFFFFFFF;

/// sync and lock functions
bool X11_Sync(Display *pDisplay); // call flush
bool X11_Lock(Display *pDisplay);
bool X11_Unlock(Display *pDisplay); // call sync

// small helper class to be used with X11 code
// locks in the constructor and unlocks in the destructor
// making it much more difficult to forget to unlock
// (which, in general, will lead to serious and difficult to debug problems)
class X11_Locker
{
public:
    X11_Locker(Display *pDisplay);
    ~X11_Locker();
protected:
    Display *v_pDisplay;
};

// small helper class to be used with X11 code
// locks in the constructor and unlocks in the destructor
// making it much more difficult to forget to unlock
// (which, in general, will lead to serious and difficult to debug problems)
// this one open a new display, and closes it in the destructor
class X11_Locker_NewDisplay
{
public:
    X11_Locker_NewDisplay();
    ~X11_Locker_NewDisplay();
    Display * GetDisplay();
protected:
    Display *v_pDisplay;
};

// X11 wrapper class
// if the display connection is assigned, it will not be closed at exit
// the main window can be kept here, but it is not handled
class X11wrapper
{
public:
    // allow to reuse an already opened display connection
    X11wrapper(Display *pDisplay=NULL);

    // if the display connection was reused, it is not closed
    ~X11wrapper();

    bool IsInitialized();

    bool Clean_Exit();

    /// display

    // reuse an already opened display connection
    void Assign_Display(Display *pDisplay);

    Display * Display_Open();
    bool Display_Close();

    /// X11 info

    Display * GetDisplay();
    int GetScreen();
    string GetDisplayString();
    int GetDisplayWidth();
    int GetDisplayHeight();
    bool GetDisplaySize(int &nWidth, int &nHeight);

    Window Window_GetRoot();

    /// main window info

    // reuse the already created main window
    void Assign_MainWindow(Window window);
    Window GetMainWindow();

    // reuse the already opened display for the main window
    void Assign_MainDisplay(Window window);
    Window GetMainDisplay();

    /// static functions
public:
    inline static bool IsReturnCodeOk(int code);
    static string GetErrorText(Display *pDisplay, int error_code);
    static int ErrorHandler_Grabber(Display *, XErrorEvent *);

    /// instance functions
public:

    /// error handler

    bool ErrorHandler_Set();
    bool ErrorHandler_Restore();

    /// sync and lock functions

    void Sync();
    void Lock();
    void Unlock();

    /// window

    // warning: the wm may change the window position
    // use Window_Create_Show, which will correct the position
    Window Window_Create(int nPosX, int nPosY, unsigned int nWidth, unsigned int nHeight, Window parent_window=None);

    // create the window, show it and correct the position
    Window Window_Create_Show(int nPosX, int nPosY, unsigned int nWidth, unsigned int nHeight, Window parent_window=None);

    // close a window
    bool Window_Destroy(Window window);

    // show or hide a window
    bool Window_Show(Window window, bool bShow=true);

    bool Window_MoveResize(Window window, int nPosX, int nPosY, unsigned int nWidth, unsigned int nHeight);

    bool Window_ClassName(Window window, const char *s_class, const char *s_name);
    bool Window_Name(Window window, const char *s_name);

    bool Window_Raise(Window window);
    bool Window_Lower(Window window);

    // opacity will be set recursive to parents
    // WINDOW_OPAQUE : normal window
    // 1 : very transparent
    // 0 : better not use it
    bool Window_SetOpacity(Window window, unsigned long nOpacity);

    // return 0 on error
    // can return the root window
    Window Window_GetParent(Window window);

    /// keyboard

    bool Keyboard_Grab(Window window_grab);
    bool Keyboard_Ungrab();

    bool Keyboard_SetAutoRepeat(bool bOn);

    /// mouse

    bool Mouse_Grab(Window window_grab, Window window_confine_to=None);
    bool Mouse_Ungrab();

    // if the parent window is the root window, use the same cursor as X11
    bool Mouse_ShowStandardCursor(Window window);
    bool Mouse_HideCursor(Window window);
    bool Mouse_SetCursor_Inherit(Window window);
    bool Mouse_SetCursor_Font(Window window, int nShape);
    bool Mouse_SetCursor_Image(Window window, const string &sPath, const string &sPathMask);
    bool Mouse_QueryMaxCursorSize(Window window, unsigned int &width_return, unsigned int &height_return);

    // accel is : accel_numerator/accel_denominator
    // threshold : used for "quick" user moves
    // defaults seems to be : accel_numerator==2 accel_denominator==1 threshold==4
    // defaults can be set with : -1 -1 -1
    bool Mouse_SetSpeed(int accel_numerator=-1, int accel_denominator=-1, int threshold=-1);

    bool Mouse_GetSpeed(int &accel_numerator_return, int &accel_denominator_return, int &threshold_return);

    bool Mouse_SetPosition(int nPosX, int nPosY);
    bool Mouse_GetPosition(int &nPosX, int &nPosY, bool bRelative=false);

    // activate   : both width and height must be > 0
    // deactivate : both width and height == 0
    bool Mouse_Constrain(int nPosX, int nPosY, unsigned int nWidth, unsigned int nHeight, Window window_grab=None);
    bool Mouse_Constrain_Release();
    bool Mouse_Constrain_ReactivateIfActive();
    bool Mouse_IsConstrainActive();
    Window Mouse_Constrain_GetWindow();

    /// cursors

    struct string_int
    {
        std::string sName;
        int nValue;
    };
    static const string_int v_aStdCursors_NameValue[];
    static const int v_aStdCursors_Count;

    // return -1 on error
    static int StdCursor_GetValueByName(const std::string &sName);

    /// graphics

    // Pixmap_Delete() should be called
    // depth==1 for a bitmap
    Pixmap Pixmap_Create(Window window, unsigned int width, unsigned int height, unsigned int depth);

    // actually XFreePixmap() with error-checking
    bool Pixmap_Delete(Pixmap &pixmap);

    // Pixmap_Delete() should be called
    bool Pixmap_ReadFile(Window window, const string &sPath, Pixmap &pixmap_return, unsigned int &width_return, unsigned int &height_return, int &x_hot_return, int &y_hot_return);

    /// other

    // drawable can be: Window, Pixmap
    // NULL arguments can be passed, for un-needed values
    // x and y are always 0 for a Pixmap
    bool Object_GetGeometry(Drawable drawable, int *x_return, int *y_return, unsigned int *width_return, unsigned int *height_return);

protected:
    int Mouse_Grab_Helper(Window window_grab, Window window_confine_to);
    int Mouse_Ungrab_Helper();
    // opacity only for this window
    bool Window_SetOpacity_Helper(Window window, unsigned long nOpacity);

    Display *v_pDisplay;
    bool v_bIsAssigned_Display;

    Window v_oMainWindow;

    XErrorHandler v_pOld_XErrorHandler;
    bool v_bIsChanged_XErrorHandler;

    bool v_bIsActive_Mouse_Constrain;
    Window v_window_Mouse_Constrain;
    struct Info_Mouse_Constrain
    {
        Info_Mouse_Constrain()
                : window_grab(None)
                , nPosX(0)
                , nPosY(0)
                , nWidth(0)
                , nHeight(0)
            {
            }
        Window window_grab;
        int nPosX;
        int nPosY;
        unsigned int nWidth;
        unsigned int nHeight;
    };
    Info_Mouse_Constrain previous_mouse_constrain;

    /// shape extension
public:
    bool Extension_Shape_IsAvailable();
    bool Extension_Shape_GetVersion_Major();
    bool Extension_Shape_GetVersion_Minor();

    // creating a callback for drawing :  not flexible enough
    // usage:
    //   call Shape_Context_Enter()
    //   remember the return values : Pixmap, GC, Display
    //   DRAW ONLY masked pixels in the Pixmap, using GC and Display
    //   call Shape_Window_Apply()
    //   call Shape_Context_Leave()

    // if returns false, then the return values are not valid
    // return values : Pixmap, GC, Display
    // Pixmap depth == 1
    bool Shape_Context_Enter(Window window, unsigned int width, unsigned int height, Pixmap &bitmap_mask, GC &gc, Display * &pDisplay);

    // returns false if the shape cannot be set
    // cleans up : Pixmap, GC, Display
    bool Shape_Context_Leave(Window window, Pixmap &bitmap_mask, GC &gc, Display * &pDisplay);

    // Pixmap should have depth==1
    bool Shape_Window_Apply(Window window, Pixmap &pixmap);

    bool Shape_Window_Reset(Window window);
    bool Shape_Window_Hide(Window window);

protected:
    bool Extension_Shape_Initialize();

    int v_bExtension_Shape_IsInitialized;
    bool v_bExtension_Shape_IsAvailable;
    int v_nExtension_Shape_Version_Major;
    int v_nExtension_Shape_Version_Minor;

    /// debug
public:
    Window Debug_Window(bool bCreate_NotClose=true);
    bool Debug_Shape_Context_Example(Window window, unsigned int width, unsigned int height);

    static int AfterFunction_Grabber(Display *);
    bool AfterFunction_Set();
    bool AfterFunction_Restore();

    X_AfterFunction v_pOld_X_AfterFunction;
    bool v_bIsChanged_X_AfterFunction;

    /// not yet finished
public:
    bool Shape_Window_Alpha(Window window, unsigned char nAlphaThreshold=128);

    // XDestroyImage should be called
    XImage * Window_GetImage(Window window, bool bOnlyMask=false);
    bool Window_GetPosition(Window window, int &nPosX, int &nPosY, unsigned int &nWidth, unsigned int &nHeight);
    Pixmap ConvertImageToPixmap(XImage *pXImage, Window window);
    bool Window_PerPixel_Transparency(Window window, unsigned long nAlphaValue=1);

};

#endif
// _WRAPPER_X11_H_
