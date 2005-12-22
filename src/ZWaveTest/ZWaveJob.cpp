#include "zwavejob.h"

#include "main.h"
#include "SerialConnection.h"

class ZWaveJob::Private
{
	public:
	
		Private(ZW_SerialAPI *);
		~Private();
		
		ZW_SerialAPI * handler;
		ZWaveJob::JobState state;
		
	private:
};

ZWaveJob::Private::Private(ZW_SerialAPI * zwAPI)
	: handler(zwAPI),
	  state(ZWaveJob::IDLE)
{
}

ZWaveJob::Private::~Private()
{
}


ZWaveJob::ZWaveJob(ZW_SerialAPI * zwAPI)
{
	d = new Private(zwAPI);
	if( d == NULL )
	{
		// error message
	}
}

ZWaveJob::~ZWaveJob(void)
{
	delete d;
	d = NULL;
}

ZWaveJob::JobState ZWaveJob::state() const
{
	return d->state;
}

void ZWaveJob::setState(ZWaveJob::JobState st)
{
	d->state = st;
}
		
