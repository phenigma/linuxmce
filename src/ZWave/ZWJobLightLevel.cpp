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
#ifdef PLUTO_DEBUG
	g_pPlutoLogger->Write(LV_DEBUG, "==============1" );
#endif
}

ZWJobLightLevel::Private::~Private()
{
	delete currentJob;
	currentJob = NULL;
#ifdef PLUTO_DEBUG
	g_pPlutoLogger->Write(LV_DEBUG, "==============2" );
#endif
	
	ZWaveJob * job = NULL;
	for(JobsDequeIterator it=jobsQueue.begin(); it!=jobsQueue.end(); ++it)
	{
		job = (*it);
		delete job;
		job = NULL;
	}
#ifdef PLUTO_DEBUG
	g_pPlutoLogger->Write(LV_DEBUG, "==============3" );
#endif
}

// ----------------------------------

ZWJobLightLevel::ZWJobLightLevel(PlutoZWSerialAPI * zwAPI, unsigned char level, unsigned char nodeID)
	: ZWaveJob(zwAPI)
{
	d = new Private(level, nodeID);
	setType(ZWaveJob::LIGHT_LEVEL);
#ifdef PLUTO_DEBUG
	g_pPlutoLogger->Write(LV_DEBUG, "==============4" );
#endif
}

ZWJobLightLevel::~ZWJobLightLevel()
{
	delete d;
	d = NULL;
#ifdef PLUTO_DEBUG
	g_pPlutoLogger->Write(LV_DEBUG, "==============5" );
#endif
}

bool ZWJobLightLevel::run()
{
#ifdef PLUTO_DEBUG
	g_pPlutoLogger->Write(LV_DEBUG, "==============6" );
#endif
	// start the initialization with ZWave API version
	d->currentJob = new ZWJobSwitchChangeLevel(handler(), d->level, d->nodeID) ;
	if( d->currentJob == NULL )
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "ZWJobLightLevel memory allocation error!");
#ifdef PLUTO_DEBUG
		g_pPlutoLogger->Write(LV_DEBUG, "==============7" );
#endif
		return false;
	}
#ifdef PLUTO_DEBUG
	g_pPlutoLogger->Write(LV_ZWAVE, "Current job = ZWJobLightLevel");
#endif
	// add jobs
	//d->currentJob = d->jobsQueue.front();
	//d->jobsQueue.pop_front();
#ifdef PLUTO_DEBUG
	g_pPlutoLogger->Write(LV_DEBUG, "==============8" );
#endif
	
	time_t currentTime = time(NULL);
	if( !again() )
	{
		setStartTime( currentTime );
	}
	setAnswerTime( currentTime );
	
	setState( ZWaveJob::RUNNING );
#ifdef PLUTO_DEBUG
	g_pPlutoLogger->Write(LV_ZWAVE, "ZWJobLightLevel running");
#endif
	return d->currentJob->run();
}

bool ZWJobLightLevel::processData(const char * buffer, size_t length)
{
	SerialConnection::printDataBuffer(buffer, length, "ZWJobLightLevel");

#ifdef PLUTO_DEBUG
	g_pPlutoLogger->Write(LV_DEBUG, "==============9" );
#endif
	if( ZWaveJob::RUNNING != state() )
	{
#ifdef PLUTO_DEBUG
		g_pPlutoLogger->Write(LV_DEBUG, "==============10" );
#endif
		g_pPlutoLogger->Write(LV_ZWAVE, "ZWJobLightLevel: wrong job state.");
		return false;
	}
	
#ifdef PLUTO_DEBUG
	g_pPlutoLogger->Write(LV_DEBUG, "==============11" );
#endif
	if( d->currentJob != NULL )
	{
#ifdef PLUTO_DEBUG
		g_pPlutoLogger->Write(LV_DEBUG, "==============12" );
#endif
		if( !d->currentJob->processData(buffer, length) )
		{
g_pPlutoLogger->Write(LV_DEBUG, "==============13" );
			g_pPlutoLogger->Write(LV_ZWAVE, "ZWJobLightLevel: current job is returning false");
			setState(ZWaveJob::STOPPED);
			return false;
		}
	}
	else
	{
		g_pPlutoLogger->Write(LV_ZWAVE, "ZWJobLightLevel: current job is null");
		setState(ZWaveJob::STOPPED);
		return false;
	}
	
#ifdef PLUTO_DEBUG
	g_pPlutoLogger->Write(LV_DEBUG, "==============14" );
#endif
	
	// check if the job has finished
	if( ZWaveJob::STOPPED == d->currentJob->state() )
	{
#ifdef PLUTO_DEBUG
		g_pPlutoLogger->Write(LV_DEBUG, "==============15" );
#endif
		// next step
		switch( d->currentJob->type() )
		{
//			case ZWaveJob::LIGHT_LEVEL :
//				// TODO check the version
//				d->jobsQueue.push_back( );
//				break;
				
			case ZWaveJob::SET_SWITCH_LEVEL :
#ifdef PLUTO_DEBUG
				g_pPlutoLogger->Write(LV_DEBUG, "==============16" );
#endif
				d->jobsQueue.push_back( new ZWJobSwitchBinaryGet(handler(), d->nodeID) );
				break;
			case ZWaveJob::GET_SWITCH_BINARY:
#ifdef PLUTO_DEBUG
				g_pPlutoLogger->Write(LV_DEBUG, "==============17" );
#endif
				//nothing
				break;
			default:
#ifdef PLUTO_DEBUG
				g_pPlutoLogger->Write(LV_DEBUG, "==============18" );
#endif
				g_pPlutoLogger->Write(LV_ZWAVE, "ZWJobLightLevel: wrong job type.");
				break;
		}
#ifdef PLUTO_DEBUG
		g_pPlutoLogger->Write(LV_DEBUG, "==============19" );
#endif
		
		// next job
		delete d->currentJob;
		d->currentJob = NULL;
#ifdef PLUTO_DEBUG
		g_pPlutoLogger->Write(LV_DEBUG, "==============20" );
#endif
		if( d->jobsQueue.size() > 0 )
		{
			d->currentJob = d->jobsQueue.front();
			d->jobsQueue.pop_front();
#ifdef PLUTO_DEBUG
			g_pPlutoLogger->Write(LV_DEBUG, "==============21" );
#endif
			bool returnValue = d->currentJob->run();
			if(!returnValue)
			{
#ifdef PLUTO_DEBUG
				g_pPlutoLogger->Write(LV_DEBUG, "==============22" );
#endif
				setState(ZWaveJob::STOPPED);
			}
#ifdef PLUTO_DEBUG
			g_pPlutoLogger->Write(LV_DEBUG, "==============23" );
#endif
			setAnswerTime( time(NULL) );
			
			return true;
		}
		else
		{
			setState(ZWaveJob::STOPPED);
#ifdef PLUTO_DEBUG
			g_pPlutoLogger->Write(LV_DEBUG, "==============24" );
#endif
			setAnswerTime( time(NULL) );
			
			return true;
		}
	}
#ifdef PLUTO_DEBUG
	g_pPlutoLogger->Write(LV_DEBUG, "==============25" );
#endif
	setAnswerTime( time(NULL) );
	return true;
}
