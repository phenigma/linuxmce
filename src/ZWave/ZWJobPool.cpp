//
// C++ Implementation: ZWJobPool
//
// Description: 
//
//
// Author: Eugen Constantinescu <eugen.c@plutohome.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "ZWJobPool.h"
#include "ZW_SerialAPI.h"
#include "ZW_classcmd.h"
#include "PlutoZWSerialAPI.h"
#include "main.h"

// jobs
#include "ZWJobPool.h"
#include "ZWJobSwitchBinaryGet.h"
#include "SerialConnection.h"

#include <stdio.h>

class ZWJobPool::Private
{
	public:
	
		Private();
		
		~Private();
		
		JobsDeque jobsQueue;
		ZWaveJob * currentJob;
	
	private:
};

ZWJobPool::Private::Private()
	: currentJob(NULL)
{
}

ZWJobPool::Private::~Private()
{
#ifdef PLUTO_DEBUG
		g_pPlutoLogger->Write(LV_DEBUG, "----- POOL ---- 1");
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
	g_pPlutoLogger->Write(LV_DEBUG, "----- POOL ---- 2");
#endif
}

ZWJobPool::ZWJobPool(PlutoZWSerialAPI * zwAPI)
	: ZWaveJob(zwAPI)
{
	d = new Private();
	setType(ZWaveJob::INITIALIZE);
}

ZWJobPool::~ZWJobPool()
{
#ifdef PLUTO_DEBUG
	handler()->showZWaveNetwork();
#endif

	delete d;
	d = NULL;
}

bool ZWJobPool::run()
{
	// get the information about all the nodes from the ZWave network
	const NodesMap& nodes = handler()->getNodes();
	ZWJobSwitchBinaryGet * job = NULL;
	ZWaveNode * node = NULL;
	for(NodesMapCIterator it=nodes.begin(); it!=nodes.end(); ++it)
	{
		node = (*it).second;
		if( node != NULL )
		{
			// set as unchanged to detect what nodes are changing
			node->changed(false);
			
			NodeType type = node->type();
			if( node->nodeID() != handler()->nodeID() )
			{
				// TODO: check the specific type too
				switch( type.generic )
				{
					// dimmer
					case GENERIC_TYPE_SWITCH_MULTILEVEL:
					// light on/off
					case GENERIC_TYPE_SWITCH_BINARY:
					{
						job = new ZWJobSwitchBinaryGet(handler(), node->nodeID());
						if( job != NULL )
						{
							d->jobsQueue.push_back(job);
						}
						else
						{
							g_pPlutoLogger->Write(LV_CRITICAL, "ZWJobPool allocation error!");
							return false;
						}
						break;
					}
						
					default:
						break;
				}
			}
		}
	}
	
	if( d->jobsQueue.size() > 0 )
	{
		d->currentJob = d->jobsQueue.front();
		d->jobsQueue.pop_front();
	}
	else
	{
		g_pPlutoLogger->Write(LV_ZWAVE, "ZWJobPool no lights available!");
		return false;
	}
	
	setState( ZWaveJob::RUNNING );
	
	return d->currentJob->run();
}

bool ZWJobPool::processData(const char* buffer, size_t length)
{
	SerialConnection::printDataBuffer(buffer, length, "ZWJobPool");

	if( ZWaveJob::RUNNING != state() )
	{
		g_pPlutoLogger->Write(LV_ZWAVE, "ZWJobPool: wrong job state.");
		return false;
	}
	
	if( d->currentJob != NULL )
	{
		if( !d->currentJob->processData(buffer, length) )
		{
			g_pPlutoLogger->Write(LV_ZWAVE, "ZWJobPool: current job returned an error");
//			try another node
//			return false;
		}
	}
	else
	{
		g_pPlutoLogger->Write(LV_ZWAVE, "ZWJobPool: current job is null");
		setState(ZWaveJob::STOPPED);
		return false;
	}
	
	// check if the job has finished
	if( ZWaveJob::STOPPED == d->currentJob->state() )
	{
		// next step
		switch( d->currentJob->type() )
		{
			case ZWaveJob::GET_SWITCH_BINARY :
				break;
			
			default:
				g_pPlutoLogger->Write(LV_ZWAVE, "ZWJobPool: wrong job type.");
				return false;
				break;
		}
		
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
			
			return true;
		}
		else
		{
#ifdef PLUTO_DEBUG
			g_pPlutoLogger->Write(LV_DEBUG, "----- POOL ---- 3");
#endif
			setState(ZWaveJob::STOPPED);
			return true;
		}
	}
	
	return true;
}

