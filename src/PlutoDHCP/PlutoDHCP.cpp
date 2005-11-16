#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "Logger.h"
#include "PlutoDHCP.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <list>

#ifdef WIN32
#include <direct.h>
#include <conio.h>
#define chdir _chdir  // Why, Microsoft, why?
#define mkdir _mkdir  // Why, Microsoft, why?
#else

#endif

#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_Device.h"
#include "pluto_main/Table_Device_DeviceData.h"
#include "pluto_main/Table_DeviceData.h"
#include "pluto_main/Table_DeviceCategory.h"
#include "pluto_main/Table_DeviceTemplate.h"
#include "pluto_main/Table_Installation.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_CommandParameter.h"

#include "BD/PhoneDevice.h"

#define  VERSION "<=version=>"

using namespace std;
using namespace DCE;

PlutoDHCP::PlutoDHCP(int PK_Installation,string host, string user, string pass, string db_name, int port) 
{
    m_pDatabase_pluto_main = new Database_pluto_main( );
    if( !m_pDatabase_pluto_main->Connect( host, user, pass, db_name, port ) )
	{
		cerr << "Cannot connect to database" << endl;
		m_bConnected=false;
	}
	else
		m_bConnected=true;

	if( PK_Installation )
		m_iPK_Installation = PK_Installation;
	else
	{
		vector<Row_Installation *> vectRow_Installation;
		m_pDatabase_pluto_main->Installation_get()->GetRows("1=1",&vectRow_Installation);
		if( vectRow_Installation.size()==0 )
		{
			cerr << "ERROR: Cannot determine Installation #" << endl;
			g_pPlutoLogger->Write(LV_CRITICAL,"Cannot determine installation");
			return;
		}
		else if( vectRow_Installation.size()>1 )
		{
			cerr << "WARNING: Cannot determine Installation # definitively" << endl;
			g_pPlutoLogger->Write(LV_CRITICAL,"Cannot determine Installation # definitively");
		}
		m_iPK_Installation = vectRow_Installation[0]->PK_Installation_get();
	}
}

bool PlutoDHCP::IsConnected()
{
	return m_bConnected;
}

void PlutoDHCP::ReadAllIPs()
{
	vector<Row_Device *> m_vectRow_Device;
	m_pDatabase_pluto_main->Device_get()->GetRows("IPAddress IS NOT NULL and IPAddress<>'' AND FK_Installation=" + StringUtils::itos(m_iPK_Installation),&m_vectRow_Device);
	for(size_t s=0;s<m_vectRow_Device.size();++s)
	{
		Row_Device *pRow_Device = m_vectRow_Device[s];
		IPAddress ip(pRow_Device->IPaddress_get());
		if( ip.BadIP() )
			cerr << "Warning: Device " << pRow_Device->PK_Device_get() << " has a bad IP" << endl;
		else
			m_mapIP_Device[ip.AsInt()] = pRow_Device;
	}
}

Row_Device *PlutoDHCP::DetermineCore()
{
	vector<Row_Device *> vectRow_Device;
	m_pDatabase_pluto_main->Device_get()->GetRows("JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_DeviceCategory=" + StringUtils::itos(DEVICECATEGORY_Core_CONST) + " AND FK_Installation=" + StringUtils::itos(m_iPK_Installation),&vectRow_Device);
	if( vectRow_Device.size()==0 )
	{
		cerr << "ERROR: Cannot determine Core #" << endl;
		g_pPlutoLogger->Write(LV_CRITICAL,"Cannot determine core");
		return NULL;
	}
	else if( vectRow_Device.size()>1 )
	{
		cerr << "WARNING: Cannot determine core # definitively" << endl;
		g_pPlutoLogger->Write(LV_CRITICAL,"Cannot determine core # definitively");
	}
	return vectRow_Device[0];
}

void PlutoDHCP::DetermineIPRange(IPAddress &ipAddressDhcpStart,IPAddress &ipAddressDhcpStop,IPAddress &ipAddressPlutoStart,IPAddress &ipAddressPlutoStop)
{
	Row_Device *pRow_Device = DetermineCore();
	if( !pRow_Device )
		return;
	Row_Device_DeviceData *pRow_Device_DeviceData = m_pDatabase_pluto_main->Device_DeviceData_get()->GetRow(pRow_Device->PK_Device_get(),DEVICEDATA_DHCP_CONST);
	if (!pRow_Device_DeviceData)
	{
		cerr << "ERROR: Cannot find dhcp data" << endl;
		g_pPlutoLogger->Write(LV_CRITICAL,"Cannot find dhcp data");
		return;
	}

	// This format: 192.168.80.3-192.168.80.128,192.168.80.129-192.168.80.254
	string sData = pRow_Device_DeviceData->IK_DeviceData_get();
	string::size_type posComma = sData.find(",");
	string::size_type posDash = sData.find("-");
	string::size_type pos2ndDash = posComma==string::npos ? string::npos : sData.find("-",posComma);

	if( posComma==string::npos || posDash==string::npos || pos2ndDash==string::npos )
	{
		cerr << "ERROR: dhcp data incomplete" << endl;
		g_pPlutoLogger->Write(LV_CRITICAL,"dhcp data incomplete");
		return;
	}

	ipAddressPlutoStart.ParseIP( sData.substr(0,posDash) );
	ipAddressPlutoStop.ParseIP( sData.substr(posDash,posComma-posDash) );
	ipAddressDhcpStart.ParseIP( sData.substr(posComma+1,pos2ndDash-posComma) );
	ipAddressDhcpStop.ParseIP( sData.substr(pos2ndDash+1) );

	if( ipAddressPlutoStart.BadIP() || ipAddressPlutoStop.BadIP() || 
		ipAddressDhcpStart.BadIP() || ipAddressDhcpStop.BadIP() )
	{
		cerr << "ERROR: dhcp data malformed" << endl;
		g_pPlutoLogger->Write(LV_CRITICAL,"dhcp data malformed");
		return;
	}
}

string PlutoDHCP::AssignIP(int PK_Device)
{
	// The device we are going to assign an IP address to
	Row_Device *pRow_Device = m_pDatabase_pluto_main->Device_get()->GetRow(PK_Device);
	if( !pRow_Device )
	{
		cout << "ERROR: Device is invalid" << endl;
		return "";
	}

	// See if it already has one
	string sIP = pRow_Device->IPaddress_get();
	if( sIP.size() )
	{
		// It's already got a good one, use it
		IPAddress ip(sIP);
		if( !ip.BadIP() )
			return ip.AsText();
	}

	// Nope, we're going to have to find an unused IP address
	IPAddress ipAddressDhcpStart,ipAddressDhcpStop,ipAddressPlutoStart,ipAddressPlutoStop;
	DetermineIPRange(ipAddressDhcpStart,ipAddressDhcpStop,ipAddressPlutoStart,ipAddressPlutoStop);
	cout << ipAddressPlutoStart.AsText() << "-" << ipAddressPlutoStop.AsText() << endl;
	for(unsigned long ip = ipAddressPlutoStart.AsInt(); ip<=ipAddressPlutoStop.AsInt(); ip++)
	{
		if( m_mapIP_Device.find(ip)==m_mapIP_Device.end() )
		{
			// We found one that's not used
			IPAddress _ip(ip);
			pRow_Device->IPaddress_set( _ip.AsText() );
			m_pDatabase_pluto_main->Device_get()->Commit();
			return _ip.AsText();
		}
	}

	cout << "No more IP Addresses!" << endl;
	return "";
}

string PlutoDHCP::GetDHCPConfig()
{
	string sResult; // The dhcp.conf stuff
	sResult += "#!/bin/bash\nMOON_ENTRIES=\nNOBOOT_ENTRIES=\n";
	int iMoonNumber = 0, iNoBootNumber = 0;
	for(map<unsigned long,class Row_Device *>::iterator it=m_mapIP_Device.begin();it!=m_mapIP_Device.end();++it)
	{
		Row_Device *pRow_Device = it->second;

		// Confirm the mac address is valid.  Use Bluetooth's phoneDevice class.  Read it in as a string
		// (which converts it to an int and stores as uppercase), and then read in the int it converted to.
		// If they are the same, we got a valid mac address.  If they are different, then something is wrong with it
		PhoneDevice pd_1("",pRow_Device->MACaddress_get(),0);
		PhoneDevice pd_2("",pd_1.m_iMacAddress,0);
		if( pd_1.m_sMacAddress!=pd_2.m_sMacAddress )
		{
			// Use cerr since cout is going into the dhcp file
			cerr << "Device " << pRow_Device->PK_Device_get() << " has bad mac address " << pRow_Device->MACaddress_get() << endl;
			continue;
		}

		IPAddress ip(pRow_Device->IPaddress_get());
		if( bIsMediaDirector(pRow_Device) )
		{
			sResult += string("MOON_ENTRIES=\"$MOON_ENTRIES\\n\\thost moon" + StringUtils::itos(++iMoonNumber)
					+ " { hardware ethernet " + pd_1.m_sMacAddress + "; fixed-address " + ip.AsText() + "; }\"\n");
		}
		else
		{
			sResult += string("NOBOOT_ENTRIES=\"$NOBOOT_ENTRIES\\n\\thost pc" + StringUtils::itos(++iNoBootNumber)
					+ " { hardware ethernet " + pd_1.m_sMacAddress + "; fixed-address " + ip.AsText() + "; }\"\n");
		}
	}
	return sResult;
}

bool PlutoDHCP::bIsMediaDirector(Row_Device *pRow_Device)
{
	Row_DeviceTemplate *pRow_DeviceTemplate=NULL;
	if( (pRow_DeviceTemplate=pRow_Device->FK_DeviceTemplate_getrow())==NULL ||
		pRow_DeviceTemplate->FK_DeviceCategory_get()!=DEVICECATEGORY_Media_Director_CONST )
			return false; // We're not an MD

	Row_Device_DeviceData *pRow_Device_DeviceData = m_pDatabase_pluto_main->Device_DeviceData_get()->GetRow(
		pRow_Device->PK_Device_get(),
		DEVICEDATA_Diskless_Boot_CONST);

	// See if we're a diskless MD, return false if the parameter isn't specified or isn't 1
	return pRow_Device_DeviceData && pRow_Device_DeviceData->IK_DeviceData_get()=="1";
}
