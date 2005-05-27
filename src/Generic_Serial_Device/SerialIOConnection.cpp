/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

#include "SerialIOConnection.h"
#include "DCE/Logger.h"

#include "IOUtils.h"

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
									serport_.c_str(), size, IOUtils::FormatHexAsciiBuffer(buff, size).c_str());
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
		if(retsize == 0) {
			g_pPlutoLogger->Write(LV_STATUS, "Received EOF. Closing Connection.");
			Close();		
		} else
		if(retsize < 0) {
			g_pPlutoLogger->Write(LV_STATUS, "Timeout receiving data.");
		} else {
			g_pPlutoLogger->Write(LV_STATUS, "Received buffer from %s: <%s>", 
									serport_.c_str(), IOUtils::FormatHexAsciiBuffer(buff, retsize).c_str());
		}
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

void SerialIOConnection::Flush() {
	if(psp_ != NULL) {
		psp_->Flush();
	}
}

bool 
SerialIOConnection::isDataAvailable(int timeout) {
	if(psp_ != NULL) {
		while(timeout > 0) {
			if(!(psp_->IsReadEmpty())) {
				return true;
			}
			usleep(10);
			timeout -= 10;
		}
		return (!(psp_->IsReadEmpty()));
	}
	return false;
}

};
