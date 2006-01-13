#include "SerialConnection.h"
#include "main.h"

//#define DEBUG_EUGEN 1
#ifdef _WIN32 
#include <windows.h> 
#include <winbase.h> 
#define READ_DELAY 200 
#else 
#include <unistd.h> 
#define READ_DELAY 200000 
#endif

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

int SerialConnection::connect(const char *port)
{
	try
	{
		serialPort = new CSerialPort(port, 115200, epbsN81);
		pthread_mutex_init(&mutex_serial, NULL);
		pthread_mutex_init(&mutex_buffer, NULL);
		int stat = pthread_create(&write_thread, NULL, receiveFunction, (void*)this);
		if( stat != 0)
		{
			g_pPlutoLogger->Write(LV_WARNING, "receive thread not created");
			switch(stat)
			{
			case EAGAIN:
				g_pPlutoLogger->Write(LV_DEBUG, "EAGAIN received");
				break;
			case EINVAL:
				g_pPlutoLogger->Write(LV_DEBUG, "EINVAT received");
				break;
			default:
				g_pPlutoLogger->Write(LV_DEBUG, "%d received", stat);
			}
		}
		else
		{
			g_pPlutoLogger->Write(LV_DEBUG, "receive thread created OK!!!!");
		}
	}
	catch(...)
	{
		g_pPlutoLogger->Write(LV_WARNING, "exeception from serial port");
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

// TODO: use 'const char * buffer'
int SerialConnection::send(char *buffer, size_t len)
{
#ifdef DEBUG_EUGEN
	pthread_mutex_lock( &instance->mutex_buffer );
	g_pPlutoLogger->Write(LV_WARNING, "Send:");
	for(size_t i=0; i<len; i++)
	{
			g_pPlutoLogger->Write(LV_WARNING, "0x%02x ", buffer[i]);
	}
	char *paddedBuffer = NULL;
	len += 3;
	paddedBuffer = new char((int)len);
	paddedBuffer[0] = SERIAL_SOF; 
	paddedBuffer[1] = (char)len - 2;
	memcpy(&(paddedBuffer[2]), buffer, len - 3);
	paddedBuffer[(int)len - 1] = checkSum(paddedBuffer + 1, len - 2);

	delete paddedBuffer;
	paddedBuffer = NULL;
	g_pPlutoLogger->Write(LV_WARNING, "--------------");
	g_pPlutoLogger->Flush();
	pthread_mutex_unlock( &instance->mutex_buffer );
	
	return 0;
#else
	//pad the command with everything that is needed (SOF, LEN, CHECKSUM)

	int returnValue = 0;
	if(buffer != NULL)
		if(instance->serialPort!= NULL)
		{
			try
			{
				char *paddedBuffer = NULL;
				len += 3;
				paddedBuffer = new char((int)len);
				paddedBuffer[0] = SERIAL_SOF; 
				paddedBuffer[1] = (char)len - 2;
				memcpy(&(paddedBuffer[2]), buffer, len - 3);
				paddedBuffer[(int)len - 1] = checkSum(paddedBuffer + 1, len - 2);
				pthread_mutex_lock( &mutex_serial );
				serialPort->Write(paddedBuffer, len);
				pthread_mutex_unlock( &mutex_serial );
				delete paddedBuffer;
				paddedBuffer = NULL;
			}
			catch(...)
			{
				returnValue = -1;
				g_pPlutoLogger->Write(LV_WARNING, "exeception from serial port");
			}
		}
		else returnValue = -1;
	else returnValue = -1;

	return returnValue ;
#endif
}

int SerialConnection::receiveCommand(char *b, size_t *len)
{
	if(b == NULL)
	{
		*len = 0;
		return -1;
	}

	pthread_mutex_lock( &instance->mutex_buffer );
	if(buffer.size() > 0)
	{
		//for(std::deque<char>::iterator it = buffer.begin(); it != buffer.end(); ++it)
		//{
		//	g_pPlutoLogger->Write(LV_WARNING, "HMM: 0x%02x ", (int)(*it));
		//}
		
		if( buffer.front() == 0x06 )
			buffer.pop_front();
		if( buffer.front() != 0x01 )
		{
			*len = 0;
			return -1;
		}
		unsigned int buffered_len = *(buffer.begin() + 1);
		if(buffer.size() < buffered_len + 2 || *len < buffered_len + 2)
		{
			*len = 0;
			return -1;
		}
		
		// SOF
		buffer.pop_front();
		// LEN
		buffer.pop_front();
		
		unsigned int i = 0;
		*len = buffered_len - 1;
		while(i < *len)
		{
			b[i++] = buffer.front();
			buffer.pop_front();
		}
		
		// CHECKSUM
		buffer.pop_front();
	}
	else
	{
		*len = 0;
	}
	
	pthread_mutex_unlock( &instance->mutex_buffer );
	return 0;
}

/**
 * 
 * @return 
 */
int SerialConnection::hasCommand()
{
	pthread_mutex_lock( &instance->mutex_buffer );
	int returnValue = 1;
	if(buffer.size() < 4)
	{
		pthread_mutex_unlock( &instance->mutex_buffer );
		return 0;
	}
	while( buffer.front() == SERIAL_ACK )
		buffer.pop_front();
	if( buffer.front() == SERIAL_SOF )
	{
		std::deque<char>::iterator i = buffer.begin();
		unsigned int len = *(++i) ;
		if(buffer.size() >= len + 2) //SOF+LEN+CHECK
		{
			i += len;
			char checkSum = *i;
			char tmpSum = (char)0xff;
			// checksum( Length_Byte + Command_Array_Bytes )
			std::deque<char>::iterator endPack = buffer.begin() + len + 1;
			for(i = buffer.begin() + 1; i != endPack; i++)
				tmpSum ^= *i; 
			if(tmpSum == checkSum)
			{
				returnValue = 1;
				char ack = SERIAL_ACK;
				pthread_mutex_lock( &mutex_serial );
#ifdef DEBUG_EUGEN
				g_pPlutoLogger->Write(LV_WARNING, "Send ACK");
#else
				serialPort->Write(&ack, 1);
#endif
				pthread_mutex_unlock( &mutex_serial );
			}
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
	char returnValue = (char)0xff;
	if( b == NULL)
	{
		return returnValue;
	}
	for(int i=0; i< len; i++)
	{
		returnValue ^= b[i]; 
//		g_pPlutoLogger->Write(LV_WARNING, "b[%d]=%x", i, b[i]);
	}
	return returnValue;
}


void *SerialConnection::receiveFunction(void *)
{
	g_pPlutoLogger->Write(LV_WARNING, "entry point receiveFUnction");
	g_pPlutoLogger->Flush();
	//printf("entry point receiveFUnction");
	if(instance->isConnected())
	{
		char mybuf[1024];
		size_t len = 1024;
#ifdef DEBUG_EUGEN
		pthread_mutex_lock( &instance->mutex_buffer );
		FILE * serialData = fopen("serial.txt", "r");
		if( serialData != NULL )
		{
			while( NULL != fgets(mybuf, (int)len, serialData) )
			{
				unsigned nr = 0;
				size_t i = 0;
				size_t iLength = strlen(mybuf);
				g_pPlutoLogger->Write(LV_WARNING, "Get:");
				while( i < iLength && 0 != sscanf(&mybuf[i], "%x", &nr) )
				{
					i += 3;
					g_pPlutoLogger->Write(LV_WARNING, "0x%02x ", nr);
					instance->buffer.push_back((unsigned char)nr);
				}
				g_pPlutoLogger->Write(LV_WARNING, "---------------");
				g_pPlutoLogger->Flush();
			}
			
			fclose(serialData);
		}
		pthread_mutex_unlock( &instance->mutex_buffer );
#else
		while(true)
		{
			len = 100;
			pthread_mutex_lock( &instance->mutex_serial );
			g_pPlutoLogger->Write(LV_WARNING, "before read");
			len = instance->serialPort->Read(mybuf, 100);
			g_pPlutoLogger->Write(LV_WARNING, "after read");
			pthread_mutex_unlock( &instance->mutex_serial );

			pthread_mutex_lock( &instance->mutex_buffer );
			while(--len >= 0)
				instance->buffer.push_back(mybuf[len]);
			pthread_mutex_unlock( &instance->mutex_buffer );
			
#ifdef _WIN32 	
			Sleep(READ_DELAY); 
#else 	
			usleep(READ_DELAY); 
#endif //_WIN32
			//TODO delay
		}
#endif // DEBUG_EUGEN
	}
	return NULL;
}

SerialConnection::~SerialConnection()
{
	pthread_exit(&write_thread);
	pthread_mutex_destroy(&mutex_serial);
	pthread_mutex_destroy(&mutex_buffer);
	delete serialPort;
	serialPort = NULL;
	instance = NULL;
}
