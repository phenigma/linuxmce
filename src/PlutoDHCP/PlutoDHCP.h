#ifndef PlutoDHCP_h
#define PlutoDHCP_h

#include "PlutoUtils/MySQLHelper.h"

class IPAddress
{
private:
	unsigned long m_IPAddress;
	bool m_bBadIP;
public:
	IPAddress(string sIPAddress)
	{
		ParseIP(sIPAddress);
	}
	bool ParseIP(string sIPAddress)
	{
		m_bBadIP=false;
		string::size_type pos=0;
		unsigned char *pByte = ((unsigned char *) &m_IPAddress) + 3;
		for(int i=0;i<4;++i)
		{
			string sNumber = StringUtils::Tokenize(sIPAddress,".",pos);
			if( !sNumber.size() )
				m_bBadIP=true;
			*pByte = (unsigned char) atoi(sNumber.c_str());
			pByte--;
		}
		return !m_bBadIP;
	}

	IPAddress(unsigned long IPAddress) { m_IPAddress=IPAddress; }
	unsigned long AsInt() { return m_IPAddress; }
	string AsText() 
	{
		string sResult;
		unsigned char *pByte = ((unsigned char *) &m_IPAddress) + 3;
		for(int i=0;i<4;++i)
		{
			if( i )
				sResult += ".";
			sResult += StringUtils::itos(*pByte);
			pByte--;
		}
		return sResult;
	}
	bool BadIP() { return m_bBadIP; }
};

class Database_pluto_main;

class PlutoDHCP 
{
	bool m_bConnected;
	int m_iPK_Installation;
	map<unsigned long,class Row_Device *> m_mapIP_Device;
	Database_pluto_main *m_pDatabase_pluto_main;

public:
	PlutoDHCP(int PK_Installation,string host, string user, string pass, string db_name, int port=3306);

	bool bIsMediaDirector(Row_Device *pRow_Device) { return false; }
	void ReadAllIPs();
	bool IsConnected();
	Row_Device *DetermineCore();
	void DetermineIPRange(IPAddress &ipAddressDhcpStart,IPAddress &ipAddressDhcpStop,IPAddress &ipAddressPlutoStart,IPAddress &ipAddressPlutoStop);
};
#endif
