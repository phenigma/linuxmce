/***************************************************************************
 *   Copyright (C) 2005 by Igor Spac,,,                                    *
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
#include "SerialIOConnection.h"
#include "DCE/Logger.h"

#define DEFAULT_SERIAL_PORT	"ttyS0"

using namespace DCE;

namespace DCE {

SerialIOConnection::SerialIOConnection()
	: psp_(NULL), serport_(DEFAULT_SERIAL_PORT), bps_(9600), bs_(epbsN81), fc_(false)
{
}


SerialIOConnection::~SerialIOConnection()
{
}

bool 
SerialIOConnection::Open() {	
	Close();
	
    g_pPlutoLogger->Write(LV_STATUS, "Opening connection to %s.", serport_.c_str());
    try {
        psp_ = new CSerialPort(serport_, bps_, bs_, fc_);
    } catch(...) {
        g_pPlutoLogger->Write(LV_CRITICAL, "Failed Opening serial port: %s.", serport_.c_str());
		Close();
        return false;
    }

    g_pPlutoLogger->Write(LV_STATUS, "Connection opened.");
	return true;
}

void 
SerialIOConnection::Close() {
	if(psp_ != NULL) {
		delete psp_;
		psp_ = NULL;
	}
}
	
int 
SerialIOConnection::Send(const char* buff, unsigned int size) {
	if(psp_ != NULL) {
		g_pPlutoLogger->Write(LV_STATUS, "Sending buffer to %s with size %d: <%s>.", 
									serport_.c_str(), size, FormatHexBuffer(buff, size).c_str());
		psp_->Write((char*)buff, size);
		g_pPlutoLogger->Write(LV_STATUS, "Buffer sent.");
		return size;
	} else {
		g_pPlutoLogger->Write(LV_WARNING, "Trying to send DATA while not connected.");
		return -1;
	}
}

int 
SerialIOConnection::Recv(char* buff, unsigned int size, int timeout) {
	if(psp_ != NULL) {
		g_pPlutoLogger->Write(LV_STATUS, "Receiving buffer from %s with max size %d and timeout %d...", 
									serport_.c_str(), size, timeout);
		int retsize = psp_->Read(buff, size, timeout);
		g_pPlutoLogger->Write(LV_STATUS, "Received buffer from %s: <%s>", 
									serport_.c_str(), FormatHexBuffer(buff, retsize).c_str());
		return retsize;
	} else {
		g_pPlutoLogger->Write(LV_WARNING, "Trying to receive DATA while not connected.");
		return -1;
	}
}
	
bool 
SerialIOConnection::isOpened() {
	return (psp_ != NULL);
}

bool 
SerialIOConnection::isDataAvailable() {
	if(psp_ != NULL) {
		return !(psp_->IsReadEmpty());
	} else {
		return false;
	}
}

std::string 
SerialIOConnection::FormatHexBuffer(const char* buff, unsigned int size) {
	std::string logstr;
	if(size <= 0) {
		logstr = "EMPTY BUFFER";
	} else {
		char hxbuff[5];
		for(unsigned int i = 0; i < size; i++) {
			sprintf(hxbuff, "0x%0hhx", buff[i]);
			logstr += ((i > 0) ? " " : "");
			logstr += hxbuff;
		}
	}
	return logstr;
}

};
