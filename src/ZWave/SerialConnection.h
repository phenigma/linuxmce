//
// C++ Interface: PlutoZWSerialAPI
//
//
// Author:	Eugen Constantinescu <eugen.c@plutohome.com>, (C) 2006
//  		Edgar Grimberg <edgar.g@plutohome.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef __SERIALCONNECTION_H
#define __SERIALCONNECTION_H

#include "DCE/ServerLogger.h"
#include "Serial/SerialPort.h" 
#include <deque>
#include <pthread.h>

#define SERIAL_ACK 0x06
#define SERIAL_SOF 0x01

/**
* Singleton class that takes care of the serial connection interface. 
*Send is single thread, receive is multi thread.
*/
class SerialConnection
{
public:
	/**gets an instance of this class
	* @return the instance of this class*/
	static SerialConnection* getInstance();
	
	/**force closing the serial connection*/
	static void forceClose();

	/**send a buffer to the serial port
	*@param buffer the buffer to be sent
	*@param len the length of the buffer
	*@return 0 if OK, else negative */
	virtual int send(char *buffer, size_t len);
	
	/**try to get the specified len from the serial buffer
	* @parameter buffer the buffer where the data should be found
	* @param len the len that should be read. After the call, len will contain the actual read data length*/
	virtual int receiveCommand(char *buffer, size_t *len);

	/**disconnect from the serial port
	*@return 0*/
	virtual int disconnect();

	/**connect to the serial port
	*@param port the serial port connected to the ZWave device 
	*(eg.: in windows can be "COM1", in linux "ttyS0")
	*@return 0 if successfull, else negative*/
	virtual int connect(const char *port);
	
	/**checks if the serial connection class has been initialized*/
	virtual bool isConnected();

	/**checks if there is a full response in the serial buffer
	*@return true if there is a full response in the buffer*/
	virtual int hasCommand();
	
	/** Use CAREFULLY !
	  * Clears serial buffer.
	  */
	virtual void clearSerialBuffer();
	
	virtual ~SerialConnection();


	/**function that will be called in a new pthread and will receive 
	 * the bytes from the serial connection*/
	static void* receiveFunction(void *);
	
	static void printDataBuffer(const char *buffer, const size_t length, const char *classID);

	static pthread_mutex_t mutex_serial;
	static pthread_mutex_t mutex_buffer;
	static pthread_t write_thread;
	
	static bool ack;
	
private:
	/**static instance of this class*/
	static SerialConnection* instance;
	
	/**private constructor*/
	SerialConnection();

	/**the serial port*/
	CSerialPort* serialPort;

	/**receive queue*/
	std::deque<char> buffer;


	
	char SerialConnection::checkSum(char *b, int len);
};

#endif
