#include ".\zw_serialapi.h"

#include "main.h"
#include "SerialConnection.h"

#include <string.h>

ZW_SerialAPI * ZW_SerialAPI::ref = NULL;

// -------------- Private -----------------------------------
class ZW_SerialAPI::Private
{
	public:

		Private(ZW_SerialAPI*);

		~Private();

		SerialConnection * connection;
		JobsDeque jobsQueue;
		NodesMap nodes;
		ZW_SerialAPI::SerialState state;
		
		ZWaveJob * currentJob;
		char command[65536];
		size_t commandLength;

	private:

		ZW_SerialAPI * parent_;
};

ZW_SerialAPI::Private::Private(ZW_SerialAPI * parent)
	: connection(NULL),
	  state(ZW_SerialAPI::STOPPED),
	  currentJob(NULL),
	  commandLength(0),
	  parent_(parent)
{
	memset(command, 0, sizeof(command));
	
	connection = SerialConnection::getInstance();
	if( connection == NULL )
	{
		g_pPlutoLogger->Write(LV_WARNING, "Not enough memory");
	}
}

ZW_SerialAPI::Private::~Private()
{
	delete connection;
	connection = NULL;
}

// ------------ ZW_SerialAPI -----------------------
ZW_SerialAPI * ZW_SerialAPI::instance()
{
	if(ref == NULL)
	{
		ref = new ZW_SerialAPI();
	}
	
	return ref;
}

ZW_SerialAPI::ZW_SerialAPI()
{
	d = new Private(this);
	if(d == NULL)
	{
		g_pPlutoLogger->Write(LV_WARNING, "Not enough memory");
	}
}

ZW_SerialAPI::~ZW_SerialAPI()
{
	delete d;
	d = NULL;
}

bool ZW_SerialAPI::start()
{
	if( d->state == ZW_SerialAPI::STOPPED )
	{
		if( d->jobsQueue.size() )
		{
			if( !d->connection->connect() )
			{
				d->currentJob = d->jobsQueue.front();
				d->jobsQueue.pop_front();
				d->state = ZW_SerialAPI::RUNNING;
				d->currentJob->run();
				
				return true;
			}
		}
	}
	
	return false;
}

bool ZW_SerialAPI::stop()
{
	if( d->state != ZW_SerialAPI::STOPPED )
	{
		d->connection->disconnect();
		d->state = ZW_SerialAPI::STOPPED;
	}
	
	return true;
}

bool ZW_SerialAPI::listen()
{
	if( d->state = ZW_SerialAPI::RUNNING )
	{
		d->state = ZW_SerialAPI::WAITTING;
		while( d->connection->isConnected() )
		{
			if( d->connection->hasCommand() )
			{
				d->commandLength = sizeof(d->command);
				memset(d->command, 0, d->commandLength);
				if( !d->connection->receiveCommand(d->command, &d->commandLength) && d->commandLength )
				{
					if( d->currentJob != NULL )
					{
						d->state = ZW_SerialAPI::RUNNING;
						if( !d->currentJob->processData(d->command, d->commandLength) )
						{
							// TODO error
						}
						d->state = ZW_SerialAPI::WAITTING;
					}
					
					// check if the job has finished
					if( ZWaveJob::STOPPED == d->currentJob->state() )
					{
						delete d->currentJob;
						d->currentJob = NULL;
						if( d->jobsQueue.size() )
						{
							d->currentJob = d->jobsQueue.front();
							d->jobsQueue.pop_front();
							d->state = ZW_SerialAPI::RUNNING;
							d->currentJob->run();
							d->state = ZW_SerialAPI::WAITTING;
						}
						else
						{
							stop();
						}
					}
				}
				else
				{
					// print the error
				}
			}
			
			// sleep 10 ms
		}
	}
	
	return true;
}

bool ZW_SerialAPI::insertJob(ZWaveJob * job)
{
	// TODO Insert INIT job as the first job if the state is STOPPED
	d->jobsQueue.push_back(job);
	return true;
}

bool ZW_SerialAPI::insertNode(ZWaveNode * node)
{
	if( d->nodes.end() == d->nodes.find( node->nodeID() ) )
	{
		d->nodes[ node->nodeID() ] = node;
		return true;
	}
	
	return false;
}

bool ZW_SerialAPI::removeNode(unsigned short id)
{
	NodesMapIterator it = d->nodes.find(id);
	if( it != d->nodes.end() )
	{
		d->nodes.erase(it);
	}
	return true;
}

ZWaveNode * ZW_SerialAPI::getNode(unsigned short id)
{
	NodesMapCIterator it = d->nodes.find(id);
	if( it != d->nodes.end() )
	{
		return (*it).second;
	}
	
	return NULL;
}

const NodesMap& ZW_SerialAPI::getNodes() const
{
	return d->nodes;
}

bool ZW_SerialAPI::processData(const char * buffer, size_t length)
{
	return true;
}
