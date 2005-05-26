#ifndef __SERIAL_PORT_H__
#define __SERIAL_PORT_H__

#include <string>

#ifdef WIN32
#include "serio.h"
#endif

using namespace std;

enum eParityBitStop { 
	epbsN81,
	epbsE81,
	epbsO81
};

class   CSerialPort
{
#ifdef WIN32
	serio_t m_Serio;
#else
    int m_fdSerial;
#endif

public:
	// Will throw with an error description as a string if the port cannot be opened.
	CSerialPort(string Port, unsigned int BPS, enum eParityBitStop ParityBitStop, bool EnableFlowControl=false);
	virtual ~CSerialPort();

	virtual bool IsReadEmpty();
	virtual size_t Read(char *Buf, size_t MaxLen, int Timeout=5);
	virtual void Write(char *Buf, size_t Len);
	virtual void Flush();

public:
#ifdef WIN32
    serio_t
#else
    int
#endif
	getHandle() {
#ifdef WIN32
		return m_Serio;
#else
		return m_fdSerial;
#endif
	}
			
};	

#endif

