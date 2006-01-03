#include "ZWJobGetInitData.h"
#include "PlutoZWSerialAPI.h"
#include "ZW_SerialAPI.h"

ZWJobGetInitData::ZWJobGetInitData(PlutoZWSerialAPI* ser):
	ZWaveJob(ser)
{
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
			if(buffer[3] != 29) //hard coded into docs
				break;
	}
	return false;
}

