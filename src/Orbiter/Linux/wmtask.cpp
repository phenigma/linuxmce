#include "wmtask.h"

//TODO: remove this file, now it is "../Task.cpp"

WMTask::WMTask(CallBackType TaskType, E_DIALOG_TYPE DialogType, CallBackData* pCallBackData, unsigned int TaskId)
{
	this->TaskType = TaskType;
	this->DialogType = DialogType;
	this->pCallBackData = pCallBackData;
    this->TaskId = TaskId;
}
