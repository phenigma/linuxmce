#ifndef SYMBIAN
	#include "PlutoUtils/CommonIncludes.h"
#endif

#include "PlutoUtils/MultiThreadIncludes.h"
#include "Logger.h"
#include "PlutoUtils/MyStl.h"
#include "BD/BDCommandProcessor.h"
#include "BD/BDCommandProcessor_Windows_Bluetooth.h"

#include "Socket.h"

using namespace DCE;

#define RECV_TIMEDOUT 15 //seconds

BDCommandProcessor_Windows_Bluetooth::BDCommandProcessor_Windows_Bluetooth(string sMacAddressPhone,string sMacAddressDongle)
	: BDCommandProcessor(sMacAddressPhone), m_ReceiveBufferMutex("receive buffer")
{
	m_IsConnected = false;
	m_pReceiveBuffer=NULL;
	m_iSizeReceiveBuffer=0;
	m_ReceiveBufferMutex.Init(NULL);
	pthread_cond_init(&m_PollingCond, NULL);

	m_ptr_rfcm = new CRfCommTransport( this );

	int RetryCount=0;
    while( !m_IsConnected && RetryCount++<5)
	{
		m_IsConnected = m_ptr_rfcm->TransportOpen( sMacAddressPhone.c_str() );
		if( !m_IsConnected )
		{
			g_pPlutoLogger->Write(LV_CRITICAL,"WIDCOM failed to open RFCOM Port.  Try #%d",RetryCount);
			Sleep(1000);
		}
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	if( !m_IsConnected )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Failed to open bluetooth device");
		m_bExit=m_bDead=true;
	}
	else
		m_bDead = false;
}

BDCommandProcessor_Windows_Bluetooth::~BDCommandProcessor_Windows_Bluetooth()
{
	if ( m_IsConnected == true )
	{
		m_IsConnected = false;
		m_ptr_rfcm->TransportClose();
		delete m_ptr_rfcm;
		m_ptr_rfcm = NULL;
	}

	if ( m_ptr_rfcm != NULL )
	{
		delete m_ptr_rfcm;
		m_ptr_rfcm = NULL;
	}
}




void BDCommandProcessor_Windows_Bluetooth::OnTranspDataReceived( BYTE* data, ULONG len )
{
	g_pPlutoLogger->Write(LV_STATUS,"Received data %d bytes long.  buffer was %d",(int) len,(int) m_iSizeReceiveBuffer);
	PLUTO_SAFETY_LOCK(rb,m_ReceiveBufferMutex);
	if( m_pReceiveBuffer)
		m_pReceiveBuffer=(char *)realloc(m_pReceiveBuffer,m_iSizeReceiveBuffer+len);
	else
	{
		m_pReceiveBuffer=(char *)malloc(len);
		::ZeroMemory(m_pReceiveBuffer, len);
	}

	memcpy( m_pReceiveBuffer+m_iSizeReceiveBuffer, data, len );
	m_iSizeReceiveBuffer+=len;
	pthread_cond_broadcast(&m_PollingCond);
}

bool BDCommandProcessor_Windows_Bluetooth::SendData(int size,const char *data)
{
	g_pPlutoLogger->Write(LV_STATUS,"Sending %d bytes of data", size);

	m_ptr_rfcm->TransportWrite( (BYTE *)data, size );

	if(NULL == data)
	{
		m_bDead = true;
		return false;
	}

	g_pPlutoLogger->Write(LV_STATUS,"Sent %d bytes of data", size);

	return true;
}

// The framework will call this function, which should just block the thread until that many characters
// are received.  
char *BDCommandProcessor_Windows_Bluetooth::ReceiveData(int size)
{
	g_pPlutoLogger->Write(LV_STATUS,"Request for data %d bytes long buffer has %d",(int) size,(int) m_iSizeReceiveBuffer);
	// Wait until we get that many characters in our buffer
	while( (int) m_iSizeReceiveBuffer<size )
	{
/*
		// We might want to make this a timed wait and return NULL after a given period.
		// For some reason there appears to be a problem with timed_conditions, and they always
		// return immediately.  This will release the mutex.
// hack
rb.Release();
Sleep(100);
rb.Relock();
//		pthread_cond_wait(&m_PollingCond,&m_PollingMutex.mutex);
	}
*/
		pthread_mutex_lock(&m_PollingMutex.mutex);

		timespec abstime;
		abstime.tv_sec = time(NULL) + RECV_TIMEDOUT;
		abstime.tv_nsec = 0;
		if(ETIMEDOUT == pthread_cond_timedwait(&m_PollingCond,&m_PollingMutex.mutex, &abstime))
		{
			m_bDead = true;
			g_pPlutoLogger->Write(LV_WARNING,"Receive data timed_out! The connection is down.");
			pthread_mutex_unlock(&m_PollingMutex.mutex);

			return NULL;
		}

		pthread_mutex_unlock(&m_PollingMutex.mutex);
	}

	PLUTO_SAFETY_LOCK(rb,m_ReceiveBufferMutex);

	g_pPlutoLogger->Write(LV_STATUS,"returning %d of data",(int) size);
	// We're holding the mutex and have the data.  The caller will have to delete the buffer
	char *Buffer = (char *)malloc(size);
	memcpy(Buffer,m_pReceiveBuffer,size);
	if( (int) m_iSizeReceiveBuffer>size )
	{
		// There's still more data in the buffer
		char *NewBuffer = (char *)malloc(m_iSizeReceiveBuffer-size);
		memcpy(NewBuffer,m_pReceiveBuffer+size,m_iSizeReceiveBuffer-size);
		delete m_pReceiveBuffer;
		m_pReceiveBuffer = NewBuffer;
		m_iSizeReceiveBuffer-=size;
	}
	else
	{
		delete m_pReceiveBuffer;
		m_pReceiveBuffer = NULL; 
		m_iSizeReceiveBuffer=0;
	}


	return Buffer;
}

