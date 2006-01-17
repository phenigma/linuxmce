//
// C++ Implementation: ZWJobAddNode
//
// Description: ZWave Add Node Command
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
	if( ZWaveJob::RUNNING != state() )
	{
		g_pPlutoLogger->Write(LV_WARNING, "ZWJobAddNode: wrong job state.");
		return false;
	}
	
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
			case ZWaveJob::SET_LEARN_NODE_STATE :
			{
				if( LEARN_NODE_STATE_NEW == ((ZWJobSetLearnNodeState*)(d->currentJob))->mode() )
				{
					char learnInfo[LEARN_INFO_MAX];
					size_t learnInfoLength = LEARN_INFO_MAX;
					if( !((ZWJobSetLearnNodeState*)(d->currentJob))->learnInfo(learnInfo, &learnInfoLength) &&
						learnInfoLength )
					{
						// LEARN_INFO = Status Byte, NodeID Byte, Param Length Byte, Param[0]...Param[Length-1]
						// ZWJobSetLearnNodeState was running well
						// Just ask info about the node
						// Then set the learning off
						ZWJobGetNodeProtocolInfo * nodeInfo = new ZWJobGetNodeProtocolInfo(handler());
						ZWJobSetLearnNodeState * learnOff = new ZWJobSetLearnNodeState(handler());
						if( nodeInfo != NULL && learnOff != NULL )
						{
							nodeInfo->setNodeID( learnInfo[1] );
							d->jobsQueue.push_back( nodeInfo );
							
							learnOff->setMode( LEARN_NODE_STATE_OFF );
							d->jobsQueue.push_back( learnOff );
						}
						else
						{
							g_pPlutoLogger->Write(LV_CRITICAL, "ZWJobAddNode allocation error!");
							
							delete nodeInfo;
							nodeInfo = NULL;
							
							delete learnOff;
							learnOff = NULL;
						}
					}
					else
					{
						g_pPlutoLogger->Write(LV_CRITICAL, "ZWJobAddNode SET_LEARN_NODE_STATE FAILURE!");
					}
				}
				else
				{
					// just nothing to do
				}
				break;
			}
				
			case ZWaveJob::GET_NODE_PROTOCOL_INFO :
				// just nothing to do
				break;
				
			default:
				g_pPlutoLogger->Write(LV_WARNING, "ZWJobAddNode: wrong job type.");
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
		g_pPlutoLogger->Write(LV_CRITICAL, "ZWJobAddNode allocation error!");
		return false;
	}
	((ZWJobSetLearnNodeState*)d->currentJob)->setMode( LEARN_NODE_STATE_NEW );
	
	setState( ZWaveJob::RUNNING );
	
	return d->currentJob->run();
}

