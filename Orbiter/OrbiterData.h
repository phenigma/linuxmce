#ifndef OrbiterData_h
#define OrbiterData_h

#include "pluto_main/Define_DesignObjType.h"
#include "PlutoUtils/ShapesColors.h"
#include "TextStyle.h"
#include "Message.h"

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
	int m_iPK_Device_MediaDirector,m_iPK_Device_DiscDrive,m_iPK_Device_AppServer,m_iPK_Device_MediaBurner,m_iPK_Device_VideoConf,m_iPK_Device_CamcorderCapt,m_iPK_Device_Orbiter,m_iPK_Device_WebBrowser; 
	string Description;
	class Row_Icon *drIcon; // Not serialized, just a temporary pointer for the generator

	LocationInfo() { m_iPK_Device_MediaDirector = m_iPK_Device_DiscDrive = m_iPK_Device_AppServer = m_iPK_Device_MediaBurner = m_iPK_Device_VideoConf = m_iPK_Device_CamcorderCapt = m_iPK_Device_Orbiter = m_iPK_Device_WebBrowser = DEVICEID_NULL; }
	void SetupSerialization()
	{
		StartSerializeList() + PK_Room + PK_EntertainArea + iLocation + Description +
			m_iPK_Device_MediaDirector + m_iPK_Device_DiscDrive + m_iPK_Device_AppServer + m_iPK_Device_MediaBurner + m_iPK_Device_VideoConf + 
			m_iPK_Device_CamcorderCapt + m_iPK_Device_Orbiter + m_iPK_Device_WebBrowser;
	}
	virtual string SerializeClassClassName() { return "LocationInfo"; }
};

typedef list<LocationInfo *> ListLocationInfo;


class OrbiterData : public SerializeClass
{
public:
	int m_Width, m_Height, m_AnimationStyle, m_iPK_Users, m_iLocation_Initial;
	string m_sMainMenu,m_sSleepingMenu,m_sScreenSaveMenu;
	ScreenMap m_ScreenMap;

	// Some global devices
	int m_iPK_Device_Router,m_iPK_Device_DatagridPlugIn,m_iPK_Device_GeneralInfoPlugIn,m_iPK_Device_OrbiterPlugIn,
		m_iPK_Device_LightingPlugIn, m_iPK_Device_ClimatePlugIn, m_iPK_Device_MediaPlugIn, m_iPK_Device_TelecomPlugIn, m_iPK_Device_SecurityPlugIn;

	OrbiterData() { m_iPK_Device_Router = m_iPK_Device_DatagridPlugIn = m_iPK_Device_GeneralInfoPlugIn = m_iPK_Device_OrbiterPlugIn =
		m_iPK_Device_LightingPlugIn = m_iPK_Device_ClimatePlugIn = m_iPK_Device_MediaPlugIn = m_iPK_Device_TelecomPlugIn = m_iPK_Device_SecurityPlugIn = DEVICEID_NULL; }

#ifdef ORBITER
	DesignObj_Orbiter *m_pDesignObj_Orbiter_MainMenu,*m_pDesignObj_Orbiter_SleepingMenu,*m_pDesignObj_Orbiter_ScreenSaveMenu;

	DesignObj_Orbiter *m_ScreenMap_Find(string sPK_DesignObj)
	{
		ScreenMap::iterator it = m_ScreenMap.find(sPK_DesignObj);
		return it==m_ScreenMap.end() ? NULL : (*it).second;
	}
#endif

	list<class LocationInfo *> m_alLocations;
	VariableMap m_mapVariable;  // our current variables
	MapTextStyle m_mapTextStyle;
	TextStyle *m_mapTextStyle_Find(int PK_Style)
	{
		MapTextStyle::iterator it = m_mapTextStyle.find(PK_Style);
		return it==m_mapTextStyle.end() ? NULL : (*it).second;
	}

	OrbiterData &operator+ (ScreenMap &i) { m_listItemToSerialize.push_back(new ItemToSerialize(SERIALIZE_DATA_TYPE_SCREEN_MAP,(void *) &i)); return (*this); }
	OrbiterData &operator+ (MapTextStyle &i) { m_listItemToSerialize.push_back(new ItemToSerialize(SERIALIZE_DATA_TYPE_STYLE_MAP,(void *) &i)); return (*this); }
	OrbiterData &operator+ (ListLocationInfo &i) { m_listItemToSerialize.push_back(new ItemToSerialize(SERIALIZE_DATA_TYPE_LOCATIONS,(void *) &i)); return (*this); }

	void SetupSerialization()
	{
		StartSerializeList() + m_mapVariable + m_Width + m_Height + m_AnimationStyle + m_sMainMenu + m_sSleepingMenu + m_sScreenSaveMenu + m_iPK_Users + m_iLocation_Initial
			+ m_iPK_Device_Router + m_iPK_Device_DatagridPlugIn + m_iPK_Device_GeneralInfoPlugIn + m_iPK_Device_OrbiterPlugIn
			+ m_iPK_Device_LightingPlugIn + m_iPK_Device_ClimatePlugIn + m_iPK_Device_MediaPlugIn + m_iPK_Device_TelecomPlugIn + m_iPK_Device_SecurityPlugIn;

		(*this) + m_ScreenMap + m_mapTextStyle + m_alLocations; // this is serialized custom
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
					Write_unsigned_long((unsigned long) m_alLocations.size());
					ListLocationInfo::iterator it;
					for(it=m_alLocations.begin();it!=m_alLocations.end();++it)
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
						m_alLocations.push_back(pLocationInfo);
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
