#ifndef _CALLBACKTYPES_H_
#define _CALLBACKTYPES_H_

//used by screen handler class
enum CallBackType
{
    cbDataGridSelected,
    cbObjectSelected,
    cbOnCreateWxWidget,  // create a unique dialog
    cbOnDeleteWxWidget,  // close dialog without saving data
    cbOnRefreshWxWidget, // refresh dialog with new data
    cbOnSaveWxWidget,    // save data from dialog
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
