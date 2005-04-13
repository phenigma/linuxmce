#ifndef OrbiterData_h
#define OrbiterData_h

#include "pluto_main/Define_DesignObjType.h"
#include "SerializeClass/ShapesColors.h"
#include "TextStyle.h"
#include "Message.h"

#include <deque>

#define ORBITER_SCHEMA		4	// Used to determine if cached screens can be used or if the schema has changed

#ifdef ORBITER
#include "DesignObj_Orbiter.h"
#endif

// This is all the data for a Orbiter that we will serialize.  Be sure "ORBITER" is defined when including this in the Orbiter projects.
// Note that the generator also uses this class to serialize the object data, but in that case, it's serializing m_ScreenMap as a list of 
// DesignObj_Data pointers.  The Orbiter adds more functionality to DesignObj_Data and needs DesignObj_Orbiter.  The serialization still
// works fine because DesignObj_Orbiter doesn't add any extra serialization.

#define SERIALIZE_DATA_TYPE_SCREEN_MAP		2000
#define SERIALIZE_DATA_TYPE_STYLE_MAP		2001
#define SERIALIZE_DATA_TYPE_LOCATIONS		2002

typedef map<int, string> VariableMap;

#ifdef ORBITER
	#include "DesignObj_Orbiter.h"
	typedef DesignObj_OrbiterMap ScreenMap;
#else
	#include "Orbiter/DesignObj_Data.h"
	typedef DesignObj_DataMap ScreenMap;
#endif

class LocationInfo : public SerializeClass
{
public:
	int PK_Room,PK_EntertainArea,iLocation;
	// Some virtual devices within the media director
	int m_dwPK_Device_MediaDirector,m_dwPK_Device_DiscDrive,m_dwPK_Device_AppServer,m_dwPK_Device_MediaBurner,m_dwPK_Device_VideoConf,m_dwPK_Device_CamcorderCapt,m_dwPK_Device_Orbiter,m_dwPK_Device_WebBrowser; 
	string Description;
	class Row_Icon *drIcon; // Not serialized, just a temporary pointer for the generator
	vector<class PlutoGraphic*> *m_pvectGraphic; // Not serialized, just a temporary pointer for the orbiter with the button for this location

	LocationInfo() { m_dwPK_Device_MediaDirector = m_dwPK_Device_DiscDrive = m_dwPK_Device_AppServer = m_dwPK_Device_MediaBurner = m_dwPK_Device_VideoConf = m_dwPK_Device_CamcorderCapt = m_dwPK_Device_Orbiter = m_dwPK_Device_WebBrowser = DEVICEID_NULL; }
	void SetupSerialization(int iSC_Version)
	{
		StartSerializeList() + PK_Room + PK_EntertainArea + iLocation + Description +
			m_dwPK_Device_MediaDirector + m_dwPK_Device_DiscDrive + m_dwPK_Device_AppServer + m_dwPK_Device_MediaBurner + m_dwPK_Device_VideoConf + 
			m_dwPK_Device_CamcorderCapt + m_dwPK_Device_Orbiter + m_dwPK_Device_WebBrowser;
	}
	virtual string SerializeClassClassName() { return "LocationInfo"; }
};

typedef deque<LocationInfo *> DequeLocationInfo;


class OrbiterData : public SerializeClass
{
public:
	int m_Width, m_Height, m_AnimationStyle, m_dwPK_Users_Default, m_iLocation_Initial;
	unsigned long m_tGenerationTime;
	string m_sMainMenu,m_sSleepingMenu,m_sScreenSaveMenu;
	ScreenMap m_ScreenMap;
	map<int,string> m_mapTextString;

	// Some global devices
	int m_dwPK_Device_Router,m_dwPK_Device_DatagridPlugIn,m_dwPK_Device_InfraredPlugIn,m_dwPK_Device_GeneralInfoPlugIn,m_dwPK_Device_EventPlugIn,m_dwPK_Device_OrbiterPlugIn,
		m_dwPK_Device_LightingPlugIn, m_dwPK_Device_ClimatePlugIn, m_dwPK_Device_MediaPlugIn, m_dwPK_Device_TelecomPlugIn, m_dwPK_Device_SecurityPlugIn,
		m_dwPK_Device_LocalAppServer;

	OrbiterData() { m_dwPK_Device_Router = m_dwPK_Device_DatagridPlugIn = m_dwPK_Device_InfraredPlugIn = m_dwPK_Device_GeneralInfoPlugIn = m_dwPK_Device_EventPlugIn = m_dwPK_Device_OrbiterPlugIn =
		m_dwPK_Device_LightingPlugIn = m_dwPK_Device_ClimatePlugIn = m_dwPK_Device_MediaPlugIn = m_dwPK_Device_TelecomPlugIn = m_dwPK_Device_SecurityPlugIn = DEVICEID_NULL; }

	virtual ~OrbiterData()
	{
		MapTextStyle::iterator it;
		for(it = m_mapTextStyle.begin(); it != m_mapTextStyle.end(); ++it)
		{
			delete (*it).second; //TextStyle *
		}
		m_mapTextStyle.clear();

		for(size_t iIndex = 0; iIndex < m_dequeLocation.size(); iIndex++)
			delete m_dequeLocation[iIndex];
		m_dequeLocation.clear();
	}

#ifdef ORBITER
	DesignObj_Orbiter *m_pDesignObj_Orbiter_MainMenu,*m_pDesignObj_Orbiter_SleepingMenu,*m_pDesignObj_Orbiter_ScreenSaveMenu;

	DesignObj_Orbiter *m_ScreenMap_Find(string sPK_DesignObj)
	{
		ScreenMap::iterator it = m_ScreenMap.find(sPK_DesignObj);
		if( it==m_ScreenMap.end() )
			it = m_ScreenMap.find(sPK_DesignObj + ".0.0");

		return it==m_ScreenMap.end() ? NULL : (*it).second;
	}
#endif

	deque<class LocationInfo *> m_dequeLocation;
	VariableMap m_mapVariable;  // our current variables
	MapTextStyle m_mapTextStyle;
	TextStyle *m_mapTextStyle_Find(int PK_Style)
	{
		MapTextStyle::iterator it = m_mapTextStyle.find(PK_Style);
		return it==m_mapTextStyle.end() ? NULL : (*it).second;
	}

	OrbiterData &operator+ (ScreenMap &i) { m_listItemToSerialize.push_back(new ItemToSerialize(SERIALIZE_DATA_TYPE_SCREEN_MAP,(void *) &i)); return (*this); }
	OrbiterData &operator+ (MapTextStyle &i) { m_listItemToSerialize.push_back(new ItemToSerialize(SERIALIZE_DATA_TYPE_STYLE_MAP,(void *) &i)); return (*this); }
	OrbiterData &operator+ (DequeLocationInfo &i) { m_listItemToSerialize.push_back(new ItemToSerialize(SERIALIZE_DATA_TYPE_LOCATIONS,(void *) &i)); return (*this); }

	void SetupSerialization(int iSC_Version)
	{
		StartSerializeList() + m_mapVariable + m_Width + m_Height + m_AnimationStyle + m_sMainMenu + m_sSleepingMenu + m_sScreenSaveMenu + m_dwPK_Users_Default + m_iLocation_Initial
			+ m_dwPK_Device_Router + m_dwPK_Device_DatagridPlugIn + m_dwPK_Device_EventPlugIn + m_dwPK_Device_InfraredPlugIn + m_dwPK_Device_GeneralInfoPlugIn + m_dwPK_Device_OrbiterPlugIn
			+ m_dwPK_Device_LightingPlugIn + m_dwPK_Device_ClimatePlugIn + m_dwPK_Device_MediaPlugIn + m_dwPK_Device_TelecomPlugIn + m_dwPK_Device_SecurityPlugIn
			+ m_dwPK_Device_LocalAppServer + m_tGenerationTime + m_mapTextString;

		(*this) + m_ScreenMap + m_mapTextStyle + m_dequeLocation; // this is serialized custom
	}
	virtual string SerializeClassClassName() { return "OrbiterData"; }
	// For our custom serialize types.

	virtual bool UnknownSerialize(ItemToSerialize *pItem,bool bWriting,char *&pDataBlock,unsigned long &iAllocatedSize,char *&pCurrentPosition) 
	{
		m_pcDataBlock=pDataBlock; m_dwAllocatedSize=iAllocatedSize; m_pcCurrentPosition=pCurrentPosition;
		if( bWriting )
		{
			switch(pItem->m_iSerializeDataType)
			{
			case SERIALIZE_DATA_TYPE_SCREEN_MAP:
				{
					Write_unsigned_long((unsigned long) m_ScreenMap.size());
					ScreenMap::iterator it;
					for(it=m_ScreenMap.begin();it!=m_ScreenMap.end();++it)
					{
						DesignObj_Data *pObj = (*it).second;
						Write_unsigned_long( pObj->m_ObjectType );
						if( !pObj->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition) )
							return false;
					}
					return true;
				}
				break;
			case SERIALIZE_DATA_TYPE_STYLE_MAP:
				{
					Write_unsigned_long((unsigned long) m_mapTextStyle.size());
					MapTextStyle::iterator it;
					for(it=m_mapTextStyle.begin();it!=m_mapTextStyle.end();++it)
					{
						if( !(*it).second->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition) )
							return false;
					}
					return true;
				}
				break;
			case SERIALIZE_DATA_TYPE_LOCATIONS:
				{
					Write_unsigned_long((unsigned long) m_dequeLocation.size());
					DequeLocationInfo::iterator it;
					for(it=m_dequeLocation.begin();it!=m_dequeLocation.end();++it)
					{
						if( !(*it)->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition) )
							return false;
					}
					return true;
				}
				break;
			};
		}
		else
		{
			switch(pItem->m_iSerializeDataType)
			{
			case SERIALIZE_DATA_TYPE_SCREEN_MAP:
				{
					unsigned long count = Read_unsigned_long();
					for(unsigned long i=0;i<count;++i)
					{
						unsigned long Type = Read_unsigned_long();
#ifdef ORBITER
						DesignObj_Orbiter *pDesignObj_Data=NULL;
						if( Type==DESIGNOBJTYPE_Datagrid_CONST )
							pDesignObj_Data = new DesignObj_DataGrid((class Orbiter *) m_pExtraSerializationData);
						else
							pDesignObj_Data = new DesignObj_Orbiter((class Orbiter *) m_pExtraSerializationData);
#else
							DesignObj_Data *pDesignObj_Data = new DesignObj_Data();
#endif
						if( !pDesignObj_Data->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition,m_pExtraSerializationData) )
							return false;
						m_ScreenMap[pDesignObj_Data->m_ObjectID]=pDesignObj_Data;
					}
					return true;
				}
				break;
			case SERIALIZE_DATA_TYPE_STYLE_MAP:
				{
					unsigned long count = Read_unsigned_long();
					for(unsigned long i=0;i<count;++i)
					{
						TextStyle *pTextStyle = new TextStyle();
						if( !pTextStyle->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition) )
							return false;
						if( pTextStyle )
							m_mapTextStyle[pTextStyle->m_iPK_Style] = pTextStyle;
					}
					return true;
				}
				break;
			case SERIALIZE_DATA_TYPE_LOCATIONS:
				{
					unsigned long count = Read_unsigned_long();
					for(unsigned long i=0;i<count;++i)
					{
						LocationInfo *pLocationInfo = new LocationInfo();
						if( !pLocationInfo->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition) )
							return false;
						m_dequeLocation.push_back(pLocationInfo);
					}
					return true;
				}
				break;
			};
		}
		return false;
	}
};

#endif
