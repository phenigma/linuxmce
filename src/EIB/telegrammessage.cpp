//
// C++ Implementation: telegrammessage
//
// Description: 
//
//
// Author:  <>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "telegrammessage.h"
#include "busconnector.h"

/* helper functions */
static unsigned getunsignedfromint( int value );
static int getintfromeis5( unsigned eis5 );
static unsigned short getgroupaddrfromstr(const char *gadstr);
static bool getstrfromgroupaddr(unsigned short gaddr, char *gadstr, int length);
static unsigned short swapushort(unsigned short toswap);

#define MAX_GROUPADDR_STR_LEN	11
#define MAX_STRING_DATA_LEN		23

/* TelegramMessage */

namespace EIBBUS {

TelegramMessage::TelegramMessage()
{
}

TelegramMessage::~TelegramMessage()
{
}

int 
TelegramMessage::Recv(BusConnector *pbusconn) {
	int ret = 0;
	unsigned char inmsg[MAX_USERDATA_LENGTH];
	unsigned int userlength = MAX_USERDATA_LENGTH;
	if((ret = ReceiveServerBuffer(pbusconn, inmsg, userlength)) != 0) {
		return ret;
	}
	unsigned int length = userlength - 8;

	/*get destination grou[ address*/
	unsigned short groupaddr = *((unsigned short*)&inmsg[3]);
	char gaddrstr[MAX_GROUPADDR_STR_LEN];
	if(!getstrfromgroupaddr(groupaddr, gaddrstr, sizeof(gaddrstr))) {
		return -1;
	}
	
	setGroupAddress(gaddrstr);
	
	//int      		dataI;
	unsigned 		dataU;
	unsigned char 	dataC;
	
	switch(length) {
	case 0:
		setEisType(DT_SWITCH);
		dataC = inmsg[7] & 0x3F;
		break;
	case 1:
		setEisType(DT_ASCIICHAR);
		dataC = *((unsigned char*)&inmsg[8]);
		break;
	case 2:
		setEisType(DT_VALUE);
		dataU = *((unsigned short*)&inmsg[8]);
		break;
	case 3:
		setEisType(DT_TIME);
		dataU = *((unsigned int*)&inmsg[8]) >> 8;
		break;
	case 4:
		setEisType(DT_FLOAT);
		dataU = *((unsigned int*)&inmsg[8]);
		break;
	default:
		setEisType(DT_STRING);
		unsigned char buff[MAX_STRING_DATA_LEN];
		memcpy(buff, &inmsg[8], length);
		break;
	}
	
	return 0;
}

int 
TelegramMessage::Send(BusConnector *pbusconn) {
	unsigned char length;
	
	switch(getEisType()) {
	case 0:
		length = 0; break;
		
	case 1:
	case 2:
	case 7:
	case 8:
		length = 0; break;
		
	case 6:
	case 13:
	case 14:
		length = 1; break;
		
	case 3:
	case 4:
		length = 3; break;
	
	case 5:
	case 10:
		length = 2; break;
	
	case 9:
	case 11:
	case 12:
		length = 4; break;
	
	case 15:
		length = 14; break;
	
	default:
		length = 0; 
		break;
	}
	
	unsigned char routingcounterlength = (unsigned short)(length + 1) + 0xE0;
	
	/*final data buffer*/
	unsigned char data[MAX_USERDATA_LENGTH];
	unsigned char acpishortuserdata = 0;
	
	if(length != 0) {
		/*temporary conversion data*/
		int      		dataI;
		unsigned 		dataU;
		unsigned char 	dataC;
        
		acpishortuserdata = (unsigned char)(getActionType() << 6);
		
		switch( getEisType() )
        {
            // length = 1
            case 6:  // scaling
            case 13: // ASCI Character
            case 14: // 8-Bit counter value
				dataU = (unsigned)atoi( usrdata_.c_str() );
                if( dataU > 255 )
                    dataC = 255;
                else
                    dataC = (unsigned char)dataU;
				*data = dataC;
                break;
            // length = 2 , 3 or 4
            case 3:  // time - 3 Bytes
            case 4:  // date - 3 Bytes
            case 5:  // value - 2 Bytes
            case 9:  // float - 4 Bytes
            case 10: // counter value - 2 Bytes
            case 11: // 32-bit counter value - 4 Bytes
            case 12: // access - 4 bytes
				dataI = atoi( usrdata_.c_str() );
				dataU = getunsignedfromint( dataI );
				if( getEisType() == 5 )
				{
					dataU = getintfromeis5( dataU );
				}
				
				memcpy(data, &dataU, length);
                break;
            
			case 15:  // Character String, length = 14
				memcpy(data, usrdata_.c_str(), length);
                break;

            default:
                break;
        }
	} else         // short data
    {
		acpishortuserdata = ((unsigned char)(getActionType()) << 6) + (unsigned char)atoi( usrdata_.c_str() );
    }
	
	/*control field*/
	int controlfield = 0x9C;
	
	/*group address*/	
	unsigned short groupaddr = getgroupaddrfromstr(gaddr_.c_str());
	

	char outmsg[MAX_USERDATA_LENGTH];
	unsigned outmsglength = 0;
	
	outmsg[0] = controlfield;
	outmsg[1] = outmsg[2] = 0;
	*((unsigned short*)&outmsg[3]) = groupaddr;
	outmsg[5] = routingcounterlength;
	outmsg[6] = 0;
	outmsg[7] = acpishortuserdata;
	
	unsigned char userlength = routingcounterlength & 0x0F; 
	if(userlength > 1) {
		memcpy(&outmsg[8], data, userlength - 1);
	}
	outmsglength = 8 + userlength - 1;
	
	return SendServerBuffer(pbusconn, (unsigned char*)outmsg, outmsglength);
}

};

/* helper functions */
unsigned getunsignedfromint( int value )
{
    if( value >= 0 )
        return value;

    unsigned ret = *((unsigned *)(&value));

    return ret;
}

int getintfromeis5( unsigned eis5 )
{
    eis5 = eis5 & 0xffff; // only 2 bytes
    int sign = (eis5 & 0x8000) ? 1 : 0;  // last bit
    int exponent = (eis5 & 0x7800) >> 11;

    int pow = 1;
    for( int i = 0; i < exponent; i++ )
        pow *= 2;

    int mantissa = eis5 & 0x07ff;
    if( sign ) // two's complement
    {
        mantissa ^= 0x7ff;
        mantissa += 1;
        mantissa &= 0x07ff;
    }

    int result = pow * mantissa;

    if( sign )
        return (-result);
    else
        return result;
}

unsigned short 
getgroupaddrfromstr(const char *gadstr)
{
    char *pFirstSlash, *pSecondSlash;
    char gad[128];
    unsigned short mainGad, middleGad, subGad, middleSubGad;

    if( !gadstr )
        return 0xffff;

    strncpy( gad, gadstr, 127 );
    gad[127] = 0;

    if( !(pFirstSlash = strchr( gad, '/' )) )
        return 0xffff;
    *pFirstSlash = 0;

    if( !(pSecondSlash = strchr( pFirstSlash + 1, '/' )) )
        return 0xffff;
    *pSecondSlash = 0;

    mainGad = (unsigned short)atoi( gad );
    middleGad = (unsigned short)atoi(pFirstSlash + 1);
    subGad = (unsigned short)atoi( pSecondSlash + 1 );

    if( (mainGad > 15) || (subGad > 0x7ff ) )
        return 0xfff;

    middleSubGad = subGad + (middleGad << 8);
    return swapushort( (middleSubGad & 0x7ff) | ( mainGad << 11 ) );
};

bool 
getstrfromgroupaddr(unsigned short gaddr, char *gadstr, int length) {
	if(length < MAX_GROUPADDR_STR_LEN) {
		return false;
	}
	
	unsigned short mainGad, subGad, middleGad;
	if(!gadstr) {
        return false;
	}

    unsigned short gadSwap = swapushort( gaddr );
	mainGad = gadSwap >> 11;
    subGad = gadSwap & 0x07ff;
    middleGad = subGad >> 8;
    subGad = subGad & 0xff;

    sprintf( gadstr, "%d/%d/%d", mainGad, middleGad, subGad );
	return true;
}

unsigned short 
swapushort(unsigned short toswap)
{
    unsigned char res[2], *ptoswap = (unsigned char *)(&toswap);

    res[0] = ptoswap[1];
    res[1] = ptoswap[0];

    return  (*((unsigned short *)res));
}
