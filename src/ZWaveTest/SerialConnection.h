#ifndef __SERIALCONNECTION_H
#define __SERIALCONNECTION_H

#include "DCE/ServerLogger.h"
#include "Serial/SerialPort.h" 
#include <deque>
#include <pthread.h>

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

	/**send a buffer to the serial port
	*@param buffer the buffer to be sent
	*@param len the length of the buffer
	*@return 0 if OK, else negative */
	int send(char *buffer, unsigned int len);
	
	/**try to get the specified len from the serial buffer
	* @parameter buffer the buffer where the data should be found
	* @param len the len that should be read. After the call, len will contain the actual read data length*/
	int receive(char *buffer, unsigned int *len);

	/**disconnect from the serial port
	*@return 0*/
	int SerialConnection::disconnect();

	/**connect to the serial port
	*@return 0 if successfull, else negative*/
	int SerialConnection::connect();

	/**checks if there is a full response in the serial buffer
	*@return true if there is a full response in the buffer*/
	int SerialConnection::hasCommand();

	~SerialConnection();

private:
	/**static instance of this class*/
	static SerialConnection* instance;
	
	/**private constructor*/
	SerialConnection();

	/**the serial port*/
	CSerialPort* serialPort;

	/**receive queue*/
	std::deque<char> buffer;


	pthread_mutex_t mutex_serial;
	pthread_mutex_t mutex_buffer;
	pthread_t write_thread;

	static void* receiveFunction(void *);

	char SerialConnection::checkSum(char *b, int len);
};

#endif
