/***************************************************************************
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
#ifndef EIBMESSAGE_H
#define EIBMESSAGE_H

namespace EIBBUS {

class BusConnector;


/**
@author igor
*/
class Message {
	friend class MessagePool;
public:
    Message() {};
    virtual ~Message() {};

protected:
	int SendBuffer(BusConnector *pbusconn, 
		const unsigned char* msg, unsigned int length);
	int RecvBuffer(BusConnector *pbusconn, 
		unsigned char* msg, unsigned int length);

	virtual int Send(BusConnector *pbusconn) 
	{ return -1; };
	virtual int Recv(BusConnector *pbusconn)
	{ return -1; };
};


#define MAX_USERDATA_LENGTH		20

class PeiMessage : public Message {
protected:
	int SendPeiBuffer(BusConnector *pbusconn, 
		const unsigned char* msg, unsigned int length);
};

class ServerMessage : public Message {
protected:
	int SendServerBuffer(BusConnector *pbusconn, 
		const unsigned char* msg, unsigned int length);
	int ReceiveServerBuffer(BusConnector *pbusconn, 
		unsigned char* msg, unsigned int& length);
};

};

#endif
