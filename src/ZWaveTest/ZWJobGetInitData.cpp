#include "ZWJobGetInitData.h"
#include "PlutoZWSerialAPI.h"
#include "ZW_SerialAPI.h"
#include "SerialConnection.h"

//this number is hard coded into docs, so I will use it as it is
#define MAGIC_LEN 29 

ZWJobGetInitData::ZWJobGetInitData(PlutoZWSerialAPI* ser):
	ZWaveJob(ser)
{
	setType(ZWaveJob::GET_INIT_DATA);
}


ZWJobGetInitData::~ZWJobGetInitData(void)
{
}

/** It is called to perform the job.*/
bool ZWJobGetInitData::run()
{
	setState(ZWaveJob::RUNNING);
	char data[] = {REQUEST, FUNC_ID_SERIAL_API_GET_INIT_DATA};
	return handler()->sendData(data, 2);	
}

/** It is called to process the protocol data flow.*/
bool ZWJobGetInitData::processData(const char * buffer, size_t length)
{
	SerialConnection::printDataBuffer(buffer, length, "ZWJobGetInitData");

	switch(state())
	{
		default:
		case ZWaveJob::IDLE:
		case ZWaveJob::STOPPED:
			break;
		case ZWaveJob::RUNNING:
			if(length < 2)
				break;
			if(buffer[0] != RESPONSE || buffer[1] != FUNC_ID_SERIAL_API_GET_INIT_DATA)
				break;
			if(buffer[4] != MAGIC_LEN) //hard coded into docs
				break;
			//save capabilities
			handler()->setCapabilities(buffer[3]);
			//save version
			handler()->setFirmwareVersion(buffer[2]);
			//get the nodes
			unsigned short node_index = 0;
			for(int i = 5; i < 9; i++)
			{
				for(int j = 0; j < 8; j++)
				{
					node_index++;
					if( (1 << j) & buffer[i])
						handler()->insertNode(new ZWaveNode(handler()->homeID(), node_index));
				}
			}
			setState(ZWaveJob::STOPPED);
			return true;
	}
	return false;
}

