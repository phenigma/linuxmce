#ifndef DeviceData_Impl_h
#define DeviceData_Impl_h

#include "Event_Impl.h"
#include "Message.h"
#include "DeviceData_Base.h"
#include "SerializeClass/SerializeClass.h"

#define SERIALIZE_DATA_TYPE_VECTOR_DEVICEDATA_IMPL		3000

namespace DCE
{
	typedef vector<class DeviceData_Impl *> VectDeviceData_Impl;

	class DeviceData_Impl : public DeviceData_Base
	{
	public:
		Event_Impl *m_pEvent_Impl;
		map<int, string> m_Parameters;
		string mapParameters_Find(int PK_DeviceData)
		{
			map<int,string>::iterator it = m_Parameters.find(PK_DeviceData);
			return it==m_Parameters.end() ? "" : (*it).second;
		}

		string m_sDescription;
		virtual const char *GetDeviceDescription() { return NULL; };
		VectDeviceData_Impl m_vectDeviceData_Impl_Children;  

		virtual int GetPK_DeviceList() { return 0; } ; 
		DeviceData_Impl() {}

		void SetupSerialization()
		{
			DeviceData_Base::SetupSerialization();
			StartSerializeList() + m_Parameters;
			(*this) + m_vectDeviceData_Impl_Children; // this is serialized custom
		}
		virtual string SerializeClassClassName() { return "DeviceData_Impl"; }
		// For our custom serialize types.
		DeviceData_Impl &operator+ (VectDeviceData_Impl &i) { m_listItemToSerialize.push_back(new ItemToSerialize(SERIALIZE_DATA_TYPE_VECTOR_DEVICEDATA_IMPL,(void *) &i)); return (*this); }

		virtual bool UnknownSerialize(ItemToSerialize *pItem,bool bWriting,char *&pDataBlock,unsigned long &iAllocatedSize,char *&pCurrentPosition);

		/*
		DeviceData_Impl(char *pDataBlock,unsigned long AllocatedSize,char *CurrentPosition,int x)
		: DeviceData_Base(pDataBlock,AllocatedSize,CurrentPosition,x)
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
		DeviceData_Impl *ChildDevice = CreateData(this,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition);
		m_vectDeviceData_Impl_Children.push_back(ChildDevice);
		}
		}
		*/
		DeviceData_Impl(unsigned long iPK_Device,unsigned long iPK_Installation,unsigned long iPK_DeviceTemplate,unsigned long  iPK_Device_ControlledVia, 
			unsigned long m_iPK_DeviceCategory, unsigned long iPK_Room,bool bImplementsDCE,bool bIsEmbedded,string sCommandLine,bool bIsPlugIn,string sDescription,
			string sIPAddress,string sMacAddress)
			: DeviceData_Base(iPK_Device,iPK_Installation,iPK_DeviceTemplate,iPK_Device_ControlledVia,m_iPK_DeviceCategory,iPK_Room,
			bImplementsDCE,bIsEmbedded,sCommandLine,bIsPlugIn,sIPAddress,sMacAddress)
		{
			m_sDescription=sDescription;
		}

		virtual ~DeviceData_Impl()
		{
		}
		/*
		void ConvertToBinary(char *pDataBlock,unsigned long AllocatedSize,char *CurrentPosition)
		{
		if( !pDataBlock )
		{
		StartWriting();
		pDataBlock=m_pcDataBlock;
		AllocatedSize=m_dwAllocatedSize;
		CurrentPosition=m_pcCurrentPosition;

		}

		DeviceData_Base::ConvertToBinary(pDataBlock,AllocatedSize,CurrentPosition);

		Write_string(m_sDescription);

		Write_unsigned_long((unsigned long) m_Parameters.size());
		map<long, string>::iterator itParms;
		for(itParms = m_Parameters.begin();itParms != m_Parameters.end();++itParms)
		{
		Write_unsigned_long( (*itParms).first );
		Write_string( (*itParms).second );
		}

		Write_unsigned_long((unsigned long) m_vectDeviceData_Impl_Children.size());
		for(unsigned long lC=0;lC<m_vectDeviceData_Impl_Children.size();++lC)
		{
		DeviceData_Impl *pChildDevice = m_vectDeviceData_Impl_Children[lC];
		pChildDevice->ConvertToBinary(m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition);
		m_pcCurrentPosition = pChildDevice->m_pcCurrentPosition;
		m_dwAllocatedSize = pChildDevice->m_dwAllocatedSize;
		}
		}
		*/
		// TODO -- This should be pure virtual as below.  for some reason MS won't link when it's implemented in the auto-generated
		// derived classes.  Got to figure out why.
		//virtual class DeviceData_Impl *CreateData(DeviceData_Impl *Parent,char *pDataBlock,unsigned long AllocatedSize,char *CurrentPosition)=0;
		virtual class DeviceData_Impl *CreateData(DeviceData_Impl *Parent,char *pDataBlock,unsigned long AllocatedSize,char *CurrentPosition) { return NULL; }

		virtual void SetParm(int ParmNum, const char *szParm) 
		{
			m_Parameters[ParmNum]=szParm;
		}
	};
}

#endif

