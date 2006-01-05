#include ".\ZWJobSwitchBinaryGet.h"
#include "ZW_SerialAPI.h"
#include "ZW_transport_api.h"
#include "ZW_classcmd.h"
#include "PlutoZWSerialAPI.h"
#include "main.h"

#define DATA_LEN 2

class ZWJobSwitchBinaryGet::Private
{
public:
	Private(unsigned short nodeID);
	unsigned short nodeID;
	int txStatusCount;
};

ZWJobSwitchBinaryGet::Private::Private(unsigned short nID)
	: nodeID(nID),
	txStatusCount(0)
{
}

ZWJobSwitchBinaryGet::ZWJobSwitchBinaryGet(PlutoZWSerialAPI * zwAPI, unsigned short nodeID)
	: ZWaveJob(zwAPI)
{
}

ZWJobSwitchBinaryGet::~ZWJobSwitchBinaryGet()
{
}

bool ZWJobSwitchBinaryGet::run()
{
	setState(ZWaveJob::RUNNING);
	size_t len = 8;
	char data[8];
	data[0] = REQUEST;
	data[1] = FUNC_ID_ZW_SEND_DATA;
	data[2] = (char)d->nodeID;
	data[3] = DATA_LEN;
	data[4] = COMMAND_CLASS_SWITCH_MULTILEVEL;
	data[5] = SWITCH_BINARY_GET;
	data[6] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
	data[7] = 2;
	return handler()->sendData(data, len);
}

bool ZWJobSwitchBinaryGet::processData(const char * buffer, size_t length)
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
			else if(d->txStatusCount == 0)//buffer[1] == REQUEST
			{
				DCE::g_pPlutoLogger->Write(LV_DEBUG, "ZWJobSwitchChangeLevel::processData the tx status is here");
				d->txStatusCount ++;
				return true;				
			}
			else//buffer[1] == REQUEST
			{
				DCE::g_pPlutoLogger->Write(LV_DEBUG, "ZWJobSwitchChangeLevel::processData the tx status is here");
				setState(ZWaveJob::STOPPED);
				return true;				
			}

	}
	return false;
}
