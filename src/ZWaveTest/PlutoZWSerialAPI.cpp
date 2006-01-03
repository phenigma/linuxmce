#include "PlutoZWSerialAPI.h"

#include "main.h"
#include "SerialConnection.h"

#include <string.h>

// thread sleep for 200 msec
#ifdef _WIN32
#include <windows.h>
#include <winbase.h>
#define READ_DELAY 200
#else
#include <unistd.h>
#define READ_DELAY 200000
#endif


PlutoZWSerialAPI * PlutoZWSerialAPI::ref = NULL;

// -------------- Private -----------------------------------
class PlutoZWSerialAPI::Private
{
	public:

		Private(PlutoZWSerialAPI*);

		~Private();

		SerialConnection * connection;
		JobsDeque jobsQueue;
		NodesMap nodes;
		PlutoZWSerialAPI::SerialState state;
		unsigned long homeID;
		unsigned short nodeID;
		unsigned short sucID;
		std::string version;
		
		ZWaveJob * currentJob;
		char command[65536];
		size_t commandLength;

	private:

		PlutoZWSerialAPI * parent_;
};

PlutoZWSerialAPI::Private::Private(PlutoZWSerialAPI * parent)
	: connection(NULL),
	  state(PlutoZWSerialAPI::STOPPED),
	  homeID(0L),
	  nodeID(0),
	  sucID(0),
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

PlutoZWSerialAPI::Private::~Private()
{
	delete connection;
	connection = NULL;
}

// ------------ PlutoZWSerialAPI -----------------------
PlutoZWSerialAPI * PlutoZWSerialAPI::instance()
{
	if(ref == NULL)
	{
		ref = new PlutoZWSerialAPI();
	}
	
	return ref;
}

PlutoZWSerialAPI::PlutoZWSerialAPI()
{
	d = new Private(this);
	if(d == NULL)
	{
		g_pPlutoLogger->Write(LV_WARNING, "Not enough memory");
	}
}

PlutoZWSerialAPI::~PlutoZWSerialAPI()
{
	delete d;
	d = NULL;
}

bool PlutoZWSerialAPI::start()
{
	if( d->state == PlutoZWSerialAPI::STOPPED )
	{
		if( d->jobsQueue.size() )
		{
			if( !d->connection->connect() )
			{
				d->currentJob = d->jobsQueue.front();
				d->jobsQueue.pop_front();
				d->state = PlutoZWSerialAPI::RUNNING;
				d->currentJob->run();
				
				return true;
			}
		}
	}
	
	return false;
}

bool PlutoZWSerialAPI::stop()
{
	if( d->state != PlutoZWSerialAPI::STOPPED )
	{
		d->connection->disconnect();
		d->state = PlutoZWSerialAPI::STOPPED;
	}
	
	return true;
}

bool PlutoZWSerialAPI::listen()
{
	if( d->state = PlutoZWSerialAPI::RUNNING )
	{
		d->state = PlutoZWSerialAPI::WAITTING;
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
						d->state = PlutoZWSerialAPI::RUNNING;
						if( !d->currentJob->processData(d->command, d->commandLength) )
						{
							// TODO error
						}
						d->state = PlutoZWSerialAPI::WAITTING;
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
							d->state = PlutoZWSerialAPI::RUNNING;
							d->currentJob->run();
							d->state = PlutoZWSerialAPI::WAITTING;
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
			
#ifdef _WIN32
	Sleep(READ_DELAY);
#else
	usleep(READ_DELAY);
#endif
		}
	}
	
	return true;
}

bool PlutoZWSerialAPI::insertJob(ZWaveJob * job)
{
	// TODO Insert INIT job as the first job if the state is STOPPED
	d->jobsQueue.push_back(job);
	return true;
}

bool PlutoZWSerialAPI::insertNode(ZWaveNode * node)
{
	if( d->nodes.end() == d->nodes.find( node->nodeID() ) )
	{
		d->nodes[ node->nodeID() ] = node;
		return true;
	}
	
	return false;
}

bool PlutoZWSerialAPI::removeNode(unsigned short id)
{
	NodesMapIterator it = d->nodes.find(id);
	if( it != d->nodes.end() )
	{
		d->nodes.erase(it);
	}
	return true;
}

ZWaveNode * PlutoZWSerialAPI::getNode(unsigned short id)
{
	NodesMapCIterator it = d->nodes.find(id);
	if( it != d->nodes.end() )
	{
		return (*it).second;
	}
	
	return NULL;
}

const NodesMap& PlutoZWSerialAPI::getNodes() const
{
	return d->nodes;
}

unsigned long PlutoZWSerialAPI::homeID() const
{
	return d->homeID;
}

void PlutoZWSerialAPI::setHomeID(unsigned long id)
{
	d->homeID = id;
}

std::string PlutoZWSerialAPI::version() const
{
	return d->version;
}

void PlutoZWSerialAPI::setVersion(std::string& version)
{
	d->version = version;
}

unsigned short PlutoZWSerialAPI::nodeID() const
{
	return d->nodeID;
}

void PlutoZWSerialAPI::setNodeID(unsigned short id)
{
	d->nodeID = id;
}

unsigned short PlutoZWSerialAPI::sucID() const
{
	return d->sucID;
}

void PlutoZWSerialAPI::setSucID(unsigned short id)
{
	d->sucID = id;
}

bool PlutoZWSerialAPI::sendData(char *buffer, size_t length)
{
	if( !d->connection->send(buffer, length) )
	{
		return true;
	}
	
	// TODO: error
	
	return false;
}

bool PlutoZWSerialAPI::processData(const char * buffer, size_t length)
{
	return true;
}
