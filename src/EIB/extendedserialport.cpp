//
// C++ Implementation: extendedserialport
//
// Description: 
//
//
// Author:  <>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "extendedserialport.h"

using namespace std;

namespace EIBBUS {

ExtendedSerialPort::ExtendedSerialPort(string Port, unsigned int BPS, enum eParityBitStop ParityBitStop, bool EnableFlowControl)
	: CSerialPort(Port, BPS, ParityBitStop, EnableFlowControl)
{}

ExtendedSerialPort::~ExtendedSerialPort()
{}

bool 
ExtendedSerialPort::IsReadEmpty() {
	return CSerialPort::IsReadEmpty() && !buff_.in_avail();
}

size_t 
ExtendedSerialPort::Read(char *Buf, size_t MaxLen, int Timeout) {
	streamsize sz = buff_.sgetn(Buf, MaxLen);
	if(sz == 0) {
		return CSerialPort::Read(Buf, MaxLen, Timeout);
	} else {
		if(sz == (streamsize)MaxLen) {
			return sz;
		} else {
			return sz + CSerialPort::Read(Buf + sz, MaxLen - sz, Timeout);
		}
	}
}

void 
ExtendedSerialPort::Write(char *Buf, size_t Len) {
	return CSerialPort::Write(Buf, Len);
}

int ExtendedSerialPort::UndoRead(char *Buf, size_t Len) {
	buff_.sputn(Buf, Len);
	return Len;
}

};
