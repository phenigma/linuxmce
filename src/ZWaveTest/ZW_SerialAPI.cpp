#include ".\zw_serialapi.h"

#include "main.h"
#include "SerialConnection.h"
#include "ZWaveNode.h"
#include "ZWaveJob.h"

#include <deque>
#include <map>

using namespace std;

typedef map<int, ZWaveNode*, less<int> > NodesMap;
typedef NodesMap::iterator NodesMapIterator;
typedef NodesMap::const_iterator NodesMapCIterator;

typedef deque<ZWaveJob*> JobsDeque;
typedef JobsDeque::iterator JobsDequeIterator;
typedef JobsDeque::const_iterator JobsDequeCIterator;

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

	private:

		ZW_SerialAPI * parent_;
};

ZW_SerialAPI::Private::Private(ZW_SerialAPI * parent)
	: connection(NULL),
	  state(ZW_SerialAPI::STOPPED),
	  currentJob(NULL),
	  parent_(parent)
{
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
				const char * command = d->connection->getCommand();
				if( d->currentJob != NULL )
				{
					if( !d->currentJob->processData(command, strlen(command)) )
					{
						// TODO error
					}
				}
				
				// check if the job has finished
				if( ZWaveJob::STOPPED == d->currentJob->state() )
				{
					delete d->currentJob;
					d->currentJob = NULL;
					if( d->jobsQueue.size() )
					{
						
					}
				}
			}
			
			// sleep 10 ms
		}
	}
	
	return true;
}

bool ZW_SerialAPI::insertJob(ZWaveJob * job)
{
	return true;
}

