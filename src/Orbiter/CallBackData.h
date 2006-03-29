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
#endif
