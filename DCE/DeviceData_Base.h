/**
 *
 * @file DeviceData_Base.h
 * @brief header file for the DeviceCategory, DeviceData_Base and AllDevices classes
 * @author
 * @todo notcommented
 *
 */
 
 
#ifndef DEVICEDATA_BASE_H
#define DEVICEDATA_BASE_H

#include "SerializeClass/SerializeClass.h"

#define SERIALIZE_DATA_TYPE_MAP_DCEDEVICEDATA_BASE			2000
#define SERIALIZE_DATA_TYPE_MAP_DCECATEGORY				2001

namespace DCE
{


	typedef map<int,class Command_Base *> Map_Command_Base;
	typedef map<int,class DeviceData_Base *> Map_DeviceData_Base;
	typedef map<int,class DeviceCategory *> Map_DeviceCategory;

	/**
	 * @brief this class contains information about a device category
	 */
	class DeviceCategory : public SerializeClass
	{
	
	public:
	
		unsigned long m_dwPK_DeviceCategory; /** < an identifier for the device category */
		unsigned long m_dwPK_DeviceCategory_Parent; /** < an identifier for the parent category for this device category */
		string m_sDescription; /** < a short description of the device category */
		
		class DeviceCategory *m_pDeviceCategory_Parent; /** < pointer to the parent category */

		/**
		 * @brief constructor the assignes values to the member data
		 */
		DeviceCategory( unsigned long dwPK_DeviceCategory, unsigned long dwPK_DeviceCategory_Parent, string sDescription )
		{
			m_dwPK_DeviceCategory = dwPK_DeviceCategory;
			m_dwPK_DeviceCategory_Parent = dwPK_DeviceCategory_Parent;
			m_sDescription = sDescription;
			m_pDeviceCategory_Parent = NULL;
		}
		
		/**
		 * @brief constructor that assignes NULL to the pointer member data (for de-serializing)
		 */
		DeviceCategory() { m_pDeviceCategory_Parent = NULL; }

		/**
		 * @brief setups serialization for the class data
		 */
		void SetupSerialization()
		{
			StartSerializeList() + m_dwPK_DeviceCategory + m_dwPK_DeviceCategory_Parent + m_sDescription;
		}
		
		/**
		 * @brief returns "DeviceCategory" so that the SerializeClass knows what it has to serialize
		 */
		virtual string SerializeClassClassName() { return "DeviceCategory"; }
	};
	
		
	/**
	 * @brief a class to contain all the devices, that can be serialized and de-serialized
	 */
	class AllDevices : public SerializeClass
	{
	
	public:
	
		Map_DeviceData_Base m_mapDeviceData_Base; /** < a map with all the devices */
		Map_DeviceCategory m_mapDeviceCategory; /** < a map with all the device categories */
		
		/**
		 * @brief finds the DeviceData_Base item associated with the specified device
		 */
		DeviceData_Base *m_mapDeviceData_Base_Find( unsigned long dwPK_Device ) {
			map<int,class DeviceData_Base *>::iterator it = m_mapDeviceData_Base.find( dwPK_Device );
			return it == m_mapDeviceData_Base.end() ? NULL : (*it).second;
		}
	
		/**
		 * @brief finds the DeviceCategory item associated with the specified device category id
		 */
		DeviceCategory *m_mapDeviceCategory_Find( unsigned long dwPK_DeviceCategory ) {
			Map_DeviceCategory::iterator it = m_mapDeviceCategory.find( dwPK_DeviceCategory );
			return it == m_mapDeviceCategory.end() ? NULL : (*it).second;
		}

		/**
		 * @brief setups serialization for the class member data
		 * @warning the maps are serialized custom
		 * @see the overloads for the + operator
		 */
		void SetupSerialization()
		{
			(*this) + m_mapDeviceData_Base + m_mapDeviceCategory; // this is serialized custom
		}
		
		/**
		 * @returns "AllDevices" so that the SerializeClass knows what to serialize
		 */
		virtual string SerializeClassClassName() { return "AllDevices"; }

		/** overloding + for our custom serialize types */
		
		/**
		 * @brief adds a new ItemToSerialize to the m_listItemToSerialize list (the DeviceData_Base map)
		 */
		AllDevices &operator+ ( Map_DeviceData_Base &i ) { m_listItemToSerialize.push_back( new ItemToSerialize( SERIALIZE_DATA_TYPE_MAP_DCEDEVICEDATA_BASE, (void *) &i) ); return (*this); }
		
		/**
		 * @brief adds a new ItemToSerialize to the m_listItemToSerialize list (the DeviceCategory map)
		 */
		AllDevices &operator+ ( Map_DeviceCategory &i ) { m_listItemToSerialize.push_back( new ItemToSerialize( SERIALIZE_DATA_TYPE_MAP_DCECATEGORY, (void *) &i ) ); return (*this); }

		/**
		 * @brief overrides the SerializeClass::UnknownSerialize method
		 * @todo ask
		 */
		virtual bool UnknownSerialize( ItemToSerialize *pItem, bool bWriting, char *&pcDataBlock, unsigned long &dwAllocatedSize, char *&pcCurrentPosition );

		/**
		 * @brief
		 * we'll override this so we can match up the categories after de-serializing
		 */
		virtual bool Serialize( bool bWriting,char *&pcDataBlock, unsigned long &dwAllocatedSize, char *&pcCurrentPosition, void *pExtraSerializationData=NULL );
	};
	
	
	/**
	 * @brief this is the lowest level base class that contains primitive information about a Device
	 * Every DCE device will have a map pointing to this basic
	 * information.  The higher level classes which contain more information needed
	 * for the DCE Implementation: DeviceData_Impl and for the router:
	 * DeviceData_Router are derived from this base class.
	 */
	class DeviceData_Base : public SerializeClass
	{
	
	public:
	
		/** fields that corespond to primary keys */
		
		unsigned long m_dwPK_Device; /** < the ID of the device */
		unsigned long m_dwPK_Installation; /** < the installation associated with the device @todo ask*/
		unsigned long m_dwPK_DeviceTemplate; /** < the identifier for the template used to build the device */
		unsigned long m_dwPK_Device_ControlledVia; /** < the specifies how the device is controlled @todo ask */
		unsigned long m_dwPK_DeviceCategory; /** < the device category */
		unsigned long m_dwPK_Room; /** < identifies the room where the device resides */
		
		/** flags */
		
		bool m_bImplementsDCE; /** < @todo ask */
		bool m_bIsEmbedded; /** < specifies if the device is embedded */
		bool m_bIsPlugIn; /** < specifies if the device is a plugin */
		
		/** other info */
		
		string m_sCommandLine; /** < the command line for the device @todo ask */
		string m_sIPAddress; /** < the IP address for the device */
		string m_sMacAddress; /** < the MAC address for the device */

		map<int,string> m_mapCommands; /** < map of commands for the device */
		AllDevices m_AllDevices;  /** < all the devices in the system */
		DeviceCategory *m_pDeviceCategory; /** < pointer to a DeviceCategory object to witch this device belongs */

		/**
		 * @brief pointer to devices controlling this one
		 * @todo ask
		 * @warning whatever creates this will need to set these pointers
		 */
		class DeviceData_Base *m_pDevice_ControlledVia;
		
		/**
		 * @brief default constructor, assignes default values to the member data
		 */
		DeviceData_Base()
		{
			m_pDeviceCategory = NULL;
			m_pDevice_ControlledVia = NULL;
			m_bIsPlugIn = false;
		}
		
		/**
		 * @brief assignes specified values to the member data; all parameters are required
		 * Call this contstructor to create it dynamically
		 */
		DeviceData_Base( unsigned long dwPK_Device, unsigned long dwPK_Installation, unsigned long dwPK_DeviceTemplate, unsigned long dwPK_Device_ControlledVia,
				unsigned long dwPK_DeviceCategory, unsigned long dwPK_Room, bool bImplementsDCE, bool bIsEmbedded, string sCommandLine, bool bIsPlugIn, string sIPAddress, string sMacAddress )
		{ 
			m_pDeviceCategory = NULL;
			m_pDevice_ControlledVia = NULL;
			m_dwPK_Device = dwPK_Device;
			m_dwPK_Installation = dwPK_Installation;
			m_dwPK_DeviceTemplate = dwPK_DeviceTemplate;
			m_dwPK_Device_ControlledVia = dwPK_Device_ControlledVia;
			m_dwPK_DeviceCategory = dwPK_DeviceCategory;
			m_dwPK_Room = dwPK_Room;
			m_bImplementsDCE = bImplementsDCE;
			m_bIsEmbedded = bIsEmbedded;
			m_sCommandLine = sCommandLine;
			m_bIsPlugIn = bIsPlugIn;
			m_sIPAddress = sIPAddress;
			m_sMacAddress = sMacAddress;
		}

		/**
		 * @brief setups serialization for the class data
		 */
		void SetupSerialization()
		{
			StartSerializeList() + m_bImplementsDCE + m_dwPK_Device + m_dwPK_Installation + m_dwPK_DeviceTemplate + m_dwPK_Device_ControlledVia +
				m_dwPK_DeviceCategory + m_dwPK_Room + m_bIsPlugIn + m_bIsEmbedded + m_sCommandLine + m_mapCommands + m_sIPAddress + m_sMacAddress;
		}

		/**
		 * @brief searches to see if the device belongs to the specified category
		 * if it isn't the device exact category it goes up to the device parents
		 */
		bool WithinCategory( unsigned long dwPK_DeviceCategory, DeviceCategory *pStarting=NULL )
		{
			if ( !pStarting ) // no device category to start with supplied
				pStarting = m_pDeviceCategory;
				
			if ( !pStarting ) // the m_pDeviceCategory member data is not assigned
				return false;

			if ( pStarting->m_dwPK_DeviceCategory == dwPK_DeviceCategory ) // the device belongs to the specified category
				return true;
				
			if ( !pStarting->m_pDeviceCategory_Parent ) // the current device category is top-level (has no parent)
				return false;
				
			return WithinCategory( dwPK_DeviceCategory, pStarting->m_pDeviceCategory_Parent ); // call it recursivelly for the parent
		}

		/**
		 * @brief searches to see if the device belongs to the specified category
		 * same as WithinCategory( unsigned long ... ) just that the device category is specified through a pointer to a device category
		 * @see the other WithinCategory method
		 */
		bool WithinCategory( DeviceCategory *pCategory, DeviceCategory *pStarting=NULL )
		{
			if( !pStarting ) // no device category to start with supplied
				pStarting = m_pDeviceCategory;
				
			if( !pStarting ) // the m_pDeviceCategory member data is not assigned
				return false;

			if( pStarting->m_dwPK_DeviceCategory == pCategory->m_dwPK_DeviceCategory ) // the device belongs to the specified category
				return true;
				
			if( !pStarting->m_pDeviceCategory_Parent ) // the current device category is top-level (has no parent)
				return false;
				
			return WithinCategory( pCategory, pStarting->m_pDeviceCategory_Parent ); // call it recursivelly for the parent
		}

		/**
		 * @brief checks if the specified command is supported by the device
		 */
		bool SupportsCommand( unsigned long dwPK_Command )
		{
			return m_mapCommands.find( dwPK_Command ) != m_mapCommands.end();
		}
	};
}

#endif

