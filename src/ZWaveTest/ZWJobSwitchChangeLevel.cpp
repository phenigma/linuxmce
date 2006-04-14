#include <stdio.h>
#include "ZWJobSwitchChangeLevel.h"
#include "ZW_SerialAPI.h"
#include "ZW_transport_api.h"
#include "ZW_classcmd.h"
#include "PlutoZWSerialAPI.h"
#include "main.h"
#include "SerialConnection.h"

#define DATA_LEN 3

class ZWJobSwitchChangeLevel::Private
{
public:
	/**it is possible to change the level for a list of nodes*/
	Private(unsigned char level, unsigned short nodeID);
	unsigned char level;
	unsigned short nodeID;
};


ZWJobSwitchChangeLevel::Private::Private(unsigned char l, unsigned short nID)
	:level(l),
	 nodeID(nID)
{
#ifdef PLUTO_DEBUG
g_pPlutoLogger->Write(LV_DEBUG, "~~~~~~~~~~~~~~1" );
#endif
}

ZWJobSwitchChangeLevel::ZWJobSwitchChangeLevel(PlutoZWSerialAPI * zwAPI, unsigned char level, unsigned short nodeID)
	: ZWaveJob(zwAPI)
{
	d = new Private(level, nodeID);
	setType(ZWaveJob::SET_SWITCH_LEVEL);
#ifdef PLUTO_DEBUG
g_pPlutoLogger->Write(LV_DEBUG, "~~~~~~~~~~~~~~2" );
#endif
}

ZWJobSwitchChangeLevel::~ZWJobSwitchChangeLevel(void)
{
#ifdef PLUTO_DEBUG
g_pPlutoLogger->Write(LV_DEBUG, "~~~~~~~~~~~~~~3" );
#endif
	delete d;
	d = NULL;
}

bool ZWJobSwitchChangeLevel::run()
{
	setState(ZWaveJob::RUNNING);
	size_t len = 9;
	char data[9];
	data[0] = REQUEST;
	data[1] = FUNC_ID_ZW_SEND_DATA;
	data[2] = (char)d->nodeID;
	data[3] = DATA_LEN;
	data[4] = COMMAND_CLASS_SWITCH_MULTILEVEL;
	data[5] = SWITCH_MULTILEVEL_SET;
	data[6] = d->level;
	data[7] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
	data[8] = 1;
#ifdef PLUTO_DEBUG
g_pPlutoLogger->Write(LV_DEBUG, "~~~~~~~~~~~~~~4" );
#endif
	return handler()->sendData(data, len);
}

bool ZWJobSwitchChangeLevel::processData(const char * buffer, size_t length)
{
	SerialConnection::printDataBuffer(buffer, length, "ZWJobSwitchChangeLevel");
	switch(state())
	{
		default:
		case ZWaveJob::IDLE:
		case ZWaveJob::STOPPED:
#ifdef PLUTO_DEBUG
g_pPlutoLogger->Write(LV_DEBUG, "~~~~~~~~~~~~~~5" );
#endif
			break;
		case ZWaveJob::RUNNING:
			if(length < 2)
			{
#ifdef PLUTO_DEBUG
g_pPlutoLogger->Write(LV_DEBUG, "~~~~~~~~~~~~~~6" );
#endif
				DCE::g_pPlutoLogger->Write(LV_CRITICAL, "ZWJobSwitchChangeLevel::processData, length too small");
				break;
			}
			if(buffer[1] != FUNC_ID_ZW_SEND_DATA)
			{
#ifdef PLUTO_DEBUG
g_pPlutoLogger->Write(LV_DEBUG, "~~~~~~~~~~~~~~7" );
#endif
				DCE::g_pPlutoLogger->Write(LV_CRITICAL, "ZWJobSwitchChangeLevel::processData, buffer incorrect");
				break;				
			}
			if(buffer[0] == RESPONSE)
			{
#ifdef PLUTO_DEBUG
g_pPlutoLogger->Write(LV_DEBUG, "~~~~~~~~~~~~~~8" );
				DCE::g_pPlutoLogger->Write(LV_DEBUG, "ZWJobSwitchChangeLevel::processData the response is here");
#endif
				return true;				
			}
			else //buffer[1] == REQUEST
			{
#ifdef PLUTO_DEBUG
g_pPlutoLogger->Write(LV_DEBUG, "~~~~~~~~~~~~~~9" );
				DCE::g_pPlutoLogger->Write(LV_DEBUG, "ZWJobSwitchChangeLevel::processData the tx status is here");
#endif
				switch(buffer[3])
				{
					case TRANSMIT_COMPLETE_OK:
						DCE::g_pPlutoLogger->Write(LV_DEBUG, "command completed OK");
#ifdef PLUTO_DEBUG
g_pPlutoLogger->Write(LV_DEBUG, "~~~~~~~~~~~~~~10" );
#endif
						break;
					case TRANSMIT_COMPLETE_NO_ACK:
						DCE::g_pPlutoLogger->Write(LV_DEBUG, "command not ack");
#ifdef PLUTO_DEBUG
g_pPlutoLogger->Write(LV_DEBUG, "~~~~~~~~~~~~~~11" );
#endif
						break;
					case TRANSMIT_COMPLETE_FAIL:
						DCE::g_pPlutoLogger->Write(LV_DEBUG, "failed to transmit command");
#ifdef PLUTO_DEBUG
g_pPlutoLogger->Write(LV_DEBUG, "~~~~~~~~~~~~~~12" );
#endif
						break;
					default:
						DCE::g_pPlutoLogger->Write(LV_DEBUG, "unrecognized response coming as tx status");
#ifdef PLUTO_DEBUG
g_pPlutoLogger->Write(LV_DEBUG, "~~~~~~~~~~~~~~13" );
#endif
				}
#ifdef PLUTO_DEBUG
g_pPlutoLogger->Write(LV_DEBUG, "~~~~~~~~~~~~~~14" );
#endif
				setState(ZWaveJob::STOPPED);
				return true;				
			
			}
	}
#ifdef PLUTO_DEBUG
g_pPlutoLogger->Write(LV_DEBUG, "~~~~~~~~~~~~~~15" );
#endif
	//setState(ZWaveJob::STOPPED);
	return false;
}
