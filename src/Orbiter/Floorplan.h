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
	virtual void GetFloorplanDeviceInfo(DeviceData_Router *pDeviceData_Router,EntertainArea *pEntertainArea,int iFloorplanObjectType,int &iPK_FloorplanObjectType_Color,int &Color,string &sDescription,string &OSD)=0;
};

#endif

