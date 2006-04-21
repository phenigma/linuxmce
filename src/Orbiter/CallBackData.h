#ifndef __CALLBACK_DATA_H__
#define __CALLBACK_DATA_H__

#include "SerializeClass/ShapesColors.h"
//-----------------------------------------------------------------------------------------------------
class CallBackData
{
public:
	CallBackData() {}
	virtual ~CallBackData() {}
};
//-----------------------------------------------------------------------------------------------------
class PositionCallBackData : public CallBackData
{
public:
	PlutoRectangle m_rectPosition;
};
//-----------------------------------------------------------------------------------------------------
class KeyCallBackData : public CallBackData
{
public:
	int m_nPlutoKey;
};
//-----------------------------------------------------------------------------------------------------
class MouseClickCallBackData : public CallBackData
{
public:
	int m_x, m_y;
};
//-----------------------------------------------------------------------------------------------------
class ObjectInfoBackData : public CallBackData
{
public:
	long m_PK_DesignObj_SelectedObject;
};
//-----------------------------------------------------------------------------------------------------
class DatagridCellBackData : public CallBackData
{
public:
	string m_sText;
	string m_sValue;
	long m_nPK_Datagrid;
};
//-----------------------------------------------------------------------------------------------------
class MsgInterceptorCellBackData : public CallBackData
{
public:
	class Socket *m_pSocket;
	class Message *m_pMessage;
	class DeviceData_Base *m_pDeviceFrom, *m_pDeviceTo;
};
//-----------------------------------------------------------------------------------------------------
class RenderScreenCallBackData : public CallBackData
{
public:
	int m_nPK_Screen;
	class DesignObj_Orbiter *m_pObj;
};
//-----------------------------------------------------------------------------------------------------
class GotoScreenCallBackData : public CallBackData
{
public:
	int m_nPK_Screen;
};
//-----------------------------------------------------------------------------------------------------
class RoomWizardCallBackData : public CallBackData
{
public:
    class WizardLogic *m_pWizardLogic;
    PlutoRectangle m_coord;

	RoomWizardCallBackData(WizardLogic *pWizardLogic, PlutoRectangle coord)
	{
        m_pWizardLogic = pWizardLogic;
        m_coord = coord;
	}
};
//-----------------------------------------------------------------------------------------------------
class WaitUserGridCallBackData : public CallBackData
{
public:
    string m_sMessage;
	map<string, bool> m_mapChildDevices;
	int m_nPercent;

	WaitUserGridCallBackData(const string &sMessage, const map<string, bool> &mapChildDevices, int nPercent)
	{
        m_sMessage = sMessage;
		m_mapChildDevices = mapChildDevices;
		m_nPercent = nPercent;
	}
};
//-----------------------------------------------------------------------------------------------------
class WaitUserListCallBackData : public CallBackData
{
public:
    string m_sMessage;
	int m_nPercent;

	WaitUserListCallBackData(const string &sMessage, int nPercent)
	{
        m_sMessage = sMessage;
		m_nPercent = nPercent;
	}
};
//-----------------------------------------------------------------------------------------------------
class WaitUserPromptCallBackData : public CallBackData
{
public:
    string m_sMessage;
    int m_nTimeoutSeconds;
    map<int,string> m_mapPrompts;

	WaitUserPromptCallBackData(const string &sMessage, int nTimeoutSeconds, const map<int,string> &mapPrompts)
	{
        m_sMessage = sMessage;
        m_nTimeoutSeconds = nTimeoutSeconds;
        m_mapPrompts = mapPrompts;
	}
};

//-----------------------------------------------------------------------------------------------------
class SpeedControlCallBackData : public CallBackData
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
    PlutoRectangle m_coord;

	SpeedControlCallBackData(PlutoRectangle coord) : 
		m_eStyle(UNUSED),
		m_nSpeed(0),
    	m_nTimeStart(0),
    	m_nTimeEnd(0),
    	m_nTimeNow(0),
    	m_nSeekToPos(0),
		m_coord (coord)
		{}
		
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
			m_coord = PlutoRectangle(0, 0, 0, 0);
        }
};

//-----------------------------------------------------------------------------------------------------
class VolumeControlCallBackData : public CallBackData
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
    PlutoRectangle m_coord;

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

//-----------------------------------------------------------------------------------------------------
class LightControlCallBackData : public CallBackData
{
public:
    int m_nPositions;
    int m_nCrtPosition;
    PlutoRectangle m_coord;

	LightControlCallBackData(
        int nPositions,
        int nCrtPosition
        )
        {
            m_nPositions = nPositions;
            m_nCrtPosition = nCrtPosition;
        }
};

//-----------------------------------------------------------------------------------------------------

#endif
