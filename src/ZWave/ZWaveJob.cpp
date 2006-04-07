#include "ZWaveJob.h"

#include "main.h"
#include "SerialConnection.h"
#include "PlutoZWSerialAPI.h"

class ZWaveJob::Private
{
	public:
	
		Private(PlutoZWSerialAPI *);
		~Private();
		
		PlutoZWSerialAPI * handler;
		ZWaveJob::JobState state;
		ZWaveJob::ZW_JOB type;
		time_t receivingTimeout;
		time_t runningTimeout;
		time_t startTime;
		time_t answerTime;
		bool again;
		
	private:
};

ZWaveJob::Private::Private(PlutoZWSerialAPI * zwAPI)
	: handler(zwAPI),
	  state(ZWaveJob::IDLE),
	  type(ZWaveJob::NOTHING),
	  receivingTimeout(0),
	  runningTimeout(20),
	  startTime(0),
	  answerTime(0),
	  again(false)
{
}

ZWaveJob::Private::~Private()
{
}


ZWaveJob::ZWaveJob(PlutoZWSerialAPI * zwAPI)
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

PlutoZWSerialAPI* ZWaveJob::handler() const
{
	return d->handler;
}

void ZWaveJob::timeoutHandler()
{
	// just nothing here
	// each job will have its implementation, if needed
}

bool ZWaveJob::hasReceivingTimeout()
{
	return (d->receivingTimeout != 0);
}

void ZWaveJob::setReceivingTimeout(time_t timeout)
{
	d->receivingTimeout = timeout;
}

time_t ZWaveJob::receivingTimeout() const
{
	return d->receivingTimeout;
}

time_t ZWaveJob::runningTimeout() const
{
	return d->runningTimeout;
}

void ZWaveJob::setRunningTimeout(time_t timeout)
{
	d->runningTimeout = timeout;
}

time_t ZWaveJob::startTime() const
{
	return d->startTime;
}

void ZWaveJob::setStartTime(time_t start)
{
	d->startTime = start;
}

time_t ZWaveJob::answerTime() const
{
	return d->answerTime;
}

void ZWaveJob::setAnswerTime(time_t answer)
{
	d->answerTime = answer;
}

bool ZWaveJob::runAgain()
{
	d->again = true;
	bool bRet = run();
	d->again = false;
	
	return bRet;
}

bool ZWaveJob::again() const
{
	return d->again;
}
