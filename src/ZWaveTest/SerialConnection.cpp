#include "SerialConnection.h"

SerialConnection* SerialConnection::instance = NULL;

SerialConnection::SerialConnection():serialPort(NULL)
{
	
}

SerialConnection *SerialConnection::getInstance()
{
	if(instance == NULL)
	{
		instance = new SerialConnection();
	}
	return instance;
}

int SerialConnection::connect()
{
	try
	{
		serialPort = new CSerialPort("COM1", 9600, epbsN81);
		pthread_mutex_init(&mutex_serial, NULL);
		pthread_mutex_init(&mutex_buffer, NULL);
		
		pthread_create(&write_thread, NULL, &receiveFunction, NULL);
	}
	catch(...)
	{
		return -1;
	}
	return 0;
}

int SerialConnection::disconnect()
{
	delete serialPort;
	serialPort = NULL;
	return 0;
}

int SerialConnection::send(char *buffer, unsigned int len)
{
	int returnValue = 0;
	if(buffer != NULL)
		if(instance->serialPort!= NULL)
		{
			try
			{
				//todo: guard the access to the serial port
				pthread_mutex_lock( &mutex_serial );
				serialPort->Write(buffer, len);
				pthread_mutex_unlock( &mutex_serial );
			}
			catch(...)
			{
				returnValue = -1;
			}
		}
		else returnValue = -1;
	else returnValue = -1;

	return returnValue ;
}

int SerialConnection::receive(char *buffer, unsigned int *len)
{
	//todo guard the access to the receive queue
	*len = 0;
	return 0;
}

int SerialConnection::hasCommand()
{
	int returnValue = 1;
	if(buffer.size() < 4)
		return 0;
	if( buffer.front() == 0x06 )
		buffer.pop_front();
	if( buffer.front() == 0x01 )
	{
		std::deque<char>::iterator i = buffer.begin();
		unsigned int len = *(++i) ;
		if(buffer.size() >= len + 3) //SOF+LEN+CHECK
		{
			i += len + 1;
			char checkSum = *i;
			char tmpSum = 0;
			std::deque<char>::iterator endPack;
			for(i = buffer.begin() + 2; i != endPack; i++)
				tmpSum ^= *i; 
			if(tmpSum == checkSum)
				returnValue = 1;
			else
				returnValue = 0;
		}
		else
			returnValue = 0;
	}
	else 
		returnValue = -1;
	return returnValue;
}

void *SerialConnection::receiveFunction(void *)
{
	try
	{
		if(instance->serialPort != NULL)
		{
			char mybuf[100];
			int len = 100;
			while(true)
			{
				len = 100;
				pthread_mutex_lock( &instance->mutex_serial );
				size_t len = instance->serialPort->Read(mybuf, 100);
				pthread_mutex_unlock( &instance->mutex_serial );

				pthread_mutex_lock( &instance->mutex_buffer );
				while(--len >= 0)
					instance->buffer.push_back(mybuf[len]);
				pthread_mutex_unlock( &instance->mutex_buffer );
			}
		}
	}
	catch(...)
	{
		//todo treat the exception
	}
	return NULL;
}

SerialConnection::~SerialConnection()
{
	pthread_exit(write_thread);
	pthread_mutex_destroy(&mutex_serial);
	pthread_mutex_destroy(&mutex_buffer);
	delete serialPort;
	serialPort = NULL;
	instance = NULL;
}
