#include "SerialPort.h"

#ifdef WIN32

#include "PlutoUtils/MultiThreadIncludes.h"
FILE *filer,*filew;
CSerialPort::CSerialPort(string Port, unsigned BPS, eParityBitStop ParityBitStop, bool FlowControl)
{
filer=fopen("C:\\serialread.cpp","wb");
filew=fopen("C:\\serialwrite.txt","wb");
    m_Serio.port=NULL;
	if (serio_open(&m_Serio, BPS, Port.c_str(), NOPARITY, 8, ONESTOPBIT)!=comm_STATUS_OK)
	{
		throw string("Couldn't open serial port "+Port+".");
	}    
}

CSerialPort::~CSerialPort()
{
fclose(filer);
fclose(filew);
	serio_close(&m_Serio);
}

void CSerialPort::Write(char *Buf, size_t Len)
{
fwrite(Buf,Len,1,filew);
	serio_write(&m_Serio, Buf, Len);
}

size_t CSerialPort::Read(char *Buf, size_t MaxLen, int Timeout)
{
	timespec tv_now, tv_timeout;
	gettimeofday(&tv_now, NULL);
	tv_timeout = tv_now;
	tv_timeout += Timeout;

	size_t ReadSoFar=0;
	size_t ReturnValue = 0;
	while( tv_now<tv_timeout )
	{
		serio_read(&m_Serio, &Buf[ReadSoFar], MaxLen, &ReturnValue);
for(int i=0;i<ReturnValue;++i )
{
	unsigned char c=Buf[ReadSoFar+i];
fprintf(filer,"-%02x- ",(int) c );
}
if( ReturnValue==0 )
fprintf(filer,".");
		ReadSoFar+=ReturnValue;
		if( ReadSoFar>=MaxLen )
			return ReadSoFar;
		Sleep(10); // We really need something that blocks -- nasty HACK
		gettimeofday(&tv_now, NULL);
	}
	return ReadSoFar;
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
    break;
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
	struct timeval now;
	struct timeval last;
	unsigned int bytes=0;
	fd_set rfds;
	int ret;
	
	gettimeofday(&now,NULL);

	last.tv_sec = now.tv_sec+Timeout/1000;
	last.tv_usec = now.tv_usec+(Timeout % 1000) * 1000;

	while(1)
	{
		FD_ZERO(&rfds);
		FD_SET(m_fdSerial, &rfds);
		tv.tv_sec = last.tv_sec - now.tv_sec;
		tv.tv_usec = last.tv_usec - now.tv_usec;
		if(tv.tv_usec < 0)
		{
			tv.tv_sec--;
			tv.tv_usec+=1000*1000;
		}
		if(tv.tv_sec<0)
		{
			break;
		}
		ret = select(m_fdSerial+1, &rfds, NULL, NULL, &tv);
		if (ret <= 0) {
			break;
		}
		size_t retval = read(m_fdSerial,Buf+bytes,MaxLen-bytes);
		bytes += retval;
		if(bytes==MaxLen)
		{
			break;
		}
		gettimeofday(&now,NULL);
	}
	return bytes>0?bytes:-1;
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
