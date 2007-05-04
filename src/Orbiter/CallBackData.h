/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#ifndef __CALLBACK_DATA_H__
#define __CALLBACK_DATA_H__

#include "SerializeClass/ShapesColors.h"

namespace DCE
{
	class DataGridCell;
	class DataGridTable;
};

class DesignObj_DataGrid;
using namespace DCE;
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
    bool m_bShowFullScreen;

	PositionCallBackData()
            : CallBackData()
            , m_rectPosition(0, 0, 0, 0)
            , m_bShowFullScreen(false)
        {}
	virtual ~PositionCallBackData() {}
};
//-----------------------------------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------------------------------
class PopupBitmapCallBackData : public PopupCallBackData
{
public:
	char *m_pRawBitmapData;
	size_t m_ulSize;

	PopupBitmapCallBackData(): PopupCallBackData(),
		m_pRawBitmapData(NULL), m_ulSize(0) {}
	virtual ~PopupBitmapCallBackData() {}
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
	class DesignObj_Orbiter *m_pObj;
	int m_X,m_Y;
	ObjectInfoBackData() : m_pObj(NULL) { m_X=m_Y=0; }
};
//-----------------------------------------------------------------------------------------------------
class DatagridCellBackData : public CallBackData
{
public:
	string m_sText;
	string m_sValue;
	long m_nPK_Datagrid;
	DataGridCell *m_pDataGridCell;
	DesignObj_DataGrid *m_pDesignObj_DataGrid;
	int m_Row,m_Column;

	DatagridCellBackData() : m_pDataGridCell(NULL), m_pDesignObj_DataGrid(NULL), m_Row(0), m_Column(0) {}
};
//-----------------------------------------------------------------------------------------------------
class DatagridAcquiredBackData : public CallBackData
{
public:
	DesignObj_DataGrid *m_pObj;
	DataGridTable *m_pDataGridTable;

	DatagridAcquiredBackData() : m_pObj(NULL), m_pDataGridTable(NULL) {}
};
//-----------------------------------------------------------------------------------------------------
namespace DCE
{
	class DeviceData_Base;
	class Message;
	class Socket;
};

class MsgInterceptorCellBackData : public CallBackData
{
public:
	DCE::Socket *m_pSocket;
	DCE::Message *m_pMessage;
	DCE::DeviceData_Base *m_pDeviceFrom, *m_pDeviceTo;
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
	DCE::Message *m_pMessage;

	GotoScreenCallBackData()
	{
		m_pMessage=NULL;
	}
};
//-----------------------------------------------------------------------------------------------------
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
};
//-----------------------------------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------------------
class RemoteKeyCodeCallBackData : public CallBackData
{
public:
	string m_sKeyCode;
};
//-----------------------------------------------------------------------------------------------------
#endif
