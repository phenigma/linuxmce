#include <stdio.h>
#include "ZWJobSwitchChangeLevel.h"
#include "ZW_SerialAPI.h"
#include "ZW_transport_api.h"
#include "ZW_classcmd.h"
#include "PlutoZWSerialAPI.h"
#include "main.h"

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
}

ZWJobSwitchChangeLevel::ZWJobSwitchChangeLevel(PlutoZWSerialAPI * zwAPI, unsigned char level, unsigned short nodeID)
	: ZWaveJob(zwAPI)
{
	d = new Private(level, nodeID);
	setType(ZWaveJob::SET_SWITCH_LEVEL);
}

ZWJobSwitchChangeLevel::~ZWJobSwitchChangeLevel(void)
{
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
	return handler()->sendData(data, len);
}

bool ZWJobSwitchChangeLevel::processData(const char * buffer, size_t length)
{
	switch(state())
	{
		default:
		case ZWaveJob::IDLE:
		case ZWaveJob::STOPPED:
			break;
		case ZWaveJob::RUNNING:
			if(length < 2)
			{
				DCE::g_pPlutoLogger->Write(LV_CRITICAL, "ZWJobSwitchChangeLevel::processData, length too small");
				break;
			}
			if(buffer[1] != FUNC_ID_ZW_SEND_DATA)
			{
				DCE::g_pPlutoLogger->Write(LV_CRITICAL, "ZWJobSwitchChangeLevel::processData, buffer incorrect");
				break;				
			}
			if(buffer[0] == RESPONSE)
			{
				DCE::g_pPlutoLogger->Write(LV_DEBUG, "ZWJobSwitchChangeLevel::processData the response is here");
				return true;				
			}
			else //buffer[1] == REQUEST
			{
				DCE::g_pPlutoLogger->Write(LV_DEBUG, "ZWJobSwitchChangeLevel::processData the tx status is here");
				switch(buffer[3])
				{
					case TRANSMIT_COMPLETE_OK:
						DCE::g_pPlutoLogger->Write(LV_DEBUG, "command completed OK");
						break;
					case TRANSMIT_COMPLETE_NO_ACK:
						DCE::g_pPlutoLogger->Write(LV_DEBUG, "command not ack");
						break;
					case TRANSMIT_COMPLETE_FAIL:
						DCE::g_pPlutoLogger->Write(LV_DEBUG, "failed to transmit command");
						break;
					default:
						DCE::g_pPlutoLogger->Write(LV_DEBUG, "unrecognized response coming as tx status");
				}
				setState(ZWaveJob::STOPPED);
				return true;				
			
			}
	}
	setState(ZWaveJob::STOPPED);
	return false;
}
