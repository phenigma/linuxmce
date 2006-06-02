#include "Task.h"

Task::Task(CallBackType TaskType, E_DIALOG_TYPE DialogType, CallBackData* pCallBackData, unsigned int TaskId)
{
	this->TaskType = TaskType;
	this->DialogType = DialogType;
	this->pCallBackData = pCallBackData;
    this->TaskId = TaskId;
}
