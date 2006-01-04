#include "ZWJobGetVersion.h"
#include "PlutoZWSerialAPI.h"
#include "ZW_SerialAPI.h"
#include "main.h"

#include <string>

ZWJobGetVersion::ZWJobGetVersion(PlutoZWSerialAPI* ser):
	ZWaveJob(ser)
{
	setType(ZWaveJob::GET_VERSION);
}

ZWJobGetVersion::~ZWJobGetVersion()
{

}

bool ZWJobGetVersion::run()
{
	setState(ZWaveJob::RUNNING);
	char data[] = {REQUEST, FUNC_ID_ZW_GET_VERSION };
	size_t len = 2;
	return handler()->sendData(data, len);;
}

bool ZWJobGetVersion::processData(const char * buffer, size_t length)
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
				DCE::g_pPlutoLogger->Write(LV_CRITICAL, "ZWJobGetVersion::processData, length too small");
				break;
			}
			if(buffer[0] != RESPONSE || buffer[1] != FUNC_ID_ZW_GET_VERSION)
			{
				DCE::g_pPlutoLogger->Write(LV_CRITICAL, "ZWJobGetVersion::processData, buffer incorrect");
				break;				
			}
			std::string version(&(buffer[2]));
			DCE::g_pPlutoLogger->Write(LV_DEBUG, version.c_str());
			handler()->setVersion(version);
			setState(ZWaveJob::STOPPED);
			return true;
	}
	return false;
}
