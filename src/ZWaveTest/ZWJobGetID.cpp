
#include "zwjobgetid.h"

#include <stdio.h>

// ----------------------------------

class ZWJobGetID::Private
{
	public:
	
		Private();
		~Private();
	
	private:
};

ZWJobGetID::Private::Private()
{
}

ZWJobGetID::Private::~Private()
{
}

// ----------------------------------

ZWJobGetID::ZWJobGetID(PlutoZWSerialAPI * zwAPI)
	: ZWaveJob(zwAPI)
{
	d = new Private();
}

ZWJobGetID::~ZWJobGetID(void)
{
	delete d;
	d = NULL;
}

bool ZWJobGetID::run()
{
	return true;
}

bool ZWJobGetID::processData(const char * buffer, size_t length)
{
	return false;
}
