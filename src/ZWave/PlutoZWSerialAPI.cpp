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
		time_t timeLeft;
		
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
	  capabilities(0),
	  firmwareVersion(0),
	  timeLeft(0),
	  currentJob(NULL),
	  commandLength(0),
	  parent_(parent)
{
	memset(command, 0, sizeof(command));
	
	connection = SerialConnection::getInstance();
	if( connection == NULL )
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Not enough memory to allocate the serial connection object");
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
		g_pPlutoLogger->Write(LV_CRITICAL, "Not enough memory to allocate PlutoZWSerialAPI::Private");
	}
}

PlutoZWSerialAPI::~PlutoZWSerialAPI()
{
	delete d;
	d = NULL;
}

PlutoZWSerialAPI::SerialState PlutoZWSerialAPI::state() const
{
	return d->state;
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
				if( d->currentJob->run() )
				{
					return true;
				}
			}
		}
	}
	
	// it wasn't possible to start it, then just reset it
	stop();
	clearJobs();
	
	return false;
}

bool PlutoZWSerialAPI::stop()
{
	if( d->state != PlutoZWSerialAPI::STOPPED )
	{
#ifdef PLUTO_DEBUG
		g_pPlutoLogger->Write(LV_DEBUG, "-------- 3");
		g_pPlutoLogger->Flush();
#endif
		d->connection->disconnect();
		d->state = PlutoZWSerialAPI::STOPPED;
		d->timeLeft = 0;
#ifdef PLUTO_DEBUG
		g_pPlutoLogger->Write(LV_DEBUG, "-------- 4");
		g_pPlutoLogger->Flush();
#endif
	}
	
	return true;
}

bool PlutoZWSerialAPI::listen(time_t timeout)
{
	time_t listenTime = time(NULL);
	time_t currentTime = listenTime;
	time_t receivingTime = listenTime;
	d->timeLeft = timeout;
	
	if( PlutoZWSerialAPI::RUNNING == d->state )
	{
		d->state = PlutoZWSerialAPI::WAITTING;
		while( d->connection->isConnected() )
		{
			// overall jobs timeout
			currentTime = time(NULL);
			if( listenTime + timeout < currentTime )
			{
				g_pPlutoLogger->Write(LV_WARNING, "PlutoZWSerialAPI::listen Timeout");
				stop();
				return false;
			}
			d->timeLeft = timeout - (currentTime - listenTime);
			
			int commandRet = d->connection->hasCommand();
			if( commandRet == 1 )
			{
				d->commandLength = sizeof(d->command);
				memset(d->command, 0, d->commandLength);
				if( 0 == d->connection->receiveCommand(d->command, &d->commandLength) && d->commandLength > 0 )
				{
					// a new answer was got
					// update the receiving timer
					receivingTime = time(NULL);
					
					if( d->currentJob != NULL )
					{
						d->state = PlutoZWSerialAPI::RUNNING;
						if( !d->currentJob->processData(d->command, d->commandLength) )
						{
							g_pPlutoLogger->Write(LV_WARNING, "PlutoZWSerialAPI::listen : current job returns error");
							stop();
							return false;
						}
						d->state = PlutoZWSerialAPI::WAITTING;
					}
					else
					{
						g_pPlutoLogger->Write(LV_WARNING, "PlutoZWSerialAPI::listen : current job is null");
						stop();
						return false;
					}
					
					// check if the job has finished
					if( ZWaveJob::STOPPED == d->currentJob->state() )
					{
#ifdef PLUTO_DEBUG
						g_pPlutoLogger->Write(LV_DEBUG, "-------- 1");
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
								g_pPlutoLogger->Write(LV_WARNING, "PlutoZWSerialAPI::listen : current job couldn't run");
								stop();
								return false;
							}
							d->state = PlutoZWSerialAPI::WAITTING;
						}
						else
						{
#ifdef PLUTO_DEBUG
							g_pPlutoLogger->Write(LV_DEBUG, "-------- 2");
#endif
							stop();
#ifdef PLUTO_DEBUG
							g_pPlutoLogger->Write(LV_DEBUG, "-------- 2-1");
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
				g_pPlutoLogger->Write(LV_ZWAVE, " commandRet = %d ", commandRet);
#endif
				// still no answer, try to see if we have to check the receiving timeout
				if( d->currentJob != NULL && d->currentJob->hasReceivingTimeout() )
				{
#ifdef PLUTO_DEBUG
					g_pPlutoLogger->Write(LV_ZWAVE, "Check receiving timeout");
#endif
					if( receivingTime + d->currentJob->receivingTimeout() < currentTime )
					{
						d->currentJob->timeoutHandler();
						// job stopped, we should leave listen()
						if( ZWaveJob::STOPPED == d->currentJob->state() )
						{
							g_pPlutoLogger->Write(LV_WARNING, "PlutoZWSerialAPI::listen : current job got timeout");
							stop();
							return false;
						}
						// a new answer should get
						// update the receiving timer
						receivingTime = time(NULL);
					}
				}
			}
#ifdef PLUTO_DEBUG
			g_pPlutoLogger->Write(LV_ZWAVE, "PlutoZWSerialAPI::listen() sleeping");		
#endif
#ifdef _WIN32
	Sleep(READ_DELAY);
#else
	usleep(READ_DELAY);
#endif
		}
	}
#ifdef PLUTO_DEBUG
		g_pPlutoLogger->Write(LV_DEBUG, "-------- 6");
#endif
	// may be it's not needed
	d->timeLeft = 0;
	return true;
}

bool PlutoZWSerialAPI::insertJob(ZWaveJob * job)
{
	// TODO Insert INIT job as the first job if the state is STOPPED
	d->jobsQueue.push_back(job);
	return true;
}

bool PlutoZWSerialAPI::clearJobs()
{
	delete d->currentJob;
	d->currentJob = NULL;
	
	ZWaveJob * job = NULL;
	for(JobsDequeIterator it=d->jobsQueue.begin(); it!=d->jobsQueue.end(); ++it)
	{
		job = (*it);
		delete job;
		job = NULL;
	}
	
	d->jobsQueue.clear();
	
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
	SerialConnection::printDataBuffer( buffer, length, "Unknown command:");
	
	return true;
}

void PlutoZWSerialAPI::showZWaveNetwork() const
{
	ZWaveNode * node = NULL;
	for(NodesMapIterator itNode=d->nodes.begin(); itNode!=d->nodes.end(); ++itNode)
	{
		node = (*itNode).second;
		if( node != NULL )
		{
			g_pPlutoLogger->Write(LV_ZWAVE, "Node[%d] : BType = 0x%02x GType = 0x%02x SType = 0x%02x Level = %d",
				node->nodeID(),
				(unsigned)node->type().basic,
				(unsigned)node->type().generic,
				(unsigned)node->type().specific,
				node->level() );
		}
		else
		{
			g_pPlutoLogger->Write(LV_ZWAVE, "PlutoZWSerialAPI::showZWaveNetwork() : node null !");
		}
	}
}

time_t PlutoZWSerialAPI::timeLeft() const
{
#ifdef PLUTO_DEBUG
	g_pPlutoLogger->Write(LV_ZWAVE, "TimeLeft = %u", d->timeLeft);
#endif

	return d->timeLeft;
}

SerialConnection* PlutoZWSerialAPI::serialConnection()
{
	return d->connection;
}
