//
// C++ Interface: extendedserialport
//
// Description: 
//
//
// Author:  <>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef EIBBUSEXTENDEDSERIALPORT_H
#define EIBBUSEXTENDEDSERIALPORT_H

#include "Serial/SerialPort.h"

#include <sstream>

namespace EIBBUS {

/**
@author 
*/
class ExtendedSerialPort : public CSerialPort
{
public:
    ExtendedSerialPort(string Port, unsigned int BPS, enum eParityBitStop ParityBitStop, bool EnableFlowControl=false);
    virtual ~ExtendedSerialPort();
	
public:
    virtual bool IsReadEmpty();
    virtual size_t Read(char *Buf, size_t MaxLen, int Timeout=5);
    virtual void Write(char *Buf, size_t Len);
    
	virtual int UndoRead(char *Buf, size_t Len);

public:
	stringbuf buff_;
};

};

#endif
