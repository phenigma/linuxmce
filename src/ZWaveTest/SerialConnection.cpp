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
/*		PTHREAD_MUTEX_INITIALIZER(mutex_receive);
		PTHREAD_MUTEX_INITIALIZER(mutex_send);
*/
		pthread_mutex_init(&mutex_receive, NULL);
		pthread_mutex_init(&mutex_send, NULL);
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
//				pthread_mutex_lock( mutex_send );
				serialPort->Write(buffer, len);
//				pthread_mutex_unlock( mutex_send );
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
	//todo: guard the access to the receive queue
	*len = 0;
	return 0;
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
//				pthread_mutex_lock( mutex_receive );
				/*size_t len =*/ instance->serialPort->Read(mybuf, 100);
//				pthread_mutex_unlock( mutex_receive );

//				pthread_mutex_lock( mutex_buffer );

				while(--len >= 0)
					instance->buffer.push(mybuf[len]);
				

//				pthread_mutex_unlock( mutex_buffer );
				
			}
		}
	}
	catch(...)
	{
		//todo: treat the exception
	}
	return NULL;
}
