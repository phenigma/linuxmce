#ifndef FLOORPLAN_H
#define FLOORPLAN_H

typedef vector<class FloorplanObject *> FloorplanObjectVector;
typedef map<int,FloorplanObjectVector *> FloorplanObjectVectorMap;
typedef map<int,class FloorplanItemStatus *> FloorplanItemStatusMap;

class FloorplanObject
{
public:
	int PK_Device,Page,Type,FillX,FillY;
	string DeviceDescription;  // Only used by Controller
	string ObjectTypeDescription;
	string ObjectID;
	string Status;
	class DeviceData_Router *ptrDevice;   // Only used by PlutoServer
	class DesignObj_Controller *pObj; // Only used by Controller
};

class FloorplanItemStatus
{
public:
	string Description;
	int Color,PK_FloorplanObjectType_Color;
};

#endif

