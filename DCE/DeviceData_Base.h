#ifndef DeviceData_Base_h
#define DeviceData_Base_h

#include "SerializeClass/SerializeClass.h"

/*

This is the lowest level base class that contains primitive information
about a Device.  Every DCE device will have a map pointing to this basic
information.  The higher level classes which contain more information needed
for the DCE Implementation: DeviceData_Impl and for the router:
DeviceData_Router are derived from this base class.

*/

#define SERIALIZE_DATA_TYPE_MAP_DCEDEVICEDATA_BASE		2000
#define SERIALIZE_DATA_TYPE_MAP_DCECATEGORY				2001

namespace DCE
{
	class DeviceCategory : public SerializeClass
	{
	public:
		int m_iPK_DeviceCategory,m_iPK_DeviceCategory_Parent;
		string m_sDescription;
		class DeviceCategory *m_pDeviceCategory_Parent;

		DeviceCategory(int iPK_DeviceCategory,int iPK_DeviceCategory_Parent,string sDescription)
		{
			m_iPK_DeviceCategory=iPK_DeviceCategory;
			m_iPK_DeviceCategory_Parent=iPK_DeviceCategory_Parent;
			m_sDescription=sDescription;
			m_pDeviceCategory_Parent = NULL;
		}
		DeviceCategory() { m_pDeviceCategory_Parent=NULL; } // For de-serializing

		void SetupSerialization()
		{
			StartSerializeList() + m_iPK_DeviceCategory + m_iPK_DeviceCategory_Parent + m_sDescription;
		}
		virtual string SerializeClassClassName() { return "DeviceCategory"; }
	};

	typedef map<int,class Command_Base *> Map_Command_Base;
	typedef map<int,class DeviceData_Base *> Map_DeviceData_Base;
	typedef map<int,class DeviceCategory *> Map_DeviceCategory;

	// A class to contain all the devices, that can be serialized and de-serialized
	class AllDevices : public SerializeClass
	{
	public:
		Map_DeviceData_Base m_mapDeviceData_Base;
		DeviceData_Base *m_mapDeviceData_Base_Find(int PK_Device) {
			map<int,class DeviceData_Base *>::iterator it = m_mapDeviceData_Base.find(PK_Device);
			return it==m_mapDeviceData_Base.end() ? NULL : (*it).second;
		}
		Map_DeviceCategory m_mapDeviceCategory;
		DeviceCategory *m_mapDeviceCategory_Find(int PK_DeviceCategory) {
			Map_DeviceCategory::iterator it = m_mapDeviceCategory.find(PK_DeviceCategory);
			return it==m_mapDeviceCategory.end() ? NULL : (*it).second;
		}

		void SetupSerialization()
		{
			(*this) + m_mapDeviceData_Base + m_mapDeviceCategory; // this is serialized custom
		}
		virtual string SerializeClassClassName() { return "AllDevices"; }

		// For our custom serialize types.
		AllDevices &operator+ (Map_DeviceData_Base &i) { m_listItemToSerialize.push_back(new ItemToSerialize(SERIALIZE_DATA_TYPE_MAP_DCEDEVICEDATA_BASE,(void *) &i)); return (*this); }
		AllDevices &operator+ (Map_DeviceCategory &i) { m_listItemToSerialize.push_back(new ItemToSerialize(SERIALIZE_DATA_TYPE_MAP_DCECATEGORY,(void *) &i)); return (*this); }

		virtual bool UnknownSerialize(ItemToSerialize *pItem,bool bWriting,char *&pDataBlock,unsigned long &iAllocatedSize,char *&pCurrentPosition);

		// We'll override this so we can match up the categories after de-serializing
		virtual bool Serialize(bool bWriting,char *&pDataBlock,unsigned long &iAllocatedSize,char *&pCurrentPosition,void *pExtraSerializationData=NULL);
	};

	class DeviceData_Base : public SerializeClass
	{
	public:
		unsigned long m_iPK_Device,m_iPK_Installation,m_iPK_DeviceTemplate,m_iPK_Device_ControlledVia;
		unsigned long m_iPK_DeviceCategory,m_iPK_Room;
		bool m_bImplementsDCE,m_bIsEmbedded,m_bIsPlugIn;
		string m_sCommandLine,m_sIPAddress,m_sMacAddress;

		map<int,string> m_mapCommands;
		AllDevices m_AllDevices;  // All the devices in the system
		DeviceCategory *m_pDeviceCategory;

		// Whatever creates this will need to set these pointers
		class DeviceData_Base *m_pDevice_ControlledVia;
		/*
		// When this class is being de-serialized the following constructor will be called
		DeviceData_Base(char *pDataBlock,unsigned long AllocatedSize,char *CurrentPosition,int x)
		{
		m_pDeviceCategory=NULL;
		m_pDevice_ControlledVia=NULL;
		ResumeReadWrite(pDataBlock,AllocatedSize,CurrentPosition);

		m_iPK_Device = Read_unsigned_long();
		m_iPK_Installation = Read_unsigned_long();
		m_iPK_DeviceTemplate = Read_unsigned_long();
		m_iPK_Device_ControlledVia = Read_unsigned_long();
		m_iPK_DeviceCategory=Read_unsigned_long();
		m_iPK_Room=Read_unsigned_long();

		m_bImplementsDCE = (Read_unsigned_char()==1);
		m_iNumberCommands = Read_unsigned_long();
		if( m_iNumberCommands )
		{
		m_piCommands = new unsigned long[m_iNumberCommands];
		for(unsigned long iCommands=0;iCommands<m_iNumberCommands;++iCommands)
		m_piCommands[iCommands] = Read_unsigned_long();
		}
		else
		m_piCommands=NULL;
		}
		*/
		void SetupSerialization()
		{
			StartSerializeList() + m_bImplementsDCE + m_iPK_Device + m_iPK_Installation + m_iPK_DeviceTemplate + m_iPK_Device_ControlledVia +
				m_iPK_DeviceCategory + m_iPK_Room + m_bIsPlugIn + m_bIsEmbedded + m_sCommandLine + m_mapCommands + m_sIPAddress + m_sMacAddress;
		}

		// Call this contstructor to create it dynamically
		DeviceData_Base(unsigned long  iPK_Device,unsigned long  iPK_Installation,unsigned long  iPK_DeviceTemplate,unsigned long  iPK_Device_ControlledVia, 
			unsigned long iPK_DeviceCategory, unsigned long iPK_Room,bool bImplementsDCE,bool bIsEmbedded,string sCommandLine,bool bIsPlugIn,string sIPAddress,string sMacAddress)
		{ 
			m_pDeviceCategory=NULL;
			m_pDevice_ControlledVia=NULL;
			m_iPK_Device=iPK_Device;
			m_iPK_Installation=iPK_Installation;
			m_iPK_DeviceTemplate=iPK_DeviceTemplate;
			m_iPK_Device_ControlledVia=iPK_Device_ControlledVia;
			m_iPK_DeviceCategory=iPK_DeviceCategory;
			m_iPK_Room=iPK_Room;
			m_bImplementsDCE=bImplementsDCE;
			m_bIsEmbedded=bIsEmbedded;
			m_sCommandLine=sCommandLine;
			m_bIsPlugIn=bIsPlugIn;
			m_sIPAddress=sIPAddress;
			m_sMacAddress=sMacAddress;
		}

		DeviceData_Base()
		{
			m_pDeviceCategory=NULL;
			m_pDevice_ControlledVia=NULL;
			m_bIsPlugIn=false;
		}

		virtual ~DeviceData_Base()
		{
		};
		/*
		void ConvertToBinary(char *pDataBlock,unsigned long AllocatedSize,char *CurrentPosition)
		{
		ResumeReadWrite(pDataBlock,AllocatedSize,CurrentPosition);

		Write_unsigned_long(m_iPK_Device);
		Write_unsigned_long(m_iPK_Installation);
		Write_unsigned_long(m_iPK_DeviceTemplate);
		Write_unsigned_long(m_iPK_Device_ControlledVia);
		Write_unsigned_long(m_iPK_DeviceCategory);
		Write_unsigned_long(m_iPK_Room);
		Write_unsigned_char((unsigned char) m_bImplementsDCE);
		Write_unsigned_long(m_iNumberCommands);
		for(unsigned long iCommands=0;iCommands<m_iNumberCommands;++iCommands)
		Write_unsigned_long(m_piCommands[iCommands]);
		}
		*/

		bool WithinCategory(int PK_DeviceCategory,DeviceCategory *pStarting=NULL)
		{
			if( !pStarting )
				pStarting = m_pDeviceCategory;
			if( !pStarting )
				return false;

			if( pStarting->m_iPK_DeviceCategory==PK_DeviceCategory )
				return true;
			if( !pStarting->m_pDeviceCategory_Parent )
				return false;
			return WithinCategory(PK_DeviceCategory,pStarting->m_pDeviceCategory_Parent);
		}

		bool WithinCategory(DeviceCategory *pCategory,DeviceCategory *pStarting=NULL)
		{
			if( !pStarting )
				pStarting = m_pDeviceCategory;
			if( !pStarting )
				return false;

			if( pStarting->m_iPK_DeviceCategory==pCategory->m_iPK_DeviceCategory )
				return true;
			if( !pStarting->m_pDeviceCategory_Parent )
				return false;
			return WithinCategory(pCategory,pStarting->m_pDeviceCategory_Parent);
		}

		bool SupportsCommand(int PK_Command)
		{
			return m_mapCommands.find(PK_Command)!=m_mapCommands.end();
		}
	};
}

#endif

