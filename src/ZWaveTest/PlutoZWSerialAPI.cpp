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
		unsigned char capabilities;
		unsigned char firmwareVersion;
		
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
	delete currentJob;
	currentJob = NULL;
	
	ZWaveJob * job = NULL;
	for(JobsDequeIterator it=jobsQueue.begin(); it!=jobsQueue.end(); ++it)
	{
		job = (*it);
		delete job;
		job = NULL;
	}
	
	ZWaveNode * node = NULL;
	for(NodesMapIterator itNode=nodes.begin(); itNode!=nodes.end(); ++itNode)
	{
		node = (*itNode).second;
		delete node;
		node = NULL;
	}
	
	SerialConnection::forceClose();
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

bool PlutoZWSerialAPI::start(const char *port)
{
	if( d->state == PlutoZWSerialAPI::STOPPED )
	{
		if( d->jobsQueue.size() )
		{
			if( !d->connection->connect(port) )
			{
				d->currentJob = d->jobsQueue.front();
				d->jobsQueue.pop_front();
				d->state = PlutoZWSerialAPI::RUNNING;
				return d->currentJob->run();
			}
		}
	}
	
	return false;
}

bool PlutoZWSerialAPI::stop()
{
	if( d->state != PlutoZWSerialAPI::STOPPED )
	{
#ifdef PLUTO_DEBUG
		g_pPlutoLogger->Write(LV_WARNING, "-------- 3");
		g_pPlutoLogger->Flush();
#endif
		d->connection->disconnect();
		d->state = PlutoZWSerialAPI::STOPPED;
#ifdef PLUTO_DEBUG
		g_pPlutoLogger->Write(LV_WARNING, "-------- 4");
		g_pPlutoLogger->Flush();
#endif
	}
	
	return true;
}

bool PlutoZWSerialAPI::listen()
{
	if( PlutoZWSerialAPI::RUNNING == d->state )
	{
		d->state = PlutoZWSerialAPI::WAITTING;
		while( d->connection->isConnected() )
		{
			int commandRet = d->connection->hasCommand();
			if( commandRet == 1 )
			{
				d->commandLength = sizeof(d->command);
				memset(d->command, 0, d->commandLength);
				if( 0 == d->connection->receiveCommand(d->command, &d->commandLength) && d->commandLength > 0 )
				{
					if( d->currentJob != NULL )
					{
						d->state = PlutoZWSerialAPI::RUNNING;
						if( !d->currentJob->processData(d->command, d->commandLength) )
						{
							// TODO error
							stop();
							return false;
						}
						d->state = PlutoZWSerialAPI::WAITTING;
					}
					
					// check if the job has finished
					if( ZWaveJob::STOPPED == d->currentJob->state() )
					{
#ifdef PLUTO_DEBUG
						g_pPlutoLogger->Write(LV_WARNING, "-------- 1");
#endif
						delete d->currentJob;
						d->currentJob = NULL;
						if( d->jobsQueue.size() )
						{
							d->currentJob = d->jobsQueue.front();
							d->jobsQueue.pop_front();
							d->state = PlutoZWSerialAPI::RUNNING;
							if( !d->currentJob->run() )
							{
								// TODO error
								stop();
								return false;
							}
							d->state = PlutoZWSerialAPI::WAITTING;
						}
						else
						{
#ifdef PLUTO_DEBUG
							g_pPlutoLogger->Write(LV_WARNING, "-------- 2");
#endif
							stop();
#ifdef PLUTO_DEBUG
							g_pPlutoLogger->Write(LV_WARNING, "-------- 2-1");
#endif
						}
					}
				}
				else
				{
					// print the error
					stop();
					return false;
				}
			}
			else
			{
#ifdef PLUTO_DEBUG
				g_pPlutoLogger->Write(LV_DEBUG, " commandRet = %d ", commandRet);
#endif
			}
#ifdef PLUTO_DEBUG
			g_pPlutoLogger->Write(LV_DEBUG, "PlutoZWSerialAPI::listen() sleeping");		
#endif
#ifdef _WIN32
	Sleep(READ_DELAY);
#else
	usleep(READ_DELAY);
#endif
		}
	}
#ifdef PLUTO_DEBUG	
		g_pPlutoLogger->Write(LV_WARNING, "-------- 6");
#endif
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

void PlutoZWSerialAPI::clearNodes()
{
	ZWaveNode * node = NULL;
	for(NodesMapIterator itNode=d->nodes.begin(); itNode!=d->nodes.end(); ++itNode)
	{
		node = (*itNode).second;
		delete node;
		node = NULL;
	}
	d->nodes.clear();
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

unsigned char PlutoZWSerialAPI::firmwareVersion() const
{
	return d->firmwareVersion;
}

void PlutoZWSerialAPI::setFirmwareVersion(unsigned char firmwareVersion)
{
	d->firmwareVersion = firmwareVersion;
}

unsigned char PlutoZWSerialAPI::capabilities() const
{
	return d->capabilities;
}

void PlutoZWSerialAPI::setCapabilities(unsigned char capabilities)
{
	d->capabilities = capabilities; 
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
	return false;
}
