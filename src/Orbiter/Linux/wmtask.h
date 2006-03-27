#ifndef WM_TASK_H
#define WM_TASK_H

// Window Message Task

#include "../CallBackTypes.h"
#include "../wxAppMain/wx_dialog_types.h"

class CallBackData;

class WMTask
{
public:
    CallBackType TaskType;
	E_DIALOG_TYPE DialogType;
	CallBackData* pCallBackData;
    unsigned int TaskId;

	/**
	 *	Property completion constructor
	 */
	WMTask(CallBackType TaskType, E_DIALOG_TYPE DialogType, CallBackData* pCallBackData, unsigned int TaskId);
};

#endif
// WM_TASK_H
