#include "ZWJobLightLevel.h"
#include "ZW_SerialAPI.h"
#include "PlutoZWSerialAPI.h"
#include "main.h"

// jobs
#include "ZWJobSwitchBinaryGet.h"
#include "ZWJobSwitchChangeLevel.h"

#include <stdio.h>

// ----------------------------------

class ZWJobLightLevel::Private
{
	public:
	
		Private(unsigned char l, unsigned char nID);
		
		~Private();
		
		JobsDeque jobsQueue;
		ZWaveJob * currentJob;
		unsigned char level;
		unsigned char nodeID;
	
	private:
};

ZWJobLightLevel::Private::Private(unsigned char l, unsigned char nID)
	: currentJob(NULL),
	level(l),
	nodeID(nID)
{
}

ZWJobLightLevel::Private::~Private()
{
	delete currentJob;
	currentJob = NULL;
	
	ZWaveJob * job = NULL;
	for(JobsDequeIterator it=jobsQueue.begin(); it!=jobsQueue.end(); ++it)
	{
		job = (*it);
		delete job;
		job = NULL;
	}
}

// ----------------------------------

ZWJobLightLevel::ZWJobLightLevel(PlutoZWSerialAPI * zwAPI, unsigned char level, unsigned char nodeID)
	: ZWaveJob(zwAPI)
{
	d = new Private(level, nodeID);
	setType(ZWaveJob::LIGHT_LEVEL);
}

ZWJobLightLevel::~ZWJobLightLevel()
{
	delete d;
	d = NULL;
}

bool ZWJobLightLevel::run()
{
	// start the initialization with ZWave API version
	d->currentJob = new ZWJobSwitchChangeLevel(handler(), d->level, d->nodeID) ;
	if( d->currentJob == NULL )
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "ZWJobLightLevel allocation error!");
		return false;
	}
	
	// add jobs
	//d->currentJob = d->jobsQueue.front();
	//d->jobsQueue.pop_front();
	
	setState( ZWaveJob::RUNNING );
	
	return d->currentJob->run();
}

bool ZWJobLightLevel::processData(const char * buffer, size_t length)
{
	if( d->currentJob != NULL )
	{
		if( !d->currentJob->processData(buffer, length) )
		{
			g_pPlutoLogger->Write(LV_WARNING, "ZWJobLightLevel: .");
			return false;
		}
	}
	
	// check if the job has finished
	if( ZWaveJob::STOPPED == d->currentJob->state() )
	{
		// next step
		switch( d->currentJob->type() )
		{
//			case ZWaveJob::LIGHT_LEVEL :
//				// TODO check the version
//				d->jobsQueue.push_back( );
//				break;
				
			case ZWaveJob::SET_SWITCH_LEVEL :
				d->jobsQueue.push_back( new ZWJobSwitchBinaryGet(handler(), d->nodeID) );
				break;
				
			default:
				g_pPlutoLogger->Write(LV_WARNING, "ZWJobLightLevel: wrong job type.");
				break;
		}
		
		// next job
		delete d->currentJob;
		d->currentJob = NULL;
		if( d->jobsQueue.size() > 0 )
		{
			d->currentJob = d->jobsQueue.front();
			d->jobsQueue.pop_front();
			return d->currentJob->run();
		}
		else
		{
			setState(ZWaveJob::STOPPED);
			return true;
		}
	}
	
	return true;
}
