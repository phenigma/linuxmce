#include "StdAfx.h"
#include ".\zwjobgetversion.h"

ZWJobGetVersion::ZWJobGetVersion(PlutoZWSerialAPI* ser):
	ZWaveJob(ser)
{

}

ZWJobGetVersion::~ZWJobGetVersion()
{

}

bool ZWJobGetVersion::run()
{
	setState(ZWaveJob::RUNNING);
	//unsigned char data[] = {}
	setState(ZWaveJob::IDLE);
	return true;
}

bool ZWJobGetVersion::processData(const char * buffer, size_t length)
{
	return true;
}
