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
#ifndef DCESERIALIOCONNECTION_H
#define DCESERIALIOCONNECTION_H

#include "IOConnection.h"
#include "Serial/SerialPort.h"

namespace DCE {

/**
@author Igor Spac,,,
*/
class SerialIOConnection : public IOConnection {
public:
    SerialIOConnection();
    virtual ~SerialIOConnection();

public:
	void setSerialPort(const char* serport) {
		serport_ = serport;
	}
	const char* getSerialPort() {
		return serport_.c_str();
	}
	
	void setBPS(unsigned int bps) {
		bps_ = bps;
	}
	unsigned int getBPS() {
		return bps_;
	}
	
	void setParityBitStop(eParityBitStop bs) {
		bs_ = bs;
	}
	unsigned int getParityBitStop() {
		return bs_;
	}
	
	void setEnableFlowControl(bool fc) {
		fc_ = fc;
	}
	bool getEnableFlowControl() {
		return fc_;
	}

public:
	virtual bool Open();
	virtual void Close();
	
	virtual int Send(const char* buff, unsigned int size);
	virtual int Recv(char* buff, unsigned int size, int timeout);
	
	virtual bool isOpened();
	virtual bool isDataAvailable();
	
private:
	std::string FormatHexBuffer(const char* buff, unsigned int size);

private:
	CSerialPort* psp_;
	
	std::string serport_;
	unsigned int bps_;
	eParityBitStop bs_;
	bool fc_;
};

};

#endif
