//
// C++ Implementation: seriald
//
// Description:
//
//
// Author: Eugen Constantinescu <eugen.c@plutohome.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "seriald.h"
#include "HAL.h"

#include "DCE/Logger.h"
#include "Serial/SerialPort.h"
#include "PlutoUtils/StringUtils.h"
#include "Gen_Devices/AllCommandsRequests.h"

DCE::HAL * SerialD::halDevice = NULL;
bool SerialD::running = false;
std::vector<std::string> SerialD::serialDevices;

SerialD::SerialD()
{
}

SerialD::~SerialD()
{
}

void* SerialD::startUp(void * device)
{
	g_pPlutoLogger->Write(LV_DEBUG, "############ SerialD Start ");
	
	if( device == NULL )
	{
		// error
		return NULL;
	}
	
	halDevice = (HAL *)device;
	if( halDevice->m_pData == NULL )
	{
		// error
		return NULL;
	}
	
	string sPort;
	unsigned baud = 9600;
	
	string val2assign, response;
	
	CMD_Get_Unused_Serial_Ports_DT cmd(halDevice->m_dwPK_Device, DEVICETEMPLATE_General_Info_Plugin_CONST, BL_SameHouse, halDevice->m_dwPK_Device, &val2assign);
	halDevice->SendCommand(cmd, &response);
	
	g_pPlutoLogger->Write(LV_DEBUG, "Serial ports %s -- %s\n", response.c_str(), val2assign.c_str());
	
	if( !val2assign.empty() )
	{
		StringUtils::Tokenize(val2assign, ",", serialDevices);
	}
	
	running = true;
	
	// thread loop
	while(running)
	{
		// test each available serial port
		std::vector<std::string>::iterator it=serialDevices.begin();
		while( it!=serialDevices.end() )
		{
			sPort = (*it);
			try
			{
				CSerialPort serialPort(sPort, baud, epbsN81, true);
				
				if( serialPort.IsBusy() )
				{
					g_pPlutoLogger->Write(LV_DEBUG, "++++++++++++ SerialD Busy %s", sPort.c_str());
					// fire event 
					halDevice->EVENT_Device_Detected("", "", "", 0, "", 2, 0, "", "37|" + sPort, "serial");
					// remove it from tests
					it = serialDevices.erase(it);
					continue;
				}
				else
				{
//					g_pPlutoLogger->Write(LV_DEBUG, "------------ SerialD Not Busy %s", sPort.c_str());
				}
			}
			catch(string sError)
			{
//				g_pPlutoLogger->Write(LV_DEBUG, "Exception: %s", sError.c_str());
			}
			
			++it;
		}
		
		usleep(1000000);
	}
	
	g_pPlutoLogger->Write(LV_DEBUG, "############ SerialD END ");
	
	return NULL;
}

void SerialD::shutDown()
{
	g_pPlutoLogger->Write(LV_DEBUG, "############ SerialD shutDown ");
	running = false;
}
