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
	vector<Row_Device *> m_vectRow_Device;
	m_pDatabase_pluto_main->Device_get()->GetRows("JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_DeviceCategory=" + StringUtils::itos(DEVICECATEGORY_Core_CONST) + " AND FK_Installation=" + StringUtils::itos(m_iPK_Installation),&m_vectRow_Device);
	if( m_vectRow_Device.size()==0 )
	{
		cerr << "ERROR: Cannot determine Core #" << endl;
		g_pPlutoLogger->Write(LV_CRITICAL,"Cannot determine core");
		return NULL;
	}
	else if( vectRow_Installation.size()>1 )
	{
		cerr << "WARNING: Cannot determine core # definitively" << endl;
		g_pPlutoLogger->Write(LV_CRITICAL,"Cannot determine core # definitively");
	}
	return m_vectRow_Device[0];
}

void PlutoDHCP::DetermineIPRange(IPAddress &ipAddressDhcpStart,IPAddress &ipAddressDhcpStop,IPAddress &ipAddressPlutoStart,IPAddress &ipAddressPlutoStop)
{
	Row_Device *pRow_Device = DetermineCore();
	if( !pRow_Device )
		return;
	Row_Device_DeviceData *pRow_Device_DeviceData = m_pDatabase_pluto_main->Device_DeviceData_get()->GetRow(pRow_Device->PK_Device_get(),DEVICEDATA_DHCP_CONST);
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
