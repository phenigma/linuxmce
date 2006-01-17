#include "ZWJobSwitchBinaryGet.h"
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
	d = new Private(nodeID);
	setType(ZWaveJob::GET_SWITCH_BINARY);
}

ZWJobSwitchBinaryGet::~ZWJobSwitchBinaryGet()
{
	delete d;
	d = NULL;
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
	char log_buf[1024 * 5];
	memset(log_buf, 0, sizeof(log_buf));
	for(unsigned int ww = 0; ww < length && ww < sizeof(log_buf); ww++)
		sprintf(&(log_buf[ww*5]), "0x%02x ", buffer[ww]);
	g_pPlutoLogger->Write(LV_DEBUG, "%s", log_buf);
	switch(state())
	{
		default:
		case ZWaveJob::IDLE:
		case ZWaveJob::STOPPED:
			break;
		case ZWaveJob::RUNNING:
			if(length < 2)
			{
				DCE::g_pPlutoLogger->Write(LV_CRITICAL, "ZWJobSwitchBinaryGet::processData, length too small");
				break;
			}

			if(buffer[0] == RESPONSE)
			{
				if(buffer[1] != FUNC_ID_ZW_SEND_DATA)
				{
					DCE::g_pPlutoLogger->Write(LV_CRITICAL, "ZWJobSwitchBinaryGet::processData, buffer incorrect");
					break;				
				}
				DCE::g_pPlutoLogger->Write(LV_DEBUG, "ZWJobSwitchBinaryGet::processData the response is here");
				return true;				
			}
			else if(d->txStatusCount == 0)//buffer[0] == REQUEST
			{
entry:
				if(buffer[1] != FUNC_ID_ZW_SEND_DATA)
				{
					DCE::g_pPlutoLogger->Write(LV_CRITICAL, "ZWJobSwitchBinaryGet::processData, buffer incorrect");
					break;				
				}
				if(length < 3)
				{
					DCE::g_pPlutoLogger->Write(LV_CRITICAL, "ZWJobSwitchBinaryGet::processData, length too small %d", length);
					break;
				}
				DCE::g_pPlutoLogger->Write(LV_DEBUG, "ZWJobSwitchBinaryGet::processData the tx status is here");
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
				d->txStatusCount ++;
				return true;				
			}
			else//buffer[0] == REQUEST
			{
				if(length < 8)
				{
					DCE::g_pPlutoLogger->Write(LV_CRITICAL, "ZWJobSwitchBinaryGet::processData, length too small %d", length);
					goto entry;
				}

				if(FUNC_ID_APPLICATION_COMMAND_HANDLER == buffer[1] &&
					COMMAND_CLASS_SWITCH_MULTILEVEL == buffer[5] && 
					(char)d->nodeID == buffer[3] &&
					BASIC_REPORT == buffer[6])
				{
					DCE::g_pPlutoLogger->Write(LV_DEBUG, "ZWJobSwitchBinaryGet::processData basic multilevel swich report is here");
					DCE::g_pPlutoLogger->Write(LV_DEBUG, "ZWJobSwitchBinaryGet::processData basic report = 0x%x", buffer[7]);
				}
				else
				{
					DCE::g_pPlutoLogger->Write(LV_DEBUG, "ZWJobSwitchBinaryGet::processData basic multilevel swich report is NOT here");
					//break;
					//sorry (mailto: edgar.g@plutohome.com)
					goto entry;
				}

				setState(ZWaveJob::STOPPED);
				return true;				
			}
	}
	return false;
}
