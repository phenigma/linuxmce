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
		ZWaveJob::ZW_JOB type;
		
	private:
};

ZWaveJob::Private::Private(ZW_SerialAPI * zwAPI)
	: handler(zwAPI),
	  state(ZWaveJob::IDLE),
	  type(ZWaveJob::NOTHING)
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

ZWaveJob::ZW_JOB ZWaveJob::type() const
{
	return d->type;
}

void ZWaveJob::setType(ZWaveJob::ZW_JOB type)
{
	d->type = type;
}

ZW_SerialAPI* ZWaveJob::handler() const
{
	return d->handler;
}
