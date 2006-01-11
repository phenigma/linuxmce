//
// C++ Implementation: zwjobaddnode
//
// Description: 
//
//
// Author: Eugen Constantinescu <eugen.c@plutohome.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "ZWJobAddNode.h"
#include "ZW_SerialAPI.h"
#include "PlutoZWSerialAPI.h"
#include "main.h"
#include "ZW_classcmd.h"

#include "ZWJobSetLearnNodeState.h"
#include "ZWJobGetNodeProtocolInfo.h"

#include <stdio.h>
#include <string.h>

// ----------------------------------

class ZWJobAddNode::Private
{
	public:
	
		Private();
		~Private();
	
		JobsDeque jobsQueue;
		ZWaveJob * currentJob;
		
	private:
};

ZWJobAddNode::Private::Private()
	: currentJob(NULL)
{
}

ZWJobAddNode::Private::~Private()
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

ZWJobAddNode::ZWJobAddNode(PlutoZWSerialAPI * zwAPI)
	: ZWaveJob(zwAPI)
{
	d = new Private();
	setType(ZWaveJob::ADD_NODE);
}

ZWJobAddNode::~ZWJobAddNode()
{
	delete d;
	d = NULL;
}

bool ZWJobAddNode::processData(const char* buffer, size_t length)
{
	if( d->currentJob != NULL )
	{
		if( !d->currentJob->processData(buffer, length) )
		{
			g_pPlutoLogger->Write(LV_WARNING, "ZWJobAddNode: .");
			return false;
		}
	}
	
	// check if the job has finished
	if( ZWaveJob::STOPPED == d->currentJob->state() )
	{
		// next step
		switch( d->currentJob->type() )
		{
			case ZWaveJob::GET_ID :
				d->jobsQueue.push_back( new ZWJobGetNodeProtocolInfo(handler()) );
				break;
				
			default:
				g_pPlutoLogger->Write(LV_WARNING, "ZWJobInitialize: wrong job type.");
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

bool ZWJobAddNode::run()
{
	// start the initialization with ZWave API version
	d->currentJob = new ZWJobSetLearnNodeState(handler());
	if( d->currentJob == NULL )
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "ZWJobInitialize allocation error!");
		return false;
	}
	((ZWJobSetLearnNodeState*)d->currentJob)->setMode( LEARN_NODE_STATE_NEW );
	
	setState( ZWaveJob::RUNNING );
	
	return d->currentJob->run();
}

