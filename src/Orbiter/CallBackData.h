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
class WaitUserGridCallBackData : public CallBackData
{
public:
    string m_sMessage;
	map<string, bool> m_mapChildDevices;
	int m_nProgress;

	WaitUserGridCallBackData(const string &sMessage, const map<string, bool> &mapChildDevices, int nProgress)
	{
        m_sMessage = sMessage;
		m_mapChildDevices = mapChildDevices;
		m_nProgress = nProgress;
	}
};
//-----------------------------------------------------------------------------------------------------
class WaitUserListCallBackData : public CallBackData
{
public:
    string m_sMessage;
	int m_nProgress;

	WaitUserListCallBackData(const string &sMessage, int nProgress)
	{
        m_sMessage = sMessage;
		m_nProgress = nProgress;
	}
};
//-----------------------------------------------------------------------------------------------------
class WaitUserPromptCallBackData : public CallBackData
{
public:
    string m_sPrompt;
    int m_iTimeoutSeconds;
    map<int,string> m_mapPrompts;

	WaitUserPromptCallBackData(const string &sPrompt, int iTimeoutSeconds, const map<int,string> &mapPrompts)
	{
        m_sPrompt = sPrompt;
        m_iTimeoutSeconds = iTimeoutSeconds;
        m_mapPrompts = mapPrompts;
	}
};
//-----------------------------------------------------------------------------------------------------
class RoomWizardCallBackData : public CallBackData
{
public:
    class WizardLogic *m_pWizardLogic;

	RoomWizardCallBackData(WizardLogic *pWizardLogic)
	{
        m_pWizardLogic = pWizardLogic;
	}
};
//-----------------------------------------------------------------------------------------------------
class RoomWizardRefreshCallBackData : public CallBackData
{
public:
    PlutoRectangle m_pRect;

	RoomWizardRefreshCallBackData(PlutoRectangle pRect)
	{
        m_pRect = pRect;
	}
};
//-----------------------------------------------------------------------------------------------------
#endif
