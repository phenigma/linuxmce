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

pthread_mutex_t SerialConnection::mutex_serial;
pthread_mutex_t SerialConnection::mutex_buffer;
pthread_t SerialConnection::write_thread;

SerialConnection::SerialConnection()
	: serialPort(NULL)
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

void SerialConnection::forceClose()
{
#ifdef PLUTO_DEBUG
	g_pPlutoLogger->Write(LV_DEBUG, "SerialConnection ------------- 3");
#endif
	delete instance;
	instance = NULL;
#ifdef PLUTO_DEBUG
	g_pPlutoLogger->Write(LV_DEBUG, "SerialConnection ------------- 4");
#endif
}

int SerialConnection::connect(const char *port)
{
	try
	{
		serialPort = new CSerialPort(port, 115200, epbsN81);
		
#ifdef PLUTO_DEBUG
		g_pPlutoLogger->Write(LV_ZWAVE, "SerialConnection::connect init serial mutex");
#endif

		pthread_mutex_init(&mutex_serial, NULL);
		pthread_mutex_init(&mutex_buffer, NULL);
		int stat = pthread_create(&write_thread, NULL, receiveFunction, (void*)this);
		if( stat != 0)
		{
			g_pPlutoLogger->Write(LV_WARNING, "receive thread not created");
			switch(stat)
			{
			case EAGAIN:
#ifdef PLUTO_DEBUG
				g_pPlutoLogger->Write(LV_ZWAVE, "EAGAIN received");
#endif
				break;
			case EINVAL:
#ifdef PLUTO_DEBUG
				g_pPlutoLogger->Write(LV_ZWAVE, "EINVAT received");
#endif
				break;
			default:
#ifdef PLUTO_DEBUG
				g_pPlutoLogger->Write(LV_ZWAVE, "%d received", stat);
#endif
				break;
			}
		}
		else
		{
#ifdef PLUTO_DEBUG
			g_pPlutoLogger->Write(LV_ZWAVE, "receive thread created OK!!!!");
#endif
		}

#ifdef _WIN32 	
			Sleep(READ_DELAY); 
#else 	
			usleep(READ_DELAY); 
#endif //_WIN32

	}
	catch(...)
	{
		serialPort = NULL;
		g_pPlutoLogger->Write(LV_WARNING, "exeception from serial port");
		return -1;
	}
	return 0;
}

int SerialConnection::disconnect()
{
	if( serialPort != NULL )
	{
#ifdef PLUTO_DEBUG
		g_pPlutoLogger->Write(LV_DEBUG, "SerialConnection ------------- asa 1");
#endif
		pthread_mutex_lock( &instance->mutex_serial );
		delete serialPort;
		serialPort = NULL;
		pthread_mutex_unlock( &instance->mutex_serial );
#ifdef PLUTO_DEBUG
		g_pPlutoLogger->Write(LV_DEBUG, "SerialConnection ------------- asa 2");
		g_pPlutoLogger->Flush();
#endif
		
// wait a bit so that the write_thread will finish his task
#ifdef PLUTO_DEBUG
		g_pPlutoLogger->Write(LV_ZWAVE, "waiting for thread to finish");
		g_pPlutoLogger->Flush();
#endif
		pthread_join(write_thread, NULL);
#ifdef PLUTO_DEBUG
		g_pPlutoLogger->Write(LV_ZWAVE, "thread finished");
		g_pPlutoLogger->Flush();
#endif

		pthread_mutex_destroy(&mutex_serial);
		pthread_mutex_destroy(&mutex_buffer);
		
		buffer.clear();
		
#ifdef PLUTO_DEBUG
		g_pPlutoLogger->Write(LV_DEBUG, "SerialConnection ------------- asa 3");
#endif
	}
	
#ifdef PLUTO_DEBUG
		g_pPlutoLogger->Write(LV_DEBUG, "SerialConnection ------------- asa 4");
		g_pPlutoLogger->Flush();
#endif
	return 0;
}


inline bool SerialConnection::isConnected()
{
	return serialPort != NULL;
}

// TODO: use 'const char * buffer'
int SerialConnection::send(char *b, size_t len)
{
	if( !isConnected() )
	{
		return -1;
	}
	
	//pad the command with everything that is needed (SOF, LEN, CHECKSUM)

	int returnValue = 0;
	if(b != NULL)
	{
#ifdef PLUTO_DEBUG
		g_pPlutoLogger->Write(LV_ZWAVE, "SerialConnection::send lock serial mutex");
#endif
		pthread_mutex_lock( &mutex_serial );
		
#ifdef PLUTO_DEBUG
		printDataBuffer(b, len, "SerialConnection::send");
#endif
	
		if(instance->serialPort!= NULL)
		{
			try
			{
				len += 3;
				// 256 + 2 should be enough, the length is one byte
				char paddedBuffer[512];
				paddedBuffer[0] = SERIAL_SOF; 
				paddedBuffer[1] = (char)len - 2;
				memcpy(&(paddedBuffer[2]), b, len - 3);
				paddedBuffer[(int)len - 1] = checkSum(&(paddedBuffer[1]), (int)len - 2);
				
				serialPort->Write(paddedBuffer, len);
			}
			catch(...)
			{
				returnValue = -1;
				g_pPlutoLogger->Write(LV_WARNING, "exeception from serial port");
			}
		}
		else returnValue = -1;
		pthread_mutex_unlock( &mutex_serial );
#ifdef PLUTO_DEBUG
		g_pPlutoLogger->Write(LV_ZWAVE, "SerialConnection::send unlock serial mutex");
#endif
	}
	else returnValue = -1;

	return returnValue ;
}

int SerialConnection::receiveCommand(char *b, size_t *len)
{
#ifdef PLUTO_DEBUG
	g_pPlutoLogger->Write(LV_DEBUG, "receiveCommand::begin");
#endif
	
	if(b == NULL || !isConnected())
	{
		*len = 0;
		return -1;
	}
	
	pthread_mutex_lock( &instance->mutex_buffer );
	if( !buffer.empty() )
	{
#ifdef PLUTO_DEBUG
		g_pPlutoLogger->Write(LV_DEBUG, "buffer.size()= %d", buffer.size());
#endif
		//for(std::deque<char>::iterator it = buffer.begin(); it != buffer.end(); ++it)
		//{
		//	g_pPlutoLogger->Write(LV_WARNING, "HMM: 0x%02x ", (int)(*it));
		//}
		
		if( buffer.front() == SERIAL_ACK )
			buffer.pop_front();
		if( buffer.front() != SERIAL_SOF )
		{
			pthread_mutex_unlock( &instance->mutex_buffer );
			*len = 0;
			return -1;
		}
		unsigned int buffered_len = *(buffer.begin() + 1);
		if(buffer.size() < buffered_len + 2 || *len < buffered_len + 2)
		{
			pthread_mutex_unlock( &instance->mutex_buffer );
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
#ifdef PLUTO_DEBUG
			if( i == 0 )
				g_pPlutoLogger->Write(LV_DEBUG, "receiveCommand: eat another command");
#endif			
			b[i++] = buffer.front();
			buffer.pop_front();
		}
		
		// CHECKSUM
		buffer.pop_front();
		
#ifdef PLUTO_DEBUG
		printDataBuffer(b, *len, "SerialConnection : received command");
#endif			
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
#ifdef PLUTO_DEBUG
	g_pPlutoLogger->Write(LV_DEBUG, "SerialConnection::hasCommand() entry point");
#endif
	if( !isConnected() )
	{
		g_pPlutoLogger->Write(LV_ZWAVE, "SerialConnection::hasCommand() is not connected");
		return -1;
	}
	
	int returnValue = 0;
#ifdef PLUTO_DEBUG
	g_pPlutoLogger->Write(LV_DEBUG, "SerialConnection::hasCommand() lock mutex_buffer");
#endif
	pthread_mutex_lock( &instance->mutex_buffer );
	while( 0 < buffer.size() && buffer.front() == SERIAL_ACK )
	{
#ifdef PLUTO_DEBUG
		g_pPlutoLogger->Write(LV_DEBUG, "SerialConnection::hasCommand() popping ACK");
#endif
		buffer.pop_front();
	}
	
	if(buffer.size() < 3)
	{
#ifdef PLUTO_DEBUG
		g_pPlutoLogger->Write(LV_DEBUG, "size too small %d", buffer.size());
#endif
		pthread_mutex_unlock( &instance->mutex_buffer );
		return 0;
	}
	
	if( buffer.front() == SERIAL_SOF )
	{
#ifdef PLUTO_DEBUG
		g_pPlutoLogger->Write(LV_DEBUG, "SerialConnection::hasCommand() front is SOF");
#endif
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
#ifdef PLUTO_DEBUG
				g_pPlutoLogger->Write(LV_DEBUG, "SerialConnection::hasCommand lock serial mutex");
#endif
				pthread_mutex_lock( &mutex_serial );

				if( serialPort != NULL )
				{
					serialPort->Write(&ack, 1);
#ifdef PLUTO_DEBUG
					g_pPlutoLogger->Write(LV_DEBUG, "SerialConnection::hasCommand wrote ack");
#endif
				}
				pthread_mutex_unlock( &mutex_serial );
#ifdef PLUTO_DEBUG
				g_pPlutoLogger->Write(LV_DEBUG, "SerialConnection::hasCommand unlock serial mutex");
#endif
			}
		}
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
#ifdef PLUTO_DEBUG
	g_pPlutoLogger->Write(LV_DEBUG, "entry point receiveFunction");
#endif

	//g_pPlutoLogger->Flush();
	//printf("entry point receiveFUnction");
	if(instance->serialPort != NULL)
	{
		char mybuf[1024];
		size_t len = sizeof(mybuf);
		bool bDisconnect = false;
		while( true )
		{
			len = sizeof(mybuf) + 1;
			pthread_mutex_lock( &instance->mutex_serial );
			if( instance != NULL && instance->isConnected() )
			{
				len = instance->serialPort->Read(mybuf, sizeof(mybuf), 20);
			}
			else
			{
				// THREAD MUST STOP
				bDisconnect = true;
			}
			pthread_mutex_unlock( &instance->mutex_serial );
			
			// THREAD MUST STOP
			if( bDisconnect )
			{
				break;
			}
			
			if(len <= sizeof(mybuf) && len != 0 && instance != NULL)
			{
				pthread_mutex_lock( &instance->mutex_buffer );
				for(size_t i=0; i<len; i++)
				{
					instance->buffer.push_back(mybuf[i]);
				}
				//SerialConnection::printDataBuffer(mybuf, len, "Thread Read");
				pthread_mutex_unlock( &instance->mutex_buffer );
			}
			
#ifdef _WIN32 	
			Sleep(READ_DELAY);
#else 
			usleep(READ_DELAY); 
#endif //_WIN32
		}

#ifdef PLUTO_DEBUG
	g_pPlutoLogger->Write(LV_DEBUG, "exit receiveFunction");
#endif

		pthread_exit(NULL);
	}
	return NULL;
}

void SerialConnection::printDataBuffer(const char *buffer, const size_t length, const char *classID)
{
#ifdef PLUTO_DEBUG
	char log_buf[1024 * 5];
	memset(log_buf, 0, sizeof(log_buf));
	for(unsigned int ww = 0; ww < length && ww < sizeof(log_buf); ww++)
		sprintf(&(log_buf[ww*5]), "0x%02x ", buffer[ww]);
	g_pPlutoLogger->Write(LV_DEBUG, "%s len = %d buf = %s", classID, length, log_buf);
#endif
}

SerialConnection::~SerialConnection()
{
#ifdef PLUTO_DEBUG
	g_pPlutoLogger->Write(LV_DEBUG, "SerialConnection ------------- 1");
#endif
	if( serialPort != NULL )
	{
		disconnect();
	}
#ifdef PLUTO_DEBUG
	g_pPlutoLogger->Write(LV_DEBUG, "SerialConnection ------------- 2");
#endif
}
