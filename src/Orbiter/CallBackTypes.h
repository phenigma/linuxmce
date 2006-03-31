#ifndef _CALLBACKTYPES_H_
#define _CALLBACKTYPES_H_

//used by screen handler class
enum CallBackType
{
    cbDataGridSelected,
    cbObjectSelected,
    cbOnWxWidgetCreate,   // create a unique dialog
    cbOnWxWidgetDelete,   // close dialog without saving data
    cbOnWxWidgetRefresh,  // refresh dialog with new data
    cbOnWxWidgetSave,     // save data from dialog
    cbOnWxWidgetWaitUser, // wait for an user action
    cbOnKeyDown,
    cbOnKeyUp,
    cbOnMouseDown,
    cbOnMouseUp,
    cbCapturedKeyboardBufferChanged,
    cbOnTimer,
    cbMessageIntercepted,
    cbOnRenderScreen,
    cbOnGotoScreen
};

#endif
// _CALLBACKTYPES_H_
