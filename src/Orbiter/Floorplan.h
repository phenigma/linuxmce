#ifndef FLOORPLAN_H
#define FLOORPLAN_H

typedef vector<class FloorplanObject *> FloorplanObjectVector;
typedef map<int,FloorplanObjectVector *> FloorplanObjectVectorMap;
typedef map<int,class FloorplanItemStatus *> FloorplanItemStatusMap;

namespace DCE
{
	class DeviceData_Router;
	class DeviceData_Base;
	class EntertainArea;
}

using namespace DCE;

class FloorplanObject
{
public:
	int PK_Device,Page,Type,FillX,FillY;
	string DeviceDescription;  // Only used by Controller
	string ObjectTypeDescription;
	string ObjectID;
	string Status;
#ifdef ORBITER
	DCE::DeviceData_Base *m_pDeviceData_Base;
	int m_dwPK_DesignObj_Toolbar; // The toolbar to control this object
#else
	DCE::DeviceData_Router *m_pDeviceData_Router;
	DCE::EntertainArea *m_pEntertainArea;
#endif
	class DesignObj_Orbiter *pObj; // Only used by the orbiter
};

class FloorplanItemStatus
{
public:
	string Description;
	int Color,PKID_C_FloorplanObjectType_Color;
};

class FloorplanInfoProvider
{
public:
	// Color is the color to fill the icon with, Description is the status which
	// appears at the bottom of the floorplan when the item is selected, OSD is
	// the text will be put into any text object within the icon (like the temperature
	// next to a thermastat, and PK_DesignObj_Toolbar is the toolbar to activate
	// when the object is selected.
	virtual void GetFloorplanDeviceInfo(DeviceData_Router *pDeviceData_Router,EntertainArea *pEntertainArea,int iFloorplanObjectType,int &iPK_FloorplanObjectType_Color,int &Color,string &sDescription,string &OSD,int &PK_DesignObj_Toolbar)=0;
};

#endif

