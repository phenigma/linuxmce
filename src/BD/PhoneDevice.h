#ifndef PhoneDevice_h
#define PhoneDevice_h

#ifndef WIN32
#ifndef NOBLUETOOTH
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#endif
#endif

#include <math.h>

#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"


class PhoneDevice
{

public:
	string m_sID,m_sMacAddress,m_UserVersion;
	unsigned long m_PKID_Users;
	u_int64_t m_iMacAddress;
	bool m_bIsConnected;
	time_t m_tFirstDetected,m_tLastDetected;
	int m_iLinkQuality;

#ifndef WIN32	
#ifndef NOBLUETOOTH
	bdaddr_t m_bdaddrDongle;
#endif
#endif
	
	// These are just for using the Socket communication & the tester
	string m_sIPAddress;
	int m_iPort;

	PhoneDevice(string ID,string MacAddress,int LinkQuality)
	{
		m_PKID_Users=0;
		m_iLinkQuality=LinkQuality;
		m_sID=ID;
		m_sMacAddress=MacAddress;
		m_bIsConnected=false;
		string::size_type pos=0;
		m_iMacAddress=0;
		for(int i=5;i>=0;--i)
		{
			string sHex = StringUtils::Tokenize(m_sMacAddress,":",pos);
			int hex;
			sscanf(sHex.c_str(),"%x",&hex);
			u_int64_t power = (u_int64_t) pow( (long double) 256, i);
			m_iMacAddress += (power * hex);
		}
		m_tFirstDetected=m_tLastDetected=time(NULL);
		m_sMacAddress=StringUtils::ToUpper(m_sMacAddress);
		CheckForPlutoID();
	}
	PhoneDevice(string ID,u_int64_t MacAddress,int LinkQuality)
	{
		m_PKID_Users=0;
		m_iLinkQuality=LinkQuality;
		m_sID=ID;
		m_iMacAddress=MacAddress;
		m_bIsConnected=false;
		m_tFirstDetected=m_tLastDetected=time(NULL);
		for(int i=5;i>=0;--i)
		{
			u_int64_t power = (u_int64_t) pow((long double) 256, i);
			int hex = (int) (MacAddress / power);
			MacAddress -= (hex * power);
			char sHex[5];
			sprintf(sHex,"%02x",hex);
			m_sMacAddress += sHex;
			if( i!=0 )
				m_sMacAddress += ":";
			
		}
		m_sMacAddress=StringUtils::ToUpper(m_sMacAddress);
		CheckForPlutoID();
	}
	void CheckForPlutoID()
	{
		m_sID = StringUtils::ToUpper(m_sID);
		if( m_sID.substr(0,2)=="PV" )
		{
			m_PKID_Users=0;
			for(int i=0;i<=5;++i)
			{
				char Digit = m_sID[i+2];
				unsigned long Value = 0;
				if( Digit<='9' )
					Value = Digit-48;
				else if( Digit<'O' )
					Value = Digit-55; // (A=65)
				else if( Digit>'O' )
					Value = Digit-56; // Skip 'O', which we treat as 0

				unsigned long p = (unsigned long) pow((double) 35,(double)5-i);
				m_PKID_Users += p * Value;
			}
			m_UserVersion = m_sID.substr(8,3);
		}
	}
};


#endif
