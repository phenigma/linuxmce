/* 
	Message
	
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation
	
	www.plutohome.com		
	
	Phone: +1 (877) 758-8648
	
	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty 
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
	
	See the GNU General Public License for more details.
*/

/**
 *
 * @file Message.cpp
 * @brief source file for the Message class
 * @author
 * @todo notcommented
 *
 */


#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "Message.h"
#include "DCE/Logger.h"

using namespace DCE;

Message::Message()
{
	Clear();
}

Message::Message( unsigned long dwSize, char *pcData )
{
	Clear();
	FromData( dwSize, pcData );
	m_pcDataBlock = NULL; // Be sure the SerializeClass destructor doesn't also try to delete this
}

Message::Message( Message *pMessage_Clone )
{
	Clear();

	m_dwPK_Device_From = pMessage_Clone->m_dwPK_Device_From;
	m_dwPK_Device_To = pMessage_Clone->m_dwPK_Device_To;
	m_dwPK_Device_Group_ID_To = pMessage_Clone->m_dwPK_Device_Group_ID_To;
	m_dwID = pMessage_Clone->m_dwID;
	m_dwPriority = pMessage_Clone->m_dwPriority;
	m_dwMessage_Type = pMessage_Clone->m_dwMessage_Type;
	m_dwPK_Device_Category_To = pMessage_Clone->m_dwPK_Device_Category_To;
	m_dwPK_Device_Template = pMessage_Clone->m_dwPK_Device_Template;
	m_bIncludeChildren = pMessage_Clone->m_bIncludeChildren;
	m_bRelativeToSender = pMessage_Clone->m_bRelativeToSender;
	m_bRespondedToMessage = pMessage_Clone->m_bRespondedToMessage;
	m_eBroadcastLevel = pMessage_Clone->m_eBroadcastLevel;
	m_eExpectedResponse = pMessage_Clone->m_eExpectedResponse;
	m_sPK_Device_List_To = pMessage_Clone->m_sPK_Device_List_To;

	map<long, string>::iterator itParms;
	for(itParms=pMessage_Clone->m_mapParameters.begin();itParms!=pMessage_Clone->m_mapParameters.end();++itParms)
	{
		m_mapParameters[(*itParms).first]=(*itParms).second;
	}

	map<long, char *>::iterator itDataParms;
	map<long, unsigned long>::iterator itDataLength;
	for(itDataLength=pMessage_Clone->m_mapData_Lengths.begin();itDataLength!=pMessage_Clone->m_mapData_Lengths.end();++itDataLength)
	{
		itDataParms = pMessage_Clone->m_mapData_Parameters.find( (*itDataLength).first );
		if( itDataParms != pMessage_Clone->m_mapData_Parameters.end() )
		{
			m_mapData_Lengths[(*itDataLength).first] = (*itDataLength).second;
			char *Buffer = new char[(*itDataLength).second];
			memcpy(Buffer,(*itDataParms).second,(*itDataLength).second);
			m_mapData_Parameters[(*itDataLength).first] = Buffer;
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

Message::Message( long dwDeviceIDFrom, long dwDeviceIDTo, long dwPriority, long dwMessageType, long dwID, unsigned long dwParameterCount, ... )
{
	Clear();
	
	m_dwPK_Device_From = dwDeviceIDFrom;
	m_dwPK_Device_To = dwDeviceIDTo;
	m_dwID = dwID;
	m_dwMessage_Type = dwMessageType;
	m_dwPriority = dwPriority;
	
	va_list marker;
	va_start( marker, dwParameterCount );
	for( unsigned long i=0; i < dwParameterCount; i++ )
	{
		long Key = va_arg( marker, unsigned long );
		if ( Key >= 0 ) // if Key > 0 it is a normal string parameter, otherwise i's a data block
		{
			m_mapParameters[Key] = string( va_arg( marker, char* ) );
		}
		else
		{
			char *pcData = va_arg( marker, char* );
			unsigned long dwSize = va_arg( marker, unsigned long );
			m_mapData_Lengths[-Key] = dwSize;
			char *pcInData = new char[dwSize];
			memcpy( pcInData, pcData, dwSize );
			m_mapData_Parameters[-Key] = pcInData;
		}
	}
	va_end( marker );              
}

Message::Message( long dwDeviceIDFrom, string dwDeviceIDTo, long dwPriority, long dwMessageType, long dwID, unsigned long dwParameterCount, ... )
{
	Clear();
	
	m_dwPK_Device_From = dwDeviceIDFrom;
	m_dwPK_Device_To = DEVICEID_LIST;
	m_sPK_Device_List_To = dwDeviceIDTo;
	m_dwID = dwID;
	m_dwMessage_Type = dwMessageType;
	m_dwPriority = dwPriority;
	
	va_list marker;
	va_start( marker, dwParameterCount );
	for( unsigned long i=0; i<dwParameterCount; i++ )
	{
		long Key = va_arg( marker, unsigned long );
		if ( Key >= 0 ) // if Key > 0 it is a normal string parameter, otherwise i's a data block
		{
			m_mapParameters[Key] = string( va_arg( marker, char* ));
		}
		else
		{
			char *pcData = va_arg( marker, char* );
			unsigned long dwSize = va_arg( marker, unsigned long );
			m_mapData_Lengths[-Key] = dwSize;
			char *pcInData = new char[dwSize];
			memcpy( pcInData, pcData, dwSize );
			m_mapData_Parameters[-Key] = pcInData;
		}
	}
	va_end( marker );              
}

Message::Message( long dwDeviceIDFrom, long dwDeviceCategory, bool bIncludeChildren, eBroadcastLevel eBL, long dwPriority, long dwMessageType, long dwID, unsigned long dwParameterCount, ... )
{
	Clear();
	
	m_dwPK_Device_From = dwDeviceIDFrom;
	m_dwPK_Device_To = DEVICEID_CATEGORY;
	m_dwPK_Device_Category_To = dwDeviceCategory;
	m_bIncludeChildren = bIncludeChildren;
	m_eBroadcastLevel = eBL;
	m_dwID = dwID;
	m_dwMessage_Type = dwMessageType;
	m_dwPriority = dwPriority;
	
	va_list marker;
	va_start( marker, dwParameterCount );
	for( unsigned long i=0; i < dwParameterCount; i++ )
	{
		long Key = va_arg( marker, unsigned long );
		if ( Key >= 0 ) // if Key > 0 it is a normal string parameter, otherwise i's a data block
		{
			m_mapParameters[Key] = string( va_arg( marker, char* ) );
		}
		else
		{
			char *pcData = va_arg( marker, char* );
			unsigned long dwSize = va_arg( marker, unsigned long );
			m_mapData_Lengths[-Key] = dwSize;
			char *pcInData = new char[dwSize];
			memcpy( pcInData, pcData, dwSize );
			m_mapData_Parameters[-Key] = pcInData;
		}
	}
	va_end( marker );              
}
// This allows for a message to be sent to a particular type of device
Message::Message(long dwDeviceIDFrom, long dwMasterDevice, eBroadcastLevel eBL, long dwPriority, long dwMessageType, long dwID, unsigned long dwParameterCount, ...)
{
	Clear();
	m_dwPK_Device_From = dwDeviceIDFrom;
	m_dwPK_Device_To = DEVICEID_MASTERDEVICE;
	m_dwPK_Device_Template = dwMasterDevice;
	m_eBroadcastLevel = eBL;
	m_dwID = dwID;
	m_dwMessage_Type = dwMessageType;
	m_dwPriority = dwPriority;
	
	va_list marker;
	va_start( marker, dwParameterCount );
	for( unsigned long i=0; i < dwParameterCount; i++ )
	{
		long Key = va_arg( marker, unsigned long );
		if ( Key >= 0 )
		{
			m_mapParameters[Key] = string( va_arg( marker, char* ) );
		}
		else
		{
			char *pcData = va_arg( marker, char* );
			unsigned long dwSize = va_arg( marker, unsigned long );
			m_mapData_Lengths[-Key] = dwSize;
			char *pcInData = new char[dwSize];
			memcpy( pcInData, pcData, dwSize );
			m_mapData_Parameters[-Key] = pcInData;
		}
	}
	va_end( marker );              
}

Message::~Message()
{
	Clear();
}

void Message::Clear()
{
	m_bRelativeToSender = false;
	m_eExpectedResponse = ER_None;
	m_eBroadcastLevel = BL_SameHouse; // Default broadcast level
	m_bRespondedToMessage = false;
	m_dwPriority = 0;
	m_dwPK_Device_From = -1;
	m_dwPK_Device_To = -1;
	m_dwPK_Device_Group_ID_To = 0;
	m_dwID = -1;
	m_dwMessage_Type = 0;
	m_dwPK_Device_Category_To = 0;
	m_dwPK_Device_Template = 0;
	m_bIncludeChildren = false;

	m_mapParameters.clear();
	
	/** @todo check comment */
	/*  
	HACK  HACKHACKHACKHACKHACKHACKHACKHACKHACKHACKHACKHACKHACKHACKHACKHACKHACKHACKHACKHACKHACKHACKHACKHACK
	THERE APPEARS TO BE SOME WEIRD BUT IN WINDOWS DLL MEMORY MANAGMEMENT.  THIS CRASHES AFTER THE DATA PARAMETER IS ALLOCATED WITH TODATA.  SEE REQUEST GRID.
	TEMORARY CREATE A MEMORY LEAK SO WE CAN KEEP GOING
	map<long, char *>::iterator i;
	for(i=m_DataParameters.begin();i!=m_DataParameters.end();++i)
	{
		delete[] (*i).second;
	}
	*/
	
	m_mapData_Parameters.clear();
	m_mapData_Lengths.clear();
}

void Message::ToData( unsigned long &dwSize, char* &pcData, bool bWithHeader )
{
	// See comments at end of this message to see why we don't delete the memory allocated in SerializeClass
	StartWriting();
	unsigned long dwHeaderPosition=0, dwStartOfMessagePosition=0;

	if ( bWithHeader )
	{
		dwHeaderPosition = m_pcCurrentPosition - m_pcDataBlock + 8;
#ifndef WIN32
		string s("MESSAGE               ");		//possibly this is a compiler bug  - gcc 3.3.3 
								//complains about call to non-existing function
		Write_string(s);
#else
		Write_string( string( "MESSAGE               " ) );
#endif
		// Save a pointer to store the header size
		dwStartOfMessagePosition = m_pcCurrentPosition - m_pcDataBlock;
	}

	Write_long( 1234 ); // Just a sanity check
	Write_long( m_dwPK_Device_From );
	Write_long( m_dwPK_Device_To );
	Write_long( m_dwPK_Device_Group_ID_To );
	Write_long( m_dwID );
	Write_long( m_dwPriority );
	Write_long( m_dwMessage_Type );
	Write_long( m_dwPK_Device_Category_To );
	Write_long( m_dwPK_Device_Template );
	Write_unsigned_char( m_bIncludeChildren );
	Write_long( m_eBroadcastLevel );
	Write_unsigned_char( m_bRelativeToSender );
	Write_long( m_eExpectedResponse );
	Write_string( m_sPK_Device_List_To );

	Write_unsigned_long( (unsigned long) m_mapParameters.size() );
	map<long, string>::iterator itParms;
	for( itParms=m_mapParameters.begin(); itParms != m_mapParameters.end(); ++itParms)
	{
		Write_long( (*itParms).first );
		Write_string( (*itParms).second );
	}

	map<long, char *>::iterator itDataParms;
	map<long, unsigned long>::iterator itDataLength;
	Write_unsigned_long((unsigned long) m_mapData_Lengths.size());
	for( itDataLength=m_mapData_Lengths.begin(); itDataLength != m_mapData_Lengths.end(); ++itDataLength )
	{
		itDataParms = m_mapData_Parameters.find( (*itDataLength).first );
		if( itDataParms != m_mapData_Parameters.end() )
		{
			char *Ptr = (*itDataParms).second;
			unsigned long Length = (*itDataLength).second;
			if( !Ptr ) Length=0;
			Write_long( (*itDataLength).first );
			Write_unsigned_long( Length );
			Write_block( Ptr, Length );
		}
		else
		{
			// We have to write something because we already counted this in the list
			Write_long( (*itDataLength).first );
			Write_unsigned_long(0);
		}
	}

	Write_unsigned_long( (unsigned long) m_vectExtraMessages.size() );
	vector<class Message *>::iterator itExtras;
	for( itExtras=m_vectExtraMessages.begin(); itExtras!=m_vectExtraMessages.end(); ++itExtras )
	{
		Message *pMessage_Child = *itExtras;
		if( !pMessage_Child )  // The embedded message could have been deleted
			Write_unsigned_long( 0 );
		else
		{
			char *Data;
			unsigned long Size;
			pMessage_Child->ToData( Size, Data, false );
			Write_unsigned_long( Size );
			Write_block( Data, Size );
			delete Data;
		}
	}
	
	Write_unsigned_long(6789); // Just a sanity check

	if (bWithHeader)
	{
		unsigned long MessageSize = (unsigned long) ((m_pcCurrentPosition - m_pcDataBlock) - dwStartOfMessagePosition );
		char *pcFreezePosition = m_pcCurrentPosition;
		m_pcCurrentPosition = m_pcDataBlock + dwHeaderPosition;
		string Size = StringUtils::itos( MessageSize );
		// Don't do a Write_String(StringUtils::itos(MessageSize)) because that will add a null term
		Write_block((char *)Size.c_str(),Size.length());
		m_pcCurrentPosition = pcFreezePosition;
	}

	// Normally we would want to delete the memory in serialize class.  But here we assign the 'out' data pointer pcData
	// to the memory block.  Socket::SendMessage needs the data block since that's what it sends, and it will delete it there.
	pcData = m_pcDataBlock;
	dwSize = ( m_pcCurrentPosition - m_pcDataBlock );

	m_pcDataBlock=NULL; // Be sure the SerializeClass destructor doesn't also try to delete this

	return;
}

void Message::FromData( unsigned long dwSize, char *pcData )
{
	StartReading( dwSize, pcData );

	// making sure that pcData contains what it should
	unsigned long dwStart = Read_unsigned_long(); // the first long should be 1234 by convention
	if( dwStart != 1234 )
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "Badly formed message" );
	}

	// reading the member data
	
	m_dwPK_Device_From = Read_long();
	m_dwPK_Device_To = Read_long();
	m_dwPK_Device_Group_ID_To = Read_long();
	m_dwID = Read_long();
	m_dwPriority = Read_long();
	m_dwMessage_Type = Read_long();
	m_dwPK_Device_Category_To = Read_long();
	m_dwPK_Device_Template = Read_long();
	m_bIncludeChildren=(Read_unsigned_char()==1);
	m_eBroadcastLevel=(eBroadcastLevel) Read_long();
	m_bRelativeToSender = Read_unsigned_char()==1;
	m_eExpectedResponse=(eExpectedResponse) Read_long();
	Read_string( m_sPK_Device_List_To );

	unsigned long dwNumParms = Read_unsigned_long();
	for( unsigned long dwNP = 0; dwNP < dwNumParms; ++dwNP )
	{
		unsigned long dwID = Read_unsigned_long();
		string sValue;
		Read_string( sValue );
		m_mapParameters[dwID] = sValue;
	}

	unsigned long dwNumData = Read_unsigned_long();
	for( unsigned long dwND = 0; dwND < dwNumData; ++dwND )
	{
		unsigned long dwID = Read_unsigned_long();
		unsigned long Size = Read_unsigned_long();
		if( Size )
		{
			char *Data = Read_block( Size );
			m_mapData_Lengths[dwID] = Size;
			m_mapData_Parameters[dwID] = Data;
		}
	}

	unsigned long dwNumChildren = Read_unsigned_long();
	for( unsigned long dwNC = 0; dwNC < dwNumChildren; ++dwNC )
	{
		unsigned long Size = Read_unsigned_long();
		if( Size )
		{
			Message *pMessage_Child = new Message( Size, m_pcCurrentPosition );
			m_vectExtraMessages.push_back(pMessage_Child);
			m_pcCurrentPosition += Size;
		}
	}

	// checking again if the message is in propper form (it's ending how it should)
	unsigned long dwEnd = Read_unsigned_long();
	if( dwEnd!=6789 )
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "Badly formed message" );
	}

	return;
}

