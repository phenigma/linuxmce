#ifndef FLOORPLAN_H
#define FLOORPLAN_H

typedef vector<class FloorplanObject *> FloorplanObjectVector;
typedef map<int,FloorplanObjectVector *> FloorplanObjectVectorMap;
typedef map<int,class FloorplanItemStatus *> FloorplanItemStatusMap;

namespace DCE
{
	class DeviceData_Router;
}

class FloorplanObject
{
public:
	int PK_Device,Page,Type,FillX,FillY;
	string DeviceDescription;  // Only used by Controller
	string ObjectTypeDescription;
	string ObjectID;
	string Status;
	DCE::DeviceData_Base *m_pDeviceData_Base;
	class DesignObj_Orbiter *pObj; // Only used by the orbiter
};

class FloorplanItemStatus
{
public:
	string Description;
	int Color,PKID_C_FloorplanObjectType_Color;
};

#endif

