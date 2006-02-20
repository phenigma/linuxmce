#include "ZWJobSwitchBinaryGet.h"
#include "ZW_SerialAPI.h"
#include "ZW_transport_api.h"
#include "ZW_classcmd.h"
#include "PlutoZWSerialAPI.h"
#include "main.h"
#include "SerialConnection.h"

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
#ifdef PLUTO_DEBUG
	g_pPlutoLogger->Write(LV_DEBUG, "*******************1" );
#endif
}

ZWJobSwitchBinaryGet::ZWJobSwitchBinaryGet(PlutoZWSerialAPI * zwAPI, unsigned short nodeID)
	: ZWaveJob(zwAPI)
{
	d = new Private(nodeID);
	setType(ZWaveJob::GET_SWITCH_BINARY);
#ifdef PLUTO_DEBUG
	g_pPlutoLogger->Write(LV_DEBUG, "*******************2" );
#endif
}

ZWJobSwitchBinaryGet::~ZWJobSwitchBinaryGet()
{
	delete d;
	d = NULL;
#ifdef PLUTO_DEBUG
	g_pPlutoLogger->Write(LV_DEBUG, "*******************3" );
#endif
}

bool ZWJobSwitchBinaryGet::run()
{
#ifdef PLUTO_DEBUG
	g_pPlutoLogger->Write(LV_DEBUG, "*******************4" );
#endif
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
	SerialConnection::printDataBuffer(buffer, length, "ZWJobSwitchBinaryGet");
#ifdef PLUTO_DEBUG
	g_pPlutoLogger->Write(LV_DEBUG, "*******************5" );
#endif
	switch(state())
	{
		default:
		case ZWaveJob::IDLE:
		case ZWaveJob::STOPPED:
#ifdef PLUTO_DEBUG
			g_pPlutoLogger->Write(LV_DEBUG, "*******************6" );
#endif
			return false;
		case ZWaveJob::RUNNING:
#ifdef PLUTO_DEBUG
			g_pPlutoLogger->Write(LV_DEBUG, "*******************7" );
#endif
			if(length < 2)
			{
				DCE::g_pPlutoLogger->Write(LV_CRITICAL, "ZWJobSwitchBinaryGet::processData, length too small");
#ifdef PLUTO_DEBUG
				g_pPlutoLogger->Write(LV_DEBUG, "*******************8" );
#endif
				break;
			}

			if(buffer[0] == RESPONSE)
			{
				if(buffer[1] != FUNC_ID_ZW_SEND_DATA)
				{
					DCE::g_pPlutoLogger->Write(LV_CRITICAL, "ZWJobSwitchBinaryGet::processData, buffer incorrect");
#ifdef PLUTO_DEBUG
					g_pPlutoLogger->Write(LV_DEBUG, "*******************9" );
#endif
					break;				
				}
#ifdef PLUTO_DEBUG
				DCE::g_pPlutoLogger->Write(LV_DEBUG, "ZWJobSwitchBinaryGet::processData the response is here");
g_pPlutoLogger->Write(LV_DEBUG, "*******************10" );
#endif
				return true;				
			}
			else if(d->txStatusCount == 0)//buffer[0] == REQUEST
			{
entry:
				if(buffer[1] != FUNC_ID_ZW_SEND_DATA)
				{
#ifdef PLUTO_DEBUG
					g_pPlutoLogger->Write(LV_DEBUG, "*******************11" );
#endif
					DCE::g_pPlutoLogger->Write(LV_CRITICAL, "ZWJobSwitchBinaryGet::processData, buffer incorrect");
					break;
				}
				if(length < 3)
				{
#ifdef PLUTO_DEBUG
					g_pPlutoLogger->Write(LV_DEBUG, "*******************12" );
#endif
					DCE::g_pPlutoLogger->Write(LV_CRITICAL, "ZWJobSwitchBinaryGet::processData, len too small %d", length);
					break;
				}
				if(length == 4 && buffer[2] != 2) //not my function ID
				{
					DCE::g_pPlutoLogger->Write(LV_CRITICAL, "ZWJobSwitchBinaryGet::processData, not for me, kill the job, because nothing will come");
					setState(ZWaveJob::STOPPED);
					return false;
				}
#ifdef PLUTO_DEBUG
g_pPlutoLogger->Write(LV_DEBUG, "*******************13" );

				DCE::g_pPlutoLogger->Write(LV_DEBUG, "ZWJobSwitchBinaryGet::processData the tx status is here");
#endif
				bool completedOK = false;
				switch(buffer[3])
				{
					case TRANSMIT_COMPLETE_OK:
						DCE::g_pPlutoLogger->Write(LV_DEBUG, "command completed OK");
#ifdef PLUTO_DEBUG
						g_pPlutoLogger->Write(LV_DEBUG, "*******************14" );
#endif
						completedOK = true;
						break;
					case TRANSMIT_COMPLETE_NO_ACK:
						DCE::g_pPlutoLogger->Write(LV_DEBUG, "command not ack");
#ifdef PLUTO_DEBUG
						g_pPlutoLogger->Write(LV_DEBUG, "*******************15" );
#endif
						break;
					case TRANSMIT_COMPLETE_FAIL:
						DCE::g_pPlutoLogger->Write(LV_DEBUG, "failed to transmit command");
#ifdef PLUTO_DEBUG
						g_pPlutoLogger->Write(LV_DEBUG, "*******************16" );
#endif
						break;
					default:
						DCE::g_pPlutoLogger->Write(LV_DEBUG, "unrecognized response coming as tx status");
#ifdef PLUTO_DEBUG
						g_pPlutoLogger->Write(LV_DEBUG, "*******************17" );
#endif
				}
				d->txStatusCount ++;
#ifdef PLUTO_DEBUG
				g_pPlutoLogger->Write(LV_DEBUG, "*******************18" );
#endif
				return completedOK;				
			}
			else//buffer[0] == REQUEST
			{
				if(length < 8)
				{
					DCE::g_pPlutoLogger->Write(LV_CRITICAL, "ZWJobSwitchBinaryGet::processData, length too small %d", length);
#ifdef PLUTO_DEBUG
					g_pPlutoLogger->Write(LV_DEBUG, "*******************19" );
#endif
					goto entry;
				}

				if(FUNC_ID_APPLICATION_COMMAND_HANDLER == buffer[1] &&
					COMMAND_CLASS_SWITCH_MULTILEVEL == buffer[5] && 
					(char)d->nodeID == buffer[3] &&
					BASIC_REPORT == buffer[6])
				{
#ifdef PLUTO_DEBUG
					DCE::g_pPlutoLogger->Write(LV_DEBUG, "ZWJobSwitchBinaryGet::processData basic multilevel swich report is here");
#endif
					DCE::g_pPlutoLogger->Write(LV_DEBUG, "ZWJobSwitchBinaryGet::processData basic report = 0x%x", buffer[7]);
					
					ZWaveNode * node = handler()->getNode( d->nodeID );
					if( node != NULL )
					{
						node->setLevel( (int)buffer[7] );
					}
					else
					{
						DCE::g_pPlutoLogger->Write(LV_CRITICAL, "ZWJobSwitchBinaryGet: the node is NULL");
					}
#ifdef PLUTO_DEBUG
					g_pPlutoLogger->Write(LV_DEBUG, "*******************20" );
#endif
				}
				else
				{
					DCE::g_pPlutoLogger->Write(LV_DEBUG, "ZWJobSwitchBinaryGet::processData basic multilevel swich report is NOT here");
					//break;
					//sorry (mailto: edgar.g@plutohome.com)
#ifdef PLUTO_DEBUG
					g_pPlutoLogger->Write(LV_DEBUG, "*******************21" );
#endif
					goto entry;
				}

				setState(ZWaveJob::STOPPED);
#ifdef PLUTO_DEBUG
g_pPlutoLogger->Write(LV_DEBUG, "*******************22" );
#endif
				return true;
			}
	}
#ifdef PLUTO_DEBUG
	g_pPlutoLogger->Write(LV_DEBUG, "*******************23" );
#endif
	return handler()->processData(buffer, length);
}
