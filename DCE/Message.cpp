#include "PlutoUtils/CommonIncludes.h"	
#include "Message.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"

using namespace DCE;

Message::Message()
{
	Clear();
}

Message::~Message()
{
	Clear();
}

Message::Message(Message *pMessage_Clone)
{
	Clear();

	m_DeviceIDFrom=pMessage_Clone->m_DeviceIDFrom;
	m_DeviceIDTo=pMessage_Clone->m_DeviceIDTo;
	m_DeviceGroupIDTo=pMessage_Clone->m_DeviceGroupIDTo;
	m_ID=pMessage_Clone->m_ID;
	m_Priority=pMessage_Clone->m_Priority;
	m_MessageType=pMessage_Clone->m_MessageType;
	m_DeviceCategoryTo=pMessage_Clone->m_DeviceCategoryTo;
	m_MasterDevice=pMessage_Clone->m_MasterDevice;
	m_bIncludeChildren=pMessage_Clone->m_bIncludeChildren;
	m_bRelativeToSender=pMessage_Clone->m_bRelativeToSender;
	m_bRespondedToMessage=pMessage_Clone->m_bRespondedToMessage;
	m_eBroadcastLevel=pMessage_Clone->m_eBroadcastLevel;
	m_eExpectedResponse=pMessage_Clone->m_eExpectedResponse;
	m_DeviceListTo=pMessage_Clone->m_DeviceListTo;

	map<long, string>::iterator itParms;
	for(itParms=pMessage_Clone->m_Parameters.begin();itParms!=pMessage_Clone->m_Parameters.end();++itParms)
	{
		m_Parameters[(*itParms).first]=(*itParms).second;
	}

	map<long, char *>::iterator itDataParms;
	map<long, unsigned long>::iterator itDataLength;
	for(itDataLength=pMessage_Clone->m_DataLengths.begin();itDataLength!=pMessage_Clone->m_DataLengths.end();++itDataLength)
	{
		itDataParms = pMessage_Clone->m_DataParameters.find( (*itDataLength).first );
		if( itDataParms != pMessage_Clone->m_DataParameters.end() )
		{
			m_DataLengths[(*itDataLength).first] = (*itDataLength).second;
			char *Buffer = new char[(*itDataLength).second];
			memcpy(Buffer,(*itDataParms).second,(*itDataLength).second);
			m_DataParameters[(*itDataLength).first] = Buffer;
		}
	}

	vector<class Message *>::iterator itExtras;
	for(itExtras=pMessage_Clone->m_vectExtraMessages.begin();
		itExtras!=pMessage_Clone->m_vectExtraMessages.end();++itExtras)
	{
		Message *pMessage_Child_Clone = *itExtras;
		if( pMessage_Child_Clone )  // It could be NULL if this multi-part message was already broken into pieces because it had separate destinations
		{
			Message *pMessage_Child = new Message(pMessage_Child_Clone);
			m_vectExtraMessages.push_back(pMessage_Child);
		}
	}
}

Message::Message(long DeviceIDFrom, long DeviceIDTo, long Priority, long MessageType, long ID, unsigned long ParameterCount, ...)
{
	Clear();
	m_DeviceIDFrom = DeviceIDFrom;
	m_DeviceIDTo = DeviceIDTo;
	m_ID = ID;
	m_MessageType = MessageType;
	m_Priority = Priority;
	
	va_list marker;
	va_start( marker, ParameterCount );
	for(unsigned long i=0;i<ParameterCount;i++)
	{
		long Key = va_arg(marker, unsigned long );
		if (Key >= 0)
		{
			m_Parameters[Key] = string(va_arg(marker, char* ));
		}
		else
		{
			char *pData = va_arg(marker, char* );
			unsigned long Size = va_arg(marker, unsigned long );
			m_DataLengths[-Key]=Size;
			char *inData = new char[Size];
			memcpy(inData, pData, Size);
			m_DataParameters[-Key]=inData;
		}
	}
	va_end( marker );              
}

// This allows for a comma-delimited list of devices to send the command to
Message::Message(long DeviceIDFrom, string DeviceIDTo, long Priority, long MessageType, long ID, unsigned long ParameterCount, ...)
{
	Clear();
	m_DeviceIDFrom = DeviceIDFrom;
	m_DeviceIDTo = DEVICEID_LIST;
	m_DeviceListTo = DeviceIDTo;
	m_ID = ID;
	m_MessageType = MessageType;
	m_Priority = Priority;
	
	va_list marker;
	va_start( marker, ParameterCount );
	for(unsigned long i=0;i<ParameterCount;i++)
	{
		long Key = va_arg(marker, unsigned long );
		if (Key >= 0)
		{
			m_Parameters[Key] = string(va_arg(marker, char* ));
		}
		else
		{
			char *pData = va_arg(marker, char* );
			unsigned long Size = va_arg(marker, unsigned long );
			m_DataLengths[-Key]=Size;
			char *inData = new char[Size];
			memcpy(inData, pData, Size);
			m_DataParameters[-Key]=inData;
		}
	}
	va_end( marker );              
}

// This allows for a message to be sent to a category of device
Message::Message(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eBL, long Priority, long MessageType, long ID, unsigned long ParameterCount, ...)
{
	Clear();
	m_DeviceIDFrom = DeviceIDFrom;
	m_DeviceIDTo = DEVICEID_CATEGORY;
	m_DeviceCategoryTo = DeviceCategory;
	m_bIncludeChildren = bIncludeChildren;
	m_eBroadcastLevel = eBL;
	m_ID = ID;
	m_MessageType = MessageType;
	m_Priority = Priority;
	
	va_list marker;
	va_start( marker, ParameterCount );
	for(unsigned long i=0;i<ParameterCount;i++)
	{
		long Key = va_arg(marker, unsigned long );
		if (Key >= 0)
		{
			m_Parameters[Key] = string(va_arg(marker, char* ));
		}
		else
		{
			char *pData = va_arg(marker, char* );
			unsigned long Size = va_arg(marker, unsigned long );
			m_DataLengths[-Key]=Size;
			char *inData = new char[Size];
			memcpy(inData, pData, Size);
			m_DataParameters[-Key]=inData;
		}
	}
	va_end( marker );              
}
// This allows for a message to be sent to a particular type of device
Message::Message(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eBL, long Priority, long MessageType, long ID, unsigned long ParameterCount, ...)
{
	Clear();
	m_DeviceIDFrom = DeviceIDFrom;
	m_DeviceIDTo = DEVICEID_MASTERDEVICE;
	m_MasterDevice = MasterDevice;
	m_eBroadcastLevel = eBL;
	m_ID = ID;
	m_MessageType = MessageType;
	m_Priority = Priority;
	
	va_list marker;
	va_start( marker, ParameterCount );
	for(unsigned long i=0;i<ParameterCount;i++)
	{
		long Key = va_arg(marker, unsigned long );
		if (Key >= 0)
		{
			m_Parameters[Key] = string(va_arg(marker, char* ));
		}
		else
		{
			char *pData = va_arg(marker, char* );
			unsigned long Size = va_arg(marker, unsigned long );
			m_DataLengths[-Key]=Size;
			char *inData = new char[Size];
			memcpy(inData, pData, Size);
			m_DataParameters[-Key]=inData;
		}
	}
	va_end( marker );              
}


void Message::Clear()
{
	m_bRelativeToSender=false;
	m_eExpectedResponse=ER_None;
	m_eBroadcastLevel=BL_SameHouse; // Default broadcast level
	m_bRespondedToMessage=false;
	m_Priority=0;
	m_DeviceIDFrom=-1;
	m_DeviceIDTo=-1;
	m_DeviceGroupIDTo=0;
	m_ID=-1;
	m_MessageType=0;
	m_DeviceCategoryTo=0;
	m_MasterDevice=0;
	m_bIncludeChildren=false;

	m_Parameters.clear();
/*  HACK  HACKHACKHACKHACKHACKHACKHACKHACKHACKHACKHACKHACKHACKHACKHACKHACKHACKHACKHACKHACKHACKHACKHACKHACK
THERE APPEARS TO BE SOME WEIRD BUT IN WINDOWS DLL MEMORY MANAGMEMENT.  THIS CRASHES AFTER THE DATA PARAMETER IS ALLOCATED WITH TODATA.  SEE REQUEST GRID.
TEMORARY CREATE A MEMORY LEAK SO WE CAN KEEP GOING
	map<long, char *>::iterator i;
	for(i=m_DataParameters.begin();i!=m_DataParameters.end();++i)
	{
		delete[] (*i).second;
	}
*/
	m_DataParameters.clear();
	m_DataLengths.clear();

}

void Message::ToData(unsigned long &Size, char* &Data, bool bWithHeader)
{
	StartWriting();
	unsigned long HeaderPosition=0,StartOfMessagePosition=0;

	if (bWithHeader)
	{
		HeaderPosition = (unsigned long) (m_pcCurrentPosition - m_pcDataBlock + 8);
#ifndef WIN32
		string s("MESSAGE               ");		//possibly this is a compiler bug  - gcc 3.3.3 
								//complains about call to non-existing function
		Write_string(s);
#else
		Write_string(string("MESSAGE               "));
#endif
		// Save a pointer to store the header size
		StartOfMessagePosition = (unsigned long) (m_pcCurrentPosition - m_pcDataBlock);
	}

	Write_long(1234); // Just a sanity check
	Write_long(m_DeviceIDFrom);
	Write_long(m_DeviceIDTo);
	Write_long(m_DeviceGroupIDTo);
	Write_long(m_ID);
	Write_long(m_Priority);
	Write_long(m_MessageType);
	Write_long(m_DeviceCategoryTo);
	Write_long(m_MasterDevice);
	Write_unsigned_char(m_bIncludeChildren);
	Write_long(m_eBroadcastLevel);
	Write_unsigned_char(m_bRelativeToSender);
	Write_long(m_eExpectedResponse);
	Write_string(m_DeviceListTo);

	Write_unsigned_long((unsigned long) m_Parameters.size());
	map<long, string>::iterator itParms;
	for(itParms=m_Parameters.begin();itParms!=m_Parameters.end();++itParms)
	{
		Write_long((*itParms).first);
		Write_string((*itParms).second);
	}

	map<long, char *>::iterator itDataParms;
	map<long, unsigned long>::iterator itDataLength;
	Write_unsigned_long((unsigned long) m_DataLengths.size());
	for(itDataLength=m_DataLengths.begin();itDataLength!=m_DataLengths.end();++itDataLength)
	{
		itDataParms = m_DataParameters.find( (*itDataLength).first );
		if( itDataParms != m_DataParameters.end() )
		{
			char *Ptr = (*itDataParms).second;
			unsigned long Length = (*itDataLength).second;
			if( !Ptr )
				Length=0;
			Write_long((*itDataLength).first);
			Write_unsigned_long(Length);
			Write_block(Ptr,Length);
		}
		else
		{
			// We have to write something because we already counted this in the list
			Write_long((*itDataLength).first);
			Write_unsigned_long(0);
		}
	}

	Write_unsigned_long((unsigned long) m_vectExtraMessages.size());
	vector<class Message *>::iterator itExtras;
	for(itExtras=m_vectExtraMessages.begin();
		itExtras!=m_vectExtraMessages.end();++itExtras)
	{
		Message *pMessage_Child = *itExtras;
		if( !pMessage_Child )  // The embedded message could have been deleted
			Write_unsigned_long(0);
		else
		{
			char *Data;
			unsigned long Size;
			pMessage_Child->ToData(Size,Data,false);
			Write_unsigned_long(Size);
			Write_block(Data,Size);
			delete Data;
		}
	}
	Write_unsigned_long(6789); // Just a sanity check

	if (bWithHeader)
	{
		unsigned long MessageSize = (unsigned long) ((m_pcCurrentPosition-m_pcDataBlock) - StartOfMessagePosition);
		char *pFreezePosition = m_pcCurrentPosition;
		m_pcCurrentPosition = m_pcDataBlock + HeaderPosition;
		string Size = StringUtils::itos(MessageSize);
		// Don't do a Write_String(StringUtils::itos(MessageSize)) because that will add a null term
		Write_block((char *)Size.c_str(),(unsigned long) Size.length());
		m_pcCurrentPosition = pFreezePosition;
	}

	Data = m_pcDataBlock;
	Size = (unsigned long)  (m_pcCurrentPosition - m_pcDataBlock);

	m_pcDataBlock=NULL; // Be sure the SerializeClass destructor doesn't also try to delete this

	return;
}


Message::Message(unsigned long Length, char *Data)
{
	Clear();
	FromData(Length, Data);
	m_pcDataBlock=NULL; // Be sure the SerializeClass destructor doesn't also try to delete this
}

void Message::FromData(unsigned long Length, char *Data)
{
	StartReading(Length,Data);

	unsigned long Start = Read_unsigned_long();
	if( Start!=1234 )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Badly formed message");
	}

	m_DeviceIDFrom=Read_long();
	m_DeviceIDTo=Read_long();
	m_DeviceGroupIDTo=Read_long();
	m_ID=Read_long();
	m_Priority=Read_long();
	m_MessageType=Read_long();
	m_DeviceCategoryTo=Read_long();
	m_MasterDevice=Read_long();
	m_bIncludeChildren=(Read_unsigned_char()==1);
	m_eBroadcastLevel=(eBroadcastLevel) Read_long();
	m_bRelativeToSender=Read_unsigned_char()==1;
	m_eExpectedResponse=(eExpectedResponse) Read_long();
	Read_string(m_DeviceListTo);

	unsigned long NumParms = Read_unsigned_long();
	for(unsigned long iNP = 0; iNP<NumParms; ++iNP)
	{
		unsigned long ID = Read_unsigned_long();
		string Value;
		Read_string(Value);
		m_Parameters[ID]=Value;
	}

	unsigned long NumData = Read_unsigned_long();
	for(unsigned long iND = 0; iND<NumData; ++iND)
	{
		unsigned long ID = Read_unsigned_long();
		unsigned long Size = Read_unsigned_long();
		if( Size )
		{
			char *Data = Read_block(Size);
			m_DataLengths[ID]=Size;
			m_DataParameters[ID]=Data;
		}
	}

	unsigned long NumChildren = Read_unsigned_long();
	for(unsigned long iNC = 0; iNC<NumChildren; ++iNC)
	{
		unsigned long Size = Read_unsigned_long();
		if( Size )
		{
			Message *pMessage_Child = new Message(Size,m_pcCurrentPosition);
			m_vectExtraMessages.push_back(pMessage_Child);
			m_pcCurrentPosition += Size;
		}
	}

	unsigned long End = Read_unsigned_long();
	if( End!=6789 )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Badly formed message");
	}

	return;
}

