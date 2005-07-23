#ifndef DESIGNOBJ_DATA_H
#define DESIGNOBJ_DATA_H

#include "SerializeClass/SerializeClass.h"
#include "SerializeClass/ShapesColors.h"
#include "TextStyle.h"

#include <map>
#include <list>
using namespace std;

// We've got some custom data types to serialize
#define SERIALIZE_DATA_TYPE_INT_DESIGNOBJACTIONLIST		1000
#define SERIALIZE_DATA_TYPE_INT_DESIGNOBJZONELIST		1001
#define SERIALIZE_DATA_TYPE_VECT_DESIGNOBJTEXT			1002
#define SERIALIZE_DATA_TYPE_INT_DESIGNOBJCHILDLIST		1003
#define SERIALIZE_DATA_TYPE_INT_STYLEMAP				1004

#define SERIALIZE_DATA_TYPE_DESIGNOBJ_CMD_DATA			1005

typedef list<class DesignObjCommandParameter *> DesignObjCommandParameterList;
typedef list<class DesignObjCommand *> DesignObjCommandList;
typedef list<class DesignObjZone *> DesignObjZoneList;

class DesignObjCommand : public SerializeClass
{
public:
	struct
	{
		int m_PK_Command,m_PK_Device,m_PK_DeviceGroup; 
		int BroadcastLevel,m_PK_DeviceTemplate,m_PK_DeviceCategory;
		bool m_bRelativeToSender,m_bDeliveryConfirmation;
	} Data; // Put them all in 1 struct to speed up the serialization

	map<int, string> m_ParameterList;
	class Orbiter_CriteriaList *ptrCriteria;

	DesignObjCommand() : ptrCriteria(NULL) {};
	DesignObjCommand(int PK_Command,int PK_Criteria,bool bHandleLocally,int PK_Device,int PK_DeviceGroup, bool bDeliveryConfirmation) 
		: ptrCriteria(NULL) 
	{
		Data.m_PK_Command=PK_Command;
		Data.m_PK_Device=PK_Device;
		Data.m_PK_DeviceGroup=PK_DeviceGroup;
		Data.m_bDeliveryConfirmation=bDeliveryConfirmation;
	};
	virtual ~DesignObjCommand() { m_ParameterList.clear(); } 

	void SetupSerialization(int iSC_Version)
	{
		StartSerializeList() + m_ParameterList;
	}
	virtual string SerializeClassClassName() { return "DesignObjCommand"; }
	virtual bool Serialize(bool bWriting, char *&pcDataBlock, unsigned long &dwAllocatedSize, char *&pcCurrentPosition, void *pExtraSerializationData=NULL);
}; 

class DesignObjZone : public SerializeClass
{
public:
	DesignObjZone(int X,int Y,int Width,int Height) 
	{
		m_Rect.X=X; m_Rect.Y=Y; m_Rect.Width=Width; m_Rect.Height=Height;
		
	};
	DesignObjZone() { }
	~DesignObjZone()
	{
		DesignObjCommandList::iterator iA;

		for(iA = m_Commands.begin(); iA != m_Commands.end(); ++iA)
			delete *iA;
		
		m_Commands.clear();
	}
	PlutoRectangle m_Rect;
	DesignObjCommandList m_Commands;

	void SetupSerialization(int iSC_Version)
	{
		StartSerializeList();
		(*this) + m_Commands; // this is serialized custom
	}
	virtual string SerializeClassClassName() { return "DesignObjZone"; }

	// For our custom serialize types.
	DesignObjZone &operator+ (DesignObjCommandList &i) { m_vectItemToSerialize.push_back(new ItemToSerialize(SERIALIZE_DATA_TYPE_INT_DESIGNOBJACTIONLIST,(void *) &i)); return (*this); }

	virtual bool UnknownSerialize(ItemToSerialize *pItem,bool bWriting,char *&pDataBlock,unsigned long &iAllocatedSize,char *&pCurrentPosition);
	virtual bool Serialize(bool bWriting, char *&pcDataBlock, unsigned long &dwAllocatedSize, char *&pcCurrentPosition, void *pExtraSerializationData=NULL);  // Serialize manually for speed
}; 

typedef list<class DesignObj_Data *> DesignObj_DataList;
typedef map<string, class DesignObj_Data*> DesignObj_DataMap;

class DesignObjText : public SerializeClass
{
public:
	map<int,string> m_mapAltVersions;
	struct
	{
		PlutoRectangle m_rPosition;
		bool m_bPreRender;
		int m_PK_Text;
		int m_iLastXOffset,m_iLastYOffset;  // Used only within the Orbiter
		int m_iPK_VertAlignment,m_iPK_HorizAlignment;
	} Data;

	string m_sText;
	MapTextStyle m_mapTextStyle;
	TextStyle *m_mapTextStyle_Find(int Version)
	{
		MapTextStyle::iterator it = m_mapTextStyle.find(Version);
		return it==m_mapTextStyle.end() ? NULL : (*it).second;
	}

	class DesignObj_Orbiter *m_pObject;

	DesignObjText(class DesignObj_Orbiter *pObj) 
	{ 
		m_pObject=pObj; Data.m_iLastXOffset=Data.m_iLastYOffset=0; Data.m_iPK_VertAlignment=Data.m_iPK_HorizAlignment=0;
	}
	DesignObjText() { m_pObject = NULL; }
	
	virtual ~DesignObjText() 
	{  
		m_pObject = NULL; //someone else should detele the object

		MapTextStyle::iterator it;

		for(it = m_mapTextStyle.begin(); it != m_mapTextStyle.end(); it++)
		{
			delete (*it).second;
			(*it).second = NULL;
		}
		m_mapTextStyle.clear();
	};

	void SetupSerialization(int iSC_Version)
	{
		StartSerializeList() + m_sText + m_mapAltVersions;
		
		(*this) + m_mapTextStyle;
	}
	virtual string SerializeClassClassName() { return "DesignObjText"; }
	DesignObjText &operator+ (MapTextStyle &i) { m_vectItemToSerialize.push_back(new ItemToSerialize(SERIALIZE_DATA_TYPE_INT_STYLEMAP,(void *) &i)); return (*this); }
	virtual bool UnknownSerialize(ItemToSerialize *pItem,bool bWriting,char *&pDataBlock,unsigned long &iAllocatedSize,char *&pCurrentPosition);
	virtual bool Serialize(bool bWriting, char *&pcDataBlock, unsigned long &dwAllocatedSize, char *&pcCurrentPosition, void *pExtraSerializationData=NULL);  // Serialize manually for speed
};

typedef vector<DesignObjText *> VectorDesignObjText;

class DesignObj_Data : public SerializeClass
{
public:
	// Our data
	struct
	{
		bool m_bChild,m_bDontResetState,m_bCantGoBack,m_bCanGoBackToSameScreen,m_bChildrenBeforeText,m_bProcessActionsAtServer,m_bAnimate,m_bHideByDefault;
		bool m_bTabStop;
		int m_PK_DesignObj_Up,m_PK_DesignObj_Down,m_PK_DesignObj_Left,m_PK_DesignObj_Right,m_iRegenInterval;
		int m_iPK_Button,m_dwTimeoutSeconds,m_Priority;
		int m_iPK_Criteria;
		int m_iBaseObjectID,m_iVersion,m_iPage;  // Version & Page will only be used for top level objects (screens)
		int m_ObjectType;
		PlutoRectangle m_rectDontDim, m_rPosition, m_rBackgroundPosition;
		class PlutoSize m_sOriginalSize;
		int m_dwIDim;
		// If non-zero, this is a repeating button, and as long as it's held down, the command will keep
		// repeating until the user lets go this many milliseconds.  A parameter 'Repeat' will be added if bRepeatParm is true
		int m_iRepeatIntervalInMS;
		bool m_bRepeatParm;
	} Data;

	string m_sPK_DesignObj_TiedTo;
	string m_sVisibleState;
	string m_sBackgroundFile, m_sSelectedFile, m_sHighlightGraphicFilename;
	vector<string> m_vectAltGraphicFilename;
	map<int,string> m_mapObjParms;
	string m_ObjectID;
	PlutoDataBlock m_dbHitTest;


	DesignObjCommandList m_Action_LoadList,m_Action_UnloadList,m_Action_TimeoutList,m_Action_StartupList;
	DesignObjZoneList m_ZoneList;
	VectorDesignObjText m_vectDesignObjText;
	DesignObj_DataList m_ChildObjects;
	class DesignObj_Data *m_pParentObject;

	DesignObj_Data() 
	{
		Data.m_PK_DesignObj_Up=Data.m_PK_DesignObj_Down=Data.m_PK_DesignObj_Left=Data.m_PK_DesignObj_Right=0;
		m_sPK_DesignObj_TiedTo="";
		Data.m_iRegenInterval=Data.m_iRepeatIntervalInMS=Data.m_iVersion=Data.m_iPage=0;
		Data.m_bTabStop = Data.m_bRepeatParm=false;
		return;
	}

	void SetupSerialization(int iSC_Version)
	{
		StartSerializeList() + 
			m_sBackgroundFile +  m_sSelectedFile +  m_sHighlightGraphicFilename +
			m_vectAltGraphicFilename + m_mapObjParms +
			m_ObjectID + m_sPK_DesignObj_TiedTo + m_sVisibleState + m_dbHitTest;

		// These are handled locally, so start with this
		(*this) + m_Action_LoadList + m_Action_UnloadList + m_Action_TimeoutList + m_Action_StartupList +
			m_ZoneList + m_ChildObjects + m_vectDesignObjText;
	}

	// For our custom serialize types
	DesignObj_Data &operator+ (DesignObjCommandList &i) { m_vectItemToSerialize.push_back(new ItemToSerialize(SERIALIZE_DATA_TYPE_INT_DESIGNOBJACTIONLIST,(void *) &i)); return (*this); }
	DesignObj_Data &operator+ (DesignObjZoneList &i) { m_vectItemToSerialize.push_back(new ItemToSerialize(SERIALIZE_DATA_TYPE_INT_DESIGNOBJZONELIST,(void *) &i)); return (*this); }
	DesignObj_Data &operator+ (VectorDesignObjText &i) { m_vectItemToSerialize.push_back(new ItemToSerialize(SERIALIZE_DATA_TYPE_VECT_DESIGNOBJTEXT,(void *) &i)); return (*this); }
	DesignObj_Data &operator+ (DesignObj_DataList &i) { m_vectItemToSerialize.push_back(new ItemToSerialize(SERIALIZE_DATA_TYPE_INT_DESIGNOBJCHILDLIST,(void *) &i)); return (*this); }

	virtual bool UnknownSerialize(ItemToSerialize *pItem,bool bWriting,char *&pDataBlock,unsigned long &iAllocatedSize,char *&pCurrentPosition);

	virtual string SerializeClassClassName() { return "DesignObj_Data"; }
	virtual ~DesignObj_Data();

	class DesignObj_Data *TopMostObject() { return m_pParentObject ? m_pParentObject->TopMostObject() : this; }
	virtual bool Serialize(bool bWriting, char *&pcDataBlock, unsigned long &dwAllocatedSize, char *&pcCurrentPosition, void *pExtraSerializationData=NULL);
};

#endif
