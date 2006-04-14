//
// C++ Implementation: ZWJobRemoveNode
//
// Description: ZWave Remove Node Command
//
//
// Author: Eugen Constantinescu <eugen.c@plutohome.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "ZWJobRemoveNode.h"
#include "ZW_SerialAPI.h"
#include "PlutoZWSerialAPI.h"
#include "main.h"
#include "ZW_classcmd.h"

#include "ZWJobSetLearnNodeState.h"
#include "ZWJobGetNodeProtocolInfo.h"
#include "SerialConnection.h"

#include <stdio.h>
#include <string.h>

// ----------------------------------

class ZWJobRemoveNode::Private
{
	public:
	
		Private();
		~Private();
	
		JobsDeque jobsQueue;
		ZWaveJob * currentJob;
		
	private:
};

ZWJobRemoveNode::Private::Private()
	: currentJob(NULL)
{
}

ZWJobRemoveNode::Private::~Private()
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

ZWJobRemoveNode::ZWJobRemoveNode(PlutoZWSerialAPI * zwAPI)
	: ZWaveJob(zwAPI)
{
	d = new Private();
	setType(ZWaveJob::REMOVE_NODE);
}

ZWJobRemoveNode::~ZWJobRemoveNode()
{
	delete d;
	d = NULL;
}

bool ZWJobRemoveNode::processData(const char* buffer, size_t length)
{
	SerialConnection::printDataBuffer(buffer, length, "ZWJobRemoveNode");

	if( ZWaveJob::RUNNING != state() )
	{
		g_pPlutoLogger->Write(LV_WARNING, "ZWJobRemoveNode: wrong job state.");
		return false;
	}
	
	if( d->currentJob != NULL )
	{
		if( !d->currentJob->processData(buffer, length) )
		{
			g_pPlutoLogger->Write(LV_WARNING, "ZWJobRemoveNode: .");
			return false;
		}
	}
	
	// check if the job has finished
	if( ZWaveJob::STOPPED == d->currentJob->state() )
	{
#ifdef PLUTO_DEBUG
			g_pPlutoLogger->Write(LV_WARNING, "----- REMOVE_NODE ---- 1");
#endif
		// next step
		switch( d->currentJob->type() )
		{
			case ZWaveJob::SET_LEARN_NODE_STATE :
			{
#ifdef PLUTO_DEBUG
			g_pPlutoLogger->Write(LV_WARNING, "----- REMOVE_NODE ---- 2");
#endif
				if( LEARN_NODE_STATE_DELETE == ((ZWJobSetLearnNodeState*)(d->currentJob))->mode() )
				{
#ifdef PLUTO_DEBUG
			g_pPlutoLogger->Write(LV_WARNING, "----- REMOVE_NODE ---- 3");
#endif
					char learnInfo[LEARN_INFO_MAX];
					size_t learnInfoLength = LEARN_INFO_MAX;
					if( !((ZWJobSetLearnNodeState*)(d->currentJob))->learnInfo(learnInfo, &learnInfoLength) &&
						learnInfoLength )
					{
#ifdef PLUTO_DEBUG
			g_pPlutoLogger->Write(LV_WARNING, "----- REMOVE_NODE ---- 4");
#endif
						// LEARN_INFO = Status Byte, NodeID Byte, Param Length Byte, Param[0]...Param[Length-1]
						// ZWJobSetLearnNodeState was running well
						// Just remove the node
						handler()->removeNode( learnInfo[1] );
						// Then set the learning off
						ZWJobSetLearnNodeState * learnOff = new ZWJobSetLearnNodeState(handler());
						if( learnOff != NULL )
						{
#ifdef PLUTO_DEBUG
			g_pPlutoLogger->Write(LV_WARNING, "----- REMOVE_NODE ---- 5");
#endif
							learnOff->setMode( LEARN_NODE_STATE_OFF );
							d->jobsQueue.push_back( learnOff );
						}
						else
						{
							g_pPlutoLogger->Write(LV_CRITICAL, "ZWJobRemoveNode allocation error!");
							
							delete learnOff;
							learnOff = NULL;
							
							return false;
						}
					}
					else
					{
						g_pPlutoLogger->Write(LV_CRITICAL, "ZWJobRemoveNode SET_LEARN_NODE_STATE FAILURE!");
					}
				}
				else
				{
#ifdef PLUTO_DEBUG
			g_pPlutoLogger->Write(LV_WARNING, "----- REMOVE_NODE ---- 6");
#endif
					// just nothing to do
				}
				break;
			}
				
			case ZWaveJob::GET_NODE_PROTOCOL_INFO :
				// just nothing to do
				break;
				
			default:
				g_pPlutoLogger->Write(LV_WARNING, "ZWJobRemoveNode: wrong job type.");
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
			setState(ZWaveJob::STOPPED);
			return true;
		}
	}
	
#ifdef PLUTO_DEBUG
			g_pPlutoLogger->Write(LV_WARNING, "----- REMOVE_NODE ---- 7");
#endif
	return true;
}

bool ZWJobRemoveNode::run()
{
	// start the initialization with ZWave API version
	d->currentJob = new ZWJobSetLearnNodeState(handler());
	if( d->currentJob == NULL )
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "ZWJobRemoveNode allocation error!");
		return false;
	}
	((ZWJobSetLearnNodeState*)d->currentJob)->setMode( LEARN_NODE_STATE_DELETE );
	
	setState( ZWaveJob::RUNNING );
	
	return d->currentJob->run();
}

