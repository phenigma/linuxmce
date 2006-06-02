#ifndef __TASK_H__
#define __TASK_H__

// Window Message Task

#include "CallBackTypes.h"
#include "dialog_types.h"

class CallBackData;

class Task
{
public:
    CallBackType TaskType;
	E_DIALOG_TYPE DialogType;
	CallBackData* pCallBackData;
    unsigned int TaskId;

	/**
	 *	Property completion constructor
	 */
	Task(CallBackType TaskType, E_DIALOG_TYPE DialogType, CallBackData* pCallBackData, unsigned int TaskId);
};

#endif
// WM_TASK_H
