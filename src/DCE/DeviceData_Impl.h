/**
 *
 * @file DeviceData_Impl.h
 * @brief header file for the DeviceData_Impl classe
 * @author
 * @todo notcommented
 *
 */
 

#ifndef DEVICEDATA_IMPL_h
#define DEVICEDATA_IMPL_h

#include "Event_Impl.h"
#include "Message.h"
#include "DeviceData_Base.h"
#include "SerializeClass/SerializeClass.h"

#define SERIALIZE_DATA_TYPE_VECTOR_DEVICEDATA_IMPL		3000

namespace DCE
{

	typedef vector<class DeviceData_Impl *> VectDeviceData_Impl;

	/**
	 * @brief containes the actual device data
	 * from this we derive the class containing data for each specific device
	 * @see DeviceData_Base class
	 */
	class DeviceData_Impl : public DeviceData_Base
	{
	
	public:
		Event_Impl *m_pEvent_Impl; /** < pointer to an event implementation assicieted with the current device @todo ask */
		map<int, string> m_mapParameters; /** < integer-keyed map with the parameters this device has @todo ask - is string=paramvalue? */
		
		VectDeviceData_Impl m_vectDeviceData_Impl_Children; /** < vector containing the child devices  */
	
		/** @todo check comment */	
		/*
		DCEDeviceData_Impl(char *pDataBlock,unsigned long AllocatedSize,char *CurrentPosition,int x)
			: DCEDeviceData_Base(pDataBlock,AllocatedSize,CurrentPosition,x)
		{
			Read_string(m_sDescription);
	
			unsigned long NumParms = Read_unsigned_long();
			for(unsigned long lP=0;lP<NumParms;++lP)
			{
				int ParmNum = Read_unsigned_long();
				string Value;
				Read_string(Value);
				SetParm(ParmNum,Value.c_str());
			}
	
			unsigned long NumChildDevice = Read_unsigned_long();
			for(unsigned long lC=0;lC<NumChildDevice;++lC)
			{
				DCEDeviceData_Impl *ChildDevice = CreateData(this,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition);
				m_vectDCEDeviceData_Impl_Children.push_back(ChildDevice);
			}
		}
		*/

		/**
		 * @brief default constructor
		 */
		DeviceData_Impl() {}
		
		/**
		 * @brief constructor, it just assigns values to the member data (and calls the base class constructor)
		 */
		DeviceData_Impl( unsigned long dwPK_Device, unsigned long dwPK_Installation, unsigned long dwPK_Device_Template, unsigned long  dwPK_Device_Controlled_Via, 
			unsigned long m_dwPK_DeviceCategory, unsigned long dwPK_Room, bool bImplementsDCE, bool bIsEmbedded, string sCommandLine, bool bIsPlugIn, string sDescription,
			string sIPAddress, string sMacAddress, bool bInheritsMacFromPC )
			: DeviceData_Base( dwPK_Device, dwPK_Installation, dwPK_Device_Template, dwPK_Device_Controlled_Via, m_dwPK_DeviceCategory, dwPK_Room,
			bImplementsDCE, bIsEmbedded, sCommandLine, bIsPlugIn, sDescription, sIPAddress, sMacAddress, bInheritsMacFromPC )
		{
		}

		virtual ~DeviceData_Impl()
		{
			for(size_t iIndex = 0; iIndex < m_vectDeviceData_Impl_Children.size(); iIndex++)
				delete m_vectDeviceData_Impl_Children[iIndex];
		}
		
		// TODO -- This should be pure virtual as below.  for some reason MS won't link when it's implemented in the auto-generated
		// derived classes.  Got to figure out why.
		//virtual class DeviceData_Impl *CreateData(DeviceData_Impl *Parent,char *pDataBlock,unsigned long AllocatedSize,char *CurrentPosition)=0;
		
		/**
		 * @brief returnes the parameter value for the specified parameter
		 */
		string mapParameters_Find( int iPK_DeviceData )
		{
			map<int,string>::iterator it = m_mapParameters.find( iPK_DeviceData );
			return it == m_mapParameters.end() ? "" : (*it).second;
		}

		
		/**
		 * @brief should be overriden to return the description for the device as a char*
		 */
		virtual const char *GetDeviceDescription() { return NULL; };
		
		/**
		 * @brief setups serialization for the class member data
		 * @warning the m_vectDeviceData_Impl_Children vector is serialized custom
		 * @see the overloads for the + operator
		 */
		void SetupSerialization(int iSC_Version)
		{
			DeviceData_Base::SetupSerialization(iSC_Version);
			StartSerializeList() + m_mapParameters;
			(*this) + m_vectDeviceData_Impl_Children; // this is serialized custom
		}
		
		/**
		 * @returns "DeviceData_Impl" so that the SerializeClass knows what to serialize
		 */		
		virtual string SerializeClassClassName() { return "DeviceData_Impl"; }
		
		/** overloding + for our custom serialize types */
		
		/**
		 * @brief adds a new ItemToSerialize to the m_listItemToSerialize list (the m_vectDeviceData_Impl_Children vector)
		 */
		DeviceData_Impl &operator+ ( VectDeviceData_Impl &i ) { m_listItemToSerialize.push_back( new ItemToSerialize( SERIALIZE_DATA_TYPE_VECTOR_DEVICEDATA_IMPL,(void *) &i ) ); return (*this); }

		/**
		 * @brief overrides the SerializeClass::UnknownSerialize method
		 * @todo ask
		 */
		virtual bool UnknownSerialize( ItemToSerialize *pItem, bool bWriting, char *&pcDataBlock, unsigned long &dwAllocatedSize, char *&pcCurrentPosition );

		/**
		 * @brief should create a new DeviceData_Impl object based on the parameters
		 * @warning this must be implemented in a derived class
		 * @todo ask for more specific
		 * @todo This should be pure virtual as below.  for some reason MS won't link when it's implemented in the auto-generated derived classes.  Got to figure out why.
		 * virtual class DeviceData_Impl *CreateData(DeviceData_Impl *Parent,char *pDataBlock,unsigned long AllocatedSize,char CurrentPosition)=0;
		 */
		virtual class DeviceData_Impl *CreateData( DeviceData_Impl *pParent, char *pcDataBlock, unsigned long dwAllocatedSize, char *pcCurrentPosition) { return NULL; }

		/** 
		 * @brief sets the specified value for the specified parameter
		 */
		virtual void SetParm( unsigned long dwParmNum, const char *pcParm ) 
		{
			m_mapParameters[dwParmNum] = pcParm;
		}
	};
}

#endif

