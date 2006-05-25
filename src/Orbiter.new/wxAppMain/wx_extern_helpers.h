//
// Author : C Remus
//
// Changed by : ...
//

// extern code copied here
// compatibility code for debug-app

// class ExternData moved here

#ifndef _WX_EXTERN_HELPERS_H_
#define _WX_EXTERN_HELPERS_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wx_extern_helpers.h"
#endif

#ifdef USE_DEBUG_CODE
class PlutoRectangle
{
public:
    PlutoRectangle()
            : X(0)
            , Y(0)
            , Width(0)
            , Height(0)
        {
        }
    PlutoRectangle(int X, int Y, int Width, int Height)
            : X(X)
            , Y(Y)
            , Width(Width)
            , Height(Height)
        {
        }
    int X;
    int Y;
    int Width;
    int Height;
};
#endif // USE_DEBUG_CODE

#ifdef USE_DEBUG_CODE

// used as a base class to allow dynamic casts
// declared as a class, but should be used as a structure
// --
// intially named ExternData
// renamed to match an external class with the same functionality
class CallBackData
{
public:
    CallBackData() {};
    virtual ~CallBackData() {};
};

class PositionCallBackData : public CallBackData
{
public:
	PlutoRectangle m_rectPosition;
    bool m_bShowFullScreen;
	PositionCallBackData()
            : CallBackData()
            , m_rectPosition(0, 0, 0, 0)
            , m_bShowFullScreen(false)
        {}
	virtual ~PositionCallBackData() {}
};

class PopupCallBackData : public PositionCallBackData
{
public:
	unsigned long m_ulPopupID;
	PopupCallBackData()
            : PositionCallBackData()
            , m_ulPopupID(0)
        {}
	virtual ~PopupCallBackData() {}
};

class RoomWizardCallBackData : public PositionCallBackData
{
public:
    class WizardLogic *m_pWizardLogic;
    RoomWizardCallBackData()
            : m_pWizardLogic(NULL)
        {
        }
	RoomWizardCallBackData(WizardLogic *pWizardLogic, PlutoRectangle rectPosition)
        {
            m_pWizardLogic = pWizardLogic;
            m_rectPosition = rectPosition;
        }
    map<int,int> map_room_types; // key, value
    map<int,string> map_fn_GetRoomTypeName; //key, name
};

class WaitUserGridCallBackData : public PositionCallBackData
{
public:
    string m_sMessage;
	map<string, bool> m_mapChildDevices;
	int m_nPercent;
    WaitUserGridCallBackData()
            : m_nPercent(0)
        {
        }
	WaitUserGridCallBackData(const string &sMessage, const map<string, bool> &mapChildDevices, int nPercent)
        {
            m_sMessage = sMessage;
            m_mapChildDevices = mapChildDevices;
            m_nPercent = nPercent;
        }
};

class WaitUserListCallBackData : public PositionCallBackData
{
public:
    string m_sMessage;
	int m_nPercent;
    WaitUserListCallBackData()
            : m_nPercent(0)
        {
        }
    WaitUserListCallBackData(const string &sMessage, int nPercent)
        {
            m_sMessage = sMessage;
            m_nPercent = nPercent;
        }
};

class WaitUserPromptCallBackData : public PositionCallBackData
{
public:
    string m_sMessage;
    int m_nTimeoutSeconds;
    map<int,string> m_mapPrompts;
    WaitUserPromptCallBackData()
            : m_nTimeoutSeconds(0)
        {
        }
	WaitUserPromptCallBackData(const string &sMessage, int nTimeoutSeconds, const map<int,string> &mapPrompts)
        {
            m_sMessage = sMessage;
            m_nTimeoutSeconds = nTimeoutSeconds;
            m_mapPrompts = mapPrompts;
        }
};

class SpeedControlCallBackData : public PopupCallBackData
{
public:
    enum Style
    {
        UNUSED,
        TIME,
        TIME_SEEK,
        TIME_SPEED,
        SPEED,
    };
    Style m_eStyle;
    list<int> m_listSpeeds; // only for TIME, TIME_SPEED
    int m_nSpeed;
    int m_nTimeStart;
    int m_nTimeEnd;
    int m_nTimeNow;
    int m_nSeekToPos;
	SpeedControlCallBackData() :
            m_eStyle(UNUSED),
            m_nSpeed(0),
            m_nTimeStart(0),
            m_nTimeEnd(0),
            m_nTimeNow(0),
            m_nSeekToPos(0)
		{
        }
	SpeedControlCallBackData(PlutoRectangle rectPosition) :
            m_eStyle(UNUSED),
            m_nSpeed(0),
            m_nTimeStart(0),
            m_nTimeEnd(0),
            m_nTimeNow(0),
            m_nSeekToPos(0)
		{
            m_rectPosition = rectPosition;
        }
	SpeedControlCallBackData(
        Style eStyle,
        list<int> listSpeeds,
        int nSpeed,
        int nTimeStart,
        int nTimeEnd,
        int nTimeNow,
        int nSeekToPos
        )
        {
            m_eStyle = eStyle;
            m_listSpeeds = listSpeeds;
            m_nSpeed = nSpeed;
            m_nTimeStart = nTimeStart;
            m_nTimeEnd = nTimeEnd;
            m_nTimeNow = nTimeNow;
            m_nSeekToPos = nSeekToPos;
        }
};

class VolumeControlCallBackData : public PopupCallBackData
{
public:
    enum Style
    {
        UNUSED,
        RULER,
        SPEED,
    };
    Style m_eStyle;
    int m_nPositions;
    int m_nCrtPosition;
	VolumeControlCallBackData()
            : m_eStyle(UNUSED)
            , m_nPositions(0)
            , m_nCrtPosition(0)
        {
        }
	VolumeControlCallBackData(
        Style eStyle,
        int nPositions,
        int nCrtPosition
        )
        {
            m_eStyle = eStyle;
            m_nPositions = nPositions;
            m_nCrtPosition = nCrtPosition;
        }
};

class LightControlCallBackData : public PopupCallBackData
{
public:
    int m_nPositions;
    int m_nCrtPosition;
	LightControlCallBackData()
            : m_nPositions(0)
            , m_nCrtPosition(0)
        {
        }
	LightControlCallBackData(
        int nPositions,
        int nCrtPosition
        )
        {
            m_nPositions = nPositions;
            m_nCrtPosition = nCrtPosition;
        }
};

#endif // USE_DEBUG_CODE

#ifdef USE_RELEASE_CODE
#include "../CallBackData.h"
#endif // USE_RELEASE_CODE

const char * _str_enum(SpeedControlCallBackData::Style value);
const char * _str_enum(VolumeControlCallBackData::Style value);

class Extern_Task_Data : public CallBackData
{
public:
    Extern_Task_Data()
            : nEventId(0)
            , nButtonId(0)
        {
        }
    unsigned int nEventId;
    int nButtonId;
};

#ifdef USE_DEBUG_CODE
enum CallBackType
{
    cbOnDialog_Unused = 0,
    cbOnDialogCreate,   // create a unique dialog
    cbOnDialogDelete,   // close dialog without saving data
    cbOnDialogRefresh,  // refresh dialog with new data
    cbOnDialogSave,     // save data from dialog
    cbOnDialogWaitUser, // wait for an user action
};
const char * _str_enum(CallBackType value);
#endif // USE_DEBUG_CODE

// wx implementation for task-manager
// used for the debug-app only
#ifdef USE_DEBUG_CODE
#include "wx_dialog_types.h"

class Task
{
public:
	Task(CallBackType TaskType, E_DIALOG_TYPE DialogType, CallBackData* pCallBackData, size_t TaskId);

    CallBackType TaskType;
	E_DIALOG_TYPE DialogType;
	CallBackData* pCallBackData;
    size_t TaskId;

protected:
    bool b_IsWaiting;
    wxSemaphore v_oSemaphoreTaskWait;
    friend class TaskManagerImpl;
};
WX_DEFINE_ARRAY_PTR(Task *, wxArray_p_Task);

const char * _str_task(Task *pTask);

class TaskManagerImpl
{
public:
    TaskManagerImpl();
    Task * CreateTask(CallBackType TaskType, E_DIALOG_TYPE DialogType, CallBackData* pCallBackData );
	void AddTask(Task *pTask);
	void AddTaskAndWait(Task *pTask);
	Task *PopTask();
    void TaskProcessed(size_t nTaskId);
protected:
    wxMutex v_oMutex;
    size_t v_nNextUniqueId;
    wxArray_p_Task v_apTask;
};

class TaskManager
{
public:
    static TaskManagerImpl & Instance()
        {
            return _g_TaskManager;
        }
protected:
    static TaskManagerImpl _g_TaskManager;
};
#endif // USE_DEBUG_CODE

#endif
// _WX_EXTERN_HELPERS_H_
