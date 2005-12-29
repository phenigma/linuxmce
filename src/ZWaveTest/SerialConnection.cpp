#include "SerialConnection.h"
#include "main.h"

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


bool SerialConnection::isConnected()
{
	return serialPort != NULL;
}

int SerialConnection::send(char *buffer, unsigned int len)
{
	int returnValue = 0;
	if(buffer != NULL)
		if(instance->serialPort!= NULL)
		{
			try
			{
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

int SerialConnection::receiveCommand(char *b, unsigned int *len)
{
	if(b == NULL)
	{
		*len = 0;
		return -1;
	}

	pthread_mutex_lock( &instance->mutex_buffer );
	if(buffer.size() > 0)
	{
		if( buffer.front() == 0x06 )
			buffer.pop_front();
		if( buffer.front() != 0x01 )
		{
			*len = 0;
			return -1;
		}
		unsigned int buffered_len = *(buffer.begin() + 1);
		if(buffer.size() < buffered_len + 3 || *len < buffered_len + 3)
		{
			*len = 0;
			return -1;
		}
		unsigned int i = 0;
		while(i < buffered_len)
		{
			b[i++] = buffer.front();
			buffer.pop_front();
		}
		if( buffer.front() != checkSum(b, buffered_len) )
			*len = 0;
		else
			*len = buffered_len;
		buffer.pop_front();
	}
	else
	{
		*len = 0;
	}
	
	pthread_mutex_unlock( &instance->mutex_buffer );
	return 0;
}

int SerialConnection::hasCommand()
{
	pthread_mutex_lock( &instance->mutex_buffer );
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
			endPack = buffer.begin() + len + 2;
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
	pthread_mutex_unlock( &instance->mutex_buffer );
	return returnValue;
}


char SerialConnection::checkSum(char *b, int len)
{
	char returnValue = 0;
	if( b == NULL)
	{
		return returnValue;
	}
	for(int i=0; i< len; i++)
	{
		returnValue ^= b[i]; 
	}
	return returnValue;
}


void *SerialConnection::receiveFunction(void *)
{
	if(instance->isConnected())
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
