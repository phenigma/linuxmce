/***************************************************************************
 *   Copyright (C) 2005 by igor                                            *
 *   igor@dexx                                                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "message.h"
#include <string.h>
#include "busconnector.h"

#include "DCE/Logger.h"

#define MAX_FRAME_LENGTH	30

// Definitions of EIB messages
#define EIB_FIRST_TYPE_BYTE 0
#define EIB_FIRST_LENGTH_BYTE 1
#define EIB_SECOND_LENGTH_BYTE 2
#define EIB_SECOND_TYPE_BYTE 3
#define EIB_CONTROL_FIELD_BYTE 4
#define EIB_MESSAGE_CODE_BYTE 5

#define EIB_STOP_BYTE_VALUE 0x16
#define EIB_TYPE_BYTE_VALUE 0x68

#define EIB_SIZE_OF_PEI_HEADER 4
#define EIB_SIZE_OF_PEI_TAIL 2
#define EIB_SIZE_OF_PEI_FRAME 6
#define EIB_SIZE_OF_PEI_CONTROL_FIELD 1
#define EIB_SIZE_OF_LINK_LAYER_FIELD 3

// only valid for received messages
#define EIB_STATUS_BYTE 6
#define EIB_FIRST_TIMESTAMP_BYTE 7
#define EIB_SECOND_TIMESTAMP_BYTE 8
#define EIB_RECEIVED_MESSAGE_CODE_VALUE 0x2B

// only valid for sent messages
#define EIB_SENT_APCI_BYTE 13
#define EIB_SENT_MESSAGE_CODE_VALUE 0x11
#define EIB_SENT_FIRST_SOURCE_ADDRESS_BYTE 7
#define EIB_SENT_SOURCE_ADDRESS_BYTE_VALUE 0x00


using namespace DCE;

namespace EIBBUS {

int 
Message::SendBuffer(BusConnector *pbusconn, 
		const unsigned char* msg, unsigned int length)
{
	pbusconn->Send((unsigned char*)msg, length);
	pbusconn->incFrameBit();
	return 0;
}

int 
Message::RecvBuffer(BusConnector *pbusconn, 
		unsigned char* msg, unsigned int length)
{
	return pbusconn->Recv(msg, length, 1000);
}

int
PeiMessage::SendPeiBuffer(BusConnector *pbusconn, 
		const unsigned char* msg, unsigned int length) 
{
	unsigned char peibuff[MAX_FRAME_LENGTH];
	
	peibuff[EIB_FIRST_TYPE_BYTE] = EIB_TYPE_BYTE_VALUE;
	peibuff[EIB_SECOND_TYPE_BYTE] = EIB_TYPE_BYTE_VALUE;
	peibuff[EIB_MESSAGE_CODE_BYTE] = EIB_SENT_MESSAGE_CODE_VALUE;
	
	peibuff[EIB_FIRST_LENGTH_BYTE] =
		length + EIB_SIZE_OF_PEI_CONTROL_FIELD;
	peibuff[EIB_SECOND_LENGTH_BYTE] =
		length + EIB_SIZE_OF_PEI_CONTROL_FIELD ;

	memcpy(&peibuff[EIB_MESSAGE_CODE_BYTE], msg, length);
	
	// Set ControlField
	if(pbusconn->getFrameBit()) {
		peibuff[EIB_CONTROL_FIELD_BYTE]= 0x73;
	} else {
		peibuff[EIB_CONTROL_FIELD_BYTE]= 0x53;
	}
	
	// Calculate PEI-CHKSUM,
	unsigned char chksum = peibuff[EIB_CONTROL_FIELD_BYTE];
	
	int laenge = ((int)peibuff[EIB_FIRST_LENGTH_BYTE])
						+ EIB_SIZE_OF_PEI_HEADER;
	for ( int i = EIB_MESSAGE_CODE_BYTE ; i < laenge ; i++) {
			chksum += peibuff[i];
	}
	// Set PEI-CHKSUM,
	peibuff[laenge] = chksum;
	
	// Set Stop Byte
	peibuff[laenge + 1] = EIB_STOP_BYTE_VALUE;
	
	return SendBuffer(pbusconn, peibuff, sizeof(peibuff));
}

int 
ServerMessage::SendServerBuffer(BusConnector *pbusconn, 
		const unsigned char* msg, unsigned int length) {
	
	unsigned char peibuff[MAX_FRAME_LENGTH];
	
	peibuff[EIB_FIRST_TYPE_BYTE] = EIB_TYPE_BYTE_VALUE;
	peibuff[EIB_SECOND_TYPE_BYTE] = EIB_TYPE_BYTE_VALUE;
	peibuff[EIB_MESSAGE_CODE_BYTE] = EIB_SENT_MESSAGE_CODE_VALUE;
	
	peibuff[EIB_FIRST_LENGTH_BYTE] =
	peibuff[EIB_SECOND_LENGTH_BYTE] =
		length + 2;

	memcpy(&peibuff[EIB_STATUS_BYTE], msg, length);
		
	// Set ControlField
	if(pbusconn->getFrameBit()) {
		peibuff[EIB_CONTROL_FIELD_BYTE]= 0x73;
	} else {
		peibuff[EIB_CONTROL_FIELD_BYTE]= 0x53;
	}
	
	// Calculate PEI-CHKSUM,
	unsigned char chksum = peibuff[EIB_CONTROL_FIELD_BYTE];
	
	int laenge = ((int)peibuff[EIB_FIRST_LENGTH_BYTE])
						+ EIB_SIZE_OF_PEI_HEADER;
	for ( int i = EIB_MESSAGE_CODE_BYTE ; i < laenge ; i++) {
			chksum += peibuff[i];
	}
	// Set PEI-CHKSUM,
	peibuff[laenge] = chksum;
	
	// Set Stop Byte
	peibuff[laenge + 1] = EIB_STOP_BYTE_VALUE;
	
	ReceiveServerBuffer(pbusconn, (unsigned char*)msg, length);
	return SendBuffer(pbusconn, peibuff, sizeof(peibuff));
}

int 
ServerMessage::ReceiveServerBuffer(BusConnector *pbusconn, 
	unsigned char* msg, unsigned int& length) 
{
	unsigned char peibuff[MAX_FRAME_LENGTH];
	
	/*receive header*/
	if(RecvBuffer(pbusconn, peibuff, 1) < 1) {
		return -1;
	}
	
	if(peibuff[0] != EIB_TYPE_BYTE_VALUE) {
//		g_pPlutoLogger->Write(LV_STATUS, "Invalid Message Type");
		return 1;
	}
		
	if(RecvBuffer(pbusconn, &peibuff[1], EIB_SIZE_OF_PEI_HEADER - 1) < EIB_SIZE_OF_PEI_HEADER - 1) {
//		g_pPlutoLogger->Write(LV_WARNING, "Could not receive complete Message Header");
		return 1;
	}
	
	/*get message length including TAIL length*/
	unsigned mlength = 0;
	if(peibuff[EIB_FIRST_LENGTH_BYTE] == peibuff[EIB_SECOND_LENGTH_BYTE]
		&& EIB_TYPE_BYTE_VALUE == peibuff[EIB_SECOND_TYPE_BYTE]) {
		mlength = peibuff[EIB_FIRST_LENGTH_BYTE];
	} else {
//		g_pPlutoLogger->Write(LV_WARNING, "Length fields set incorrectly");
		return 1;
	}
	
	if(RecvBuffer(pbusconn, &peibuff[EIB_SIZE_OF_PEI_HEADER], mlength + EIB_SIZE_OF_PEI_TAIL) < (int)mlength + EIB_SIZE_OF_PEI_TAIL) {
//		g_pPlutoLogger->Write(LV_WARNING, "Could not receive message body");
		return 1;
	}

	/* check tail */	
	unsigned char chksum = peibuff[EIB_CONTROL_FIELD_BYTE];
	int dlength = ((int)peibuff[EIB_FIRST_LENGTH_BYTE])
                     - EIB_SIZE_OF_PEI_CONTROL_FIELD ;
	
	/* calculate checksum */
	for ( int i = 0 ; i < dlength; i++)
	{
		chksum += peibuff[ i + EIB_MESSAGE_CODE_BYTE];
	}
	
	if (( chksum != peibuff[ dlength + EIB_MESSAGE_CODE_BYTE] )
		|| ( EIB_STOP_BYTE_VALUE != peibuff[dlength + EIB_MESSAGE_CODE_BYTE+1] ))	{
		g_pPlutoLogger->Write(LV_WARNING, "Invalid CHECKSUM");
		return -1;
	};
   
	/* We are only interested in messages with
	containing user data,
	i.e. messages that are at least 13 bytes long */	
	if ( 12 > (int)peibuff[EIB_FIRST_LENGTH_BYTE]-1 ) {
		return 1;
	}
	
	/* We are only interested in busmonitor messages */
	if ( EIB_RECEIVED_MESSAGE_CODE_VALUE != peibuff[EIB_MESSAGE_CODE_BYTE]) {
// 		return 1;
	}

	/*check if repetition*/
	unsigned short ts = *((unsigned short*)&peibuff[EIB_FIRST_TIMESTAMP_BYTE]);
	if ( ts == pbusconn->getLastFrameTimestamp()) {
		g_pPlutoLogger->Write(LV_STATUS, "Message is a REPETITION");
		return 1;
	}
	pbusconn->setLastFrameTimestamp(ts);
   	
	/*copy msg*/
	unsigned int ulength = peibuff[EIB_FIRST_LENGTH_BYTE] - EIB_SIZE_OF_LINK_LAYER_FIELD - 2 - 1;
	if(ulength <= length) {
		length = ulength;
		memcpy(msg, &peibuff[EIB_STATUS_BYTE + EIB_SIZE_OF_LINK_LAYER_FIELD], length);
		return 0;
	}

	return -1;
}


};
