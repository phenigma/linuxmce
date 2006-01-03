#include "ZWJobInitialize.h"
#include "ZW_SerialAPI.h"
#include "PlutoZWSerialAPI.h"
#include "main.h"

#include <stdio.h>

// ----------------------------------

class ZWJobInitialize::Private
{
	public:
	
		Private();
		
		~Private();
		
		JobsDeque jobsQueue;
		ZWaveJob * currentJob;
	
	private:
};

ZWJobInitialize::Private::Private()
	: currentJob(NULL)
{
}

ZWJobInitialize::Private::~Private()
{
}

// ----------------------------------

ZWJobInitialize::ZWJobInitialize(PlutoZWSerialAPI * zwAPI)
	: ZWaveJob(zwAPI)
{
	d = new Private();
	setType(ZWaveJob::INITIALIZE);
}

ZWJobInitialize::~ZWJobInitialize()
{
	delete d;
	d = NULL;
}

bool ZWJobInitialize::run()
{
	return true;
}

bool ZWJobInitialize::processData(const char * buffer, size_t length)
{
	return false;
}
