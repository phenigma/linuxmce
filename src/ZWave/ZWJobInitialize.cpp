//
// C++ Interface: ZWJobIntialize
//
// Description: ZWave initialization, looking for nodes
//
//
// Author: Eugen Constantinescu <eugen.c@plutohome.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "ZWJobInitialize.h"
#include "ZW_SerialAPI.h"
#include "PlutoZWSerialAPI.h"
#include "main.h"

// jobs
#include "ZWJobGetVersion.h"
#include "ZWJobGetID.h"
#include "ZWJobGetInitData.h"
#include "ZWJobGetSUC.h"
#include "ZWJobGetNodeProtocolInfo.h"
#include "SerialConnection.h"

#include <stdio.h>

// ----------------------------------

class ZWJobInitialize::Private
{
	public:
	
		Private();
		
		~Private();
		
		JobsDeque jobsQueue;
		ZWaveJob * currentJob;
		unsigned triesCount;
	
	private:
};

ZWJobInitialize::Private::Private()
	: currentJob(NULL),
	  triesCount(0)
{
}

ZWJobInitialize::Private::~Private()
{
#ifdef PLUTO_DEBUG
		g_pPlutoLogger->Write(LV_DEBUG, "----- INIT ---- 1");
#endif
	delete currentJob;
	currentJob = NULL;
	
	ZWaveJob * job = NULL;
	for(JobsDequeIterator it=jobsQueue.begin(); it!=jobsQueue.end(); ++it)
	{
		job = (*it);
		delete job;
		job = NULL;
	}
#ifdef PLUTO_DEBUG
	g_pPlutoLogger->Write(LV_DEBUG, "----- INIT ---- 2");
#endif
}

// ----------------------------------

ZWJobInitialize::ZWJobInitialize(PlutoZWSerialAPI * zwAPI)
	: ZWaveJob(zwAPI)
{
	d = new Private();
	setType(ZWaveJob::INITIALIZE);
	setReceivingTimeout( 4 );
}

ZWJobInitialize::~ZWJobInitialize()
{
	delete d;
	d = NULL;
}

bool ZWJobInitialize::run()
{
	// start the initialization with ZWave API version
	d->currentJob = new ZWJobGetVersion(handler());
	if( d->currentJob == NULL )
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "ZWJobInitialize allocation error!");
		return false;
	}
	
	// add jobs
	//d->currentJob = d->jobsQueue.front();
	//d->jobsQueue.pop_front();
	
	time_t currentTime = time(NULL);
	
	if( !again() )
	{
		setStartTime( currentTime );
	}
	setAnswerTime( currentTime );
	
	setState( ZWaveJob::RUNNING );
	
	// for safeness
	handler()->clearNodes();
	
	return d->currentJob->run();
}

bool ZWJobInitialize::processData(const char * buffer, size_t length)
{
	SerialConnection::printDataBuffer(buffer, length, "ZWJobInitialize");

	if( ZWaveJob::RUNNING != state() )
	{
		g_pPlutoLogger->Write(LV_ZWAVE, "ZWJobInitialize: wrong job state.");
		return false;
	}
	
	if( d->currentJob != NULL )
	{
		if( !d->currentJob->processData(buffer, length) )
		{
			g_pPlutoLogger->Write(LV_ZWAVE, "ZWJobInitialize: current job returned an error");
			setState(ZWaveJob::STOPPED);
			return false;
		}
	}
	else
	{
		g_pPlutoLogger->Write(LV_ZWAVE, "ZWJobInitialize: current job is null");
		setState(ZWaveJob::STOPPED);
		return false;
	}
	
	// check if the job has finished
	if( ZWaveJob::STOPPED == d->currentJob->state() )
	{
		// next step
		switch( d->currentJob->type() )
		{
			case ZWaveJob::GET_VERSION :
				// TODO check the version
				d->jobsQueue.push_back( new ZWJobGetID(handler()) );
				break;
				
			case ZWaveJob::GET_ID :
				d->jobsQueue.push_back( new ZWJobGetInitData(handler()) );
				break;
				
			case ZWaveJob::GET_INIT_DATA :
				{
					// check if there is a SUC
					d->jobsQueue.push_back( new ZWJobGetSUC(handler()) );
					
					// We have to disable the timeout because some nodes can be offline
					setReceivingTimeout( 0 );
					
					// get the information about all the nodes from the ZWave network
					const NodesMap& nodes = handler()->getNodes();
					ZWJobGetNodeProtocolInfo * job = NULL;
					for(NodesMapCIterator it=nodes.begin(); it!=nodes.end(); ++it)
					{
						job = new ZWJobGetNodeProtocolInfo(handler());
						if( job != NULL )
						{
							job->setNodeID( (*it).second->nodeID() );
							d->jobsQueue.push_back(job);
						}
						else
						{
							g_pPlutoLogger->Write(LV_CRITICAL, "ZWJobInitialize allocation error!");
							return false;
						}
					}
				}
				break;
				
			case ZWaveJob::GET_SUC :
				// TODO: what's up if there is a SUC
				break;
				
			case ZWaveJob::GET_NODE_PROTOCOL_INFO :
				// nothing
				break;
				
			default:
				g_pPlutoLogger->Write(LV_ZWAVE, "ZWJobInitialize: wrong job type.");
				return false;
				break;
		}
		
		d->triesCount = 0;
		
		// next job
		delete d->currentJob;
		d->currentJob = NULL;
		if( d->jobsQueue.size() > 0 )
		{
			d->currentJob = d->jobsQueue.front();
			d->jobsQueue.pop_front();
			
			if( !d->currentJob->run() )
			{
				setState(ZWaveJob::STOPPED);
				return false;
			}
			
			setAnswerTime( time(NULL) );
			return true;
		}
		else
		{
#ifdef PLUTO_DEBUG
			g_pPlutoLogger->Write(LV_DEBUG, "----- INIT ---- 3");
#endif
			setState(ZWaveJob::STOPPED);
			setAnswerTime( time(NULL) );
			return true;
		}
	}
	
	setAnswerTime( time(NULL) );
	return true;
}

void ZWJobInitialize::timeoutHandler()
{
#ifdef PLUTO_DEBUG
	g_pPlutoLogger->Write(LV_WARNING, "ZWJobInitialize::timeoutHandler");
#endif

	setAnswerTime( time(NULL) );
	
	if( d->currentJob != NULL && d->triesCount < 3 )
	{
		d->triesCount++;
		
		// try again the current job
		if( !d->currentJob->runAgain() )
		{
			setState(ZWaveJob::STOPPED);
		}
	}
	else
	{
		setState(ZWaveJob::STOPPED);
	}
}
