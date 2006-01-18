#include "ZWJobLightLevel.h"
#include "ZW_SerialAPI.h"
#include "PlutoZWSerialAPI.h"
#include "main.h"

// jobs
#include "ZWJobSwitchBinaryGet.h"
#include "ZWJobSwitchChangeLevel.h"
#include "SerialConnection.h"

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
g_pPlutoLogger->Write(LV_DEBUG, "==============1" );
}

ZWJobLightLevel::Private::~Private()
{
	delete currentJob;
	currentJob = NULL;
g_pPlutoLogger->Write(LV_DEBUG, "==============2" );
	
	ZWaveJob * job = NULL;
	for(JobsDequeIterator it=jobsQueue.begin(); it!=jobsQueue.end(); ++it)
	{
		job = (*it);
		delete job;
		job = NULL;
	}
g_pPlutoLogger->Write(LV_DEBUG, "==============3" );
}

// ----------------------------------

ZWJobLightLevel::ZWJobLightLevel(PlutoZWSerialAPI * zwAPI, unsigned char level, unsigned char nodeID)
	: ZWaveJob(zwAPI)
{
	d = new Private(level, nodeID);
	setType(ZWaveJob::LIGHT_LEVEL);
g_pPlutoLogger->Write(LV_DEBUG, "==============4" );
}

ZWJobLightLevel::~ZWJobLightLevel()
{
	delete d;
	d = NULL;
g_pPlutoLogger->Write(LV_DEBUG, "==============5" );
}

bool ZWJobLightLevel::run()
{
g_pPlutoLogger->Write(LV_DEBUG, "==============6" );
	// start the initialization with ZWave API version
	d->currentJob = new ZWJobSwitchChangeLevel(handler(), d->level, d->nodeID) ;
	if( d->currentJob == NULL )
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "ZWJobLightLevel allocation error!");
g_pPlutoLogger->Write(LV_DEBUG, "==============7" );
		return false;
	}
	g_pPlutoLogger->Write(LV_DEBUG, "Current job = ZWJobLightLevel");
	// add jobs
	//d->currentJob = d->jobsQueue.front();
	//d->jobsQueue.pop_front();
g_pPlutoLogger->Write(LV_DEBUG, "==============8" );
	
	setState( ZWaveJob::RUNNING );
	g_pPlutoLogger->Write(LV_DEBUG, "ZWJobLightLevel running");
	return d->currentJob->run();
}

bool ZWJobLightLevel::processData(const char * buffer, size_t length)
{
	SerialConnection::printDataBuffer(buffer, length, "ZWJobLightLevel");

g_pPlutoLogger->Write(LV_DEBUG, "==============9" );
	if( ZWaveJob::RUNNING != state() )
	{
g_pPlutoLogger->Write(LV_DEBUG, "==============10" );
		g_pPlutoLogger->Write(LV_WARNING, "ZWJobLightLevel: wrong job state.");
		return false;
	}
	
g_pPlutoLogger->Write(LV_DEBUG, "==============11" );
	if( d->currentJob != NULL )
	{
g_pPlutoLogger->Write(LV_DEBUG, "==============12" );
		if( !d->currentJob->processData(buffer, length) )
		{
g_pPlutoLogger->Write(LV_DEBUG, "==============13" );
			g_pPlutoLogger->Write(LV_WARNING, "ZWJobLightLevel: current job is returning false");
			setState(ZWaveJob::STOPPED);
			return false;
		}
	}
g_pPlutoLogger->Write(LV_DEBUG, "==============14" );
	
	// check if the job has finished
	if( ZWaveJob::STOPPED == d->currentJob->state() )
	{
g_pPlutoLogger->Write(LV_DEBUG, "==============15" );
		// next step
		switch( d->currentJob->type() )
		{
//			case ZWaveJob::LIGHT_LEVEL :
//				// TODO check the version
//				d->jobsQueue.push_back( );
//				break;
				
			case ZWaveJob::SET_SWITCH_LEVEL :
g_pPlutoLogger->Write(LV_DEBUG, "==============16" );
				d->jobsQueue.push_back( new ZWJobSwitchBinaryGet(handler(), d->nodeID) );
				break;
			case ZWaveJob::GET_SWITCH_BINARY:
g_pPlutoLogger->Write(LV_DEBUG, "==============17" );
				//nothing
				break;
			default:
g_pPlutoLogger->Write(LV_DEBUG, "==============18" );
				g_pPlutoLogger->Write(LV_WARNING, "ZWJobLightLevel: wrong job type.");
				break;
		}
g_pPlutoLogger->Write(LV_DEBUG, "==============19" );
		
		// next job
		delete d->currentJob;
		d->currentJob = NULL;
g_pPlutoLogger->Write(LV_DEBUG, "==============20" );
		if( d->jobsQueue.size() > 0 )
		{
			d->currentJob = d->jobsQueue.front();
			d->jobsQueue.pop_front();
g_pPlutoLogger->Write(LV_DEBUG, "==============21" );
			bool returnValue = d->currentJob->run();
			if(!returnValue)
			{
g_pPlutoLogger->Write(LV_DEBUG, "==============22" );
				setState(ZWaveJob::STOPPED);
			}
g_pPlutoLogger->Write(LV_DEBUG, "==============23" );
			return true;
		}
		else
		{
			setState(ZWaveJob::STOPPED);
g_pPlutoLogger->Write(LV_DEBUG, "==============24" );
			return true;
		}
	}
g_pPlutoLogger->Write(LV_DEBUG, "==============25" );
	return true;
}
