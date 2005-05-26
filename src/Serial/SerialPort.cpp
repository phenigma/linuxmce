#include "SerialPort.h"

#ifdef WIN32


CSerialPort::CSerialPort(string Port, unsigned BPS, eParityBitStop ParityBitStop, bool FlowControl)
{
    m_Serio.port=NULL;
	if (serio_open(&m_Serio, BPS, Port.c_str(), NOPARITY, 8, ONESTOPBIT)!=comm_STATUS_OK)
	{
		throw string("Couldn't open serial port "+Port+".");
	}    
}

CSerialPort::~CSerialPort()
{
	serio_close(&m_Serio);
}

void CSerialPort::Write(char *Buf, size_t Len)
{
	serio_write(&m_Serio, Buf, Len);
}

size_t CSerialPort::Read(char *Buf, size_t MaxLen, int Timeout)
{
	size_t ReturnValue = 0;
	serio_read(&m_Serio, Buf, MaxLen, &ReturnValue);
	return ReturnValue;
}

bool CSerialPort::IsReadEmpty()
{
    Sleep(0);    
    return FALSE;
}

void CSerialPort::Flush() {
}


#else

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>

CSerialPort::CSerialPort(string Port, unsigned BPS, eParityBitStop ParityBitStop, bool FlowControl)
{
    struct termios t;
    
    if ((m_fdSerial = open(string("/dev/"+Port).c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK)) < 0)
    {
	if (Port == "ttyS5")
		system("mknod /dev/ttyS5 c 4 69");	    
        if (Port == "ttyS4")
		system("mknod /dev/ttyS4 c 4 68");
	
	if ((m_fdSerial = open(string("/dev/"+Port).c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK)) < 0)
		throw string("Cannot open ")+Port+string("\r\n");
    }
    memset(&t, 0, sizeof(struct termios));
    t.c_iflag = IGNBRK | INPCK;
    t.c_oflag = 0;
    // TODO: Implement paritybitstop
	switch(ParityBitStop) {
	case epbsN81:
	    t.c_cflag = CS8 | CREAD | CLOCAL;
		break;
	case epbsE81:
	    t.c_cflag = CS8 | CREAD | CLOCAL | PARENB;
		break;
	case epbsO81:
	    t.c_cflag = CS8 | CREAD | CLOCAL | PARENB | PARODD;
		break;
	default:
	    t.c_cflag = CS8 | CREAD | CLOCAL/* | CSTOPB*/;
		break;
	}
	if (FlowControl)
		t.c_cflag |= CRTSCTS;
    t.c_lflag = 0;
    switch(BPS)
    {
    case 300:
        cfsetispeed(&t,B300);
        cfsetospeed(&t,B300);
        break;
    case 1200:
        cfsetispeed(&t,B1200);
        cfsetospeed(&t,B1200);
        break;
    case 2400:
        cfsetispeed(&t,B2400);
        cfsetospeed(&t,B2400);
        break;
    case 4800:
        cfsetispeed(&t,B4800);
        cfsetospeed(&t,B4800);
        break;
    case 9600:
        cfsetispeed(&t,B9600);
        cfsetospeed(&t,B9600);
        break;
    case 19200:
        cfsetispeed(&t,B19200);
        cfsetospeed(&t,B19200);
        break;
    case 38400:
        cfsetispeed(&t,B38400);
        cfsetospeed(&t,B38400);
	break;
    case 57600:
        cfsetispeed(&t,B57600);
        cfsetospeed(&t,B57600);
	break;
    case 115200:
        cfsetispeed(&t,B115200);
        cfsetospeed(&t,B115200);
    default:
        throw string("Baud rate not supported");
    }
    tcsetattr(m_fdSerial,TCSANOW,&t);
}

CSerialPort::~CSerialPort()
{
    close(m_fdSerial);  
}

void CSerialPort::Write(char *Buf, size_t Len)
{
   int Timeout = 2000;
   int BytesWritten = 0;
   time_t end, start = time(NULL);

   do
   {
		fd_set wrfds;
		FD_ZERO(&wrfds);
		FD_SET(m_fdSerial, &wrfds);
		int ret;
		struct timeval tv;
		tv.tv_sec = Timeout / 1000 + 1;
		tv.tv_usec = 0;
		do
		{
   			ret = select(m_fdSerial+1, NULL, &wrfds, NULL, &tv);
		} while(ret != -1 && ret != 1);
		if (ret == 0 || ret ==-1)
		{ 
    			return;
		}  
		BytesWritten += write(m_fdSerial, Buf, Len);
		end=time(NULL);
	} while(BytesWritten < Len && (end-start) < (Timeout/1000 +1));
}

size_t CSerialPort::Read(char *Buf, size_t MaxLen, int Timeout)
{
    struct timeval tv;
    fd_set rfds;

    FD_ZERO(&rfds);
    FD_SET(m_fdSerial, &rfds);
	int ret;
    
    tv.tv_sec = Timeout/1000 ;
    tv.tv_usec = (Timeout % 1000) * 1000;

    ret = select(m_fdSerial+1, &rfds, NULL, NULL, &tv);
    if (ret <= 0) { 
    	return -1;
    }   
    size_t retval = read(m_fdSerial,Buf,MaxLen);
    return retval;   
}

bool CSerialPort::IsReadEmpty()
{
    struct timeval tv;
	fd_set rfds;
		
	FD_ZERO(&rfds);
	FD_SET(m_fdSerial, &rfds);
	int ret;
					
	tv.tv_sec = 0 ;
	tv.tv_usec = 0;
							
    ret = select(m_fdSerial+1, &rfds, NULL, NULL, &tv);
    if (ret == 0 || ret == -1) {
		return true;
    } else {
		return false;
	}
}


void CSerialPort::Flush() {
    tcflush(m_fdSerial, TCIOFLUSH);
}


#endif
