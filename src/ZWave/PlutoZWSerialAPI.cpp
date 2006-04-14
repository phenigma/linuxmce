#include "PlutoZWSerialAPI.h"

#include "main.h"
#include "SerialConnection.h"

#include <string.h>

#ifdef _WIN32
#include <windows.h>
#include <winbase.h>
#define READ_DELAY  50
#define WRITE_DELAY 50
#define WAIT_DELAY  400
#else
#include <unistd.h>
#define READ_DELAY  50000
#define WRITE_DELAY 50000
#define WAIT_DELAY  400000
#endif

#include "PlutoUtils/MultiThreadIncludes.h" 

PlutoZWSerialAPI * PlutoZWSerialAPI::ref = NULL;

// -------------- Private -----------------------------------
class PlutoZWSerialAPI::Private
{
	public:

		static unsigned char count;
		
		Private(PlutoZWSerialAPI*);

		~Private();

		/** mutex for the asynchronous mode.*/
	    pluto_pthread_mutex_t asynchMutex;
		
		// mutex attribute
	    pthread_mutexattr_t mutexAttr;
		
		SerialConnection * connection;
		JobsDeque jobsQueue;
		NodesMap nodes;
		PlutoZWSerialAPI::SerialState state;
		PlutoZWSerialAPI::ListenType listenType;
		
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
		bool unknown;

	private:

		PlutoZWSerialAPI * parent_;
};

unsigned char PlutoZWSerialAPI::Private::count = 0;

PlutoZWSerialAPI::Private::Private(PlutoZWSerialAPI * parent)
	: asynchMutex("zwave_asynchronous"),
	  connection(NULL),
	  state(PlutoZWSerialAPI::STOPPED),
	  listenType(PlutoZWSerialAPI::ASYNCHRONOUS),
	  homeID(0L),
	  nodeID(0),
	  sucID(0),
	  capabilities(0),
	  firmwareVersion(0),
	  timeLeft(0),
	  currentJob(NULL),
	  commandLength(0),
	  unknown(false),
	  parent_(parent)
{
	pthread_mutexattr_init( &mutexAttr );
	pthread_mutexattr_settype( &mutexAttr, PTHREAD_MUTEX_RECURSIVE_NP );
	asynchMutex.Init( &mutexAttr );
	
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

unsigned char PlutoZWSerialAPI::callbackCount()
{
	ref->d->count++;
	if( ref->d->count >= 120 )
	{
		ref->d->count = 1;
	}
	
	return ref->d->count;
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

PlutoZWSerialAPI::ListenType PlutoZWSerialAPI::listenType() const
{
	return d->listenType;
}

bool PlutoZWSerialAPI::start(const char *port)
{
	PLUTO_SAFETY_LOCK(am, d->asynchMutex);
	
	if( d->state == PlutoZWSerialAPI::STOPPED )
	{
		if( d->jobsQueue.size() )
		{
			if( !d->connection->connect(port) )
			{
#ifdef PLUTO_DEBUG
		g_pPlutoLogger->Write(LV_DEBUG, "++++++++++++ ZWave is connected");
#endif
				d->state = PlutoZWSerialAPI::RUNNING;
				
				if( PlutoZWSerialAPI::SYNCHRONOUS == d->listenType )
				{
					d->currentJob = d->jobsQueue.front();
					d->jobsQueue.pop_front();
					if( d->currentJob != NULL && d->currentJob->run() )
					{
						return true;
					}
				}
			}
		}
	}
	
	if( d->state != PlutoZWSerialAPI::STOPPED && PlutoZWSerialAPI::ASYNCHRONOUS == d->listenType )
	{
		return true;
	}
	
	g_pPlutoLogger->Write(LV_WARNING, "PlutoZWSerialAPI::start : connection problems");
	
	// it wasn't possible to start it, then just reset it
	stop();
	clearJobs();
	
	return false;
}

bool PlutoZWSerialAPI::stop()
{
	PLUTO_SAFETY_LOCK(am, d->asynchMutex);
	
	if( d->state != PlutoZWSerialAPI::STOPPED )
	{
#ifdef PLUTO_DEBUG
		g_pPlutoLogger->Write(LV_DEBUG, "-------- 3");
#endif
		d->connection->disconnect();
		d->state = PlutoZWSerialAPI::STOPPED;
		d->timeLeft = 0;
#ifdef PLUTO_DEBUG
		g_pPlutoLogger->Write(LV_DEBUG, "-------- 4");
#endif
	}
	
	return true;
}

bool PlutoZWSerialAPI::listen(time_t timeout)
{
	// for asynchronous mode, please use listenAsynchronous
	if( PlutoZWSerialAPI::ASYNCHRONOUS == d->listenType )
	{
		return true;
	}
	
	d->listenType = PlutoZWSerialAPI::SYNCHRONOUS;
	
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

bool PlutoZWSerialAPI::listenAsynchronous()
{
	d->listenType = PlutoZWSerialAPI::ASYNCHRONOUS;

	if( PlutoZWSerialAPI::RUNNING == d->state )
	{
		// important, if there isn't any 'ack' set in the last time (try few times)
		// then we have to keep on, so we set 'ack'
		int iTry = 0;
		// important, this is asynchronous BUT secvential (the tasks will run one after each other)
		bool secvential = true;
		
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
					PLUTO_SAFETY_LOCK(am, d->asynchMutex);
					d->state = PlutoZWSerialAPI::RUNNING;
					ZWaveJob * job = NULL;
					for(JobsDequeIterator it=d->jobsQueue.begin(); it!=d->jobsQueue.end(); ++it)
					{
						job = (*it);
						if( job != NULL && ZWaveJob::STOPPED != job->state() )
						{
							d->unknown = false;
							if( job->processData(d->command, d->commandLength) && !d->unknown )
							{
								if( ZWaveJob::STOPPED == job->state() )
								{
									// the current job is finished, let's run the next one
									secvential = true;
									
									delete job;
									d->jobsQueue.erase(it);
#ifdef PLUTO_DEBUG
			g_pPlutoLogger->Write(LV_ZWAVE, "## Current Job Finished --- Left = %u", d->jobsQueue.size());
#endif
								}
								// if the answer is for this job, than that's all to process
								break;
							}
						}
					}
					d->state = PlutoZWSerialAPI::WAITTING;
				}
				else
				{
					// print the error
					g_pPlutoLogger->Write(LV_WARNING, "ZWave Command Error");
				
					stop();
					return false;
				}
			}
			else
			{
				iTry++;
			}
			
			// check the timeout, idle and closed jobs
			if( d->connection->ack )
			{
				iTry = 0;
				PLUTO_SAFETY_LOCK(am, d->asynchMutex);
				time_t currentTime = time(NULL);
				ZWaveJob * job = NULL;
				JobsDequeIterator it=d->jobsQueue.begin();
				while( it!=d->jobsQueue.end() )
				{
					job = (*it);
					
					if( job != NULL )
					{
						if( ZWaveJob::RUNNING == job->state() )
						{
							if( currentTime - job->startTime() > job->runningTimeout() )
							{
								job->setState( ZWaveJob::STOPPED );
							}
							else if( job->hasReceivingTimeout() && 
									 currentTime - job->answerTime() > job->receivingTimeout() )
							{
#ifdef PLUTO_DEBUG
								g_pPlutoLogger->Write(LV_WARNING, "### Retry");
#endif
								job->timeoutHandler();
								break;
							}
						}
						// if IDLE , just run it
						else if( ZWaveJob::IDLE == job->state() )
						{
							if( secvential )
							{
								d->connection->clearSerialBuffer();
								secvential = false;
#ifdef PLUTO_DEBUG
								g_pPlutoLogger->Write(LV_ZWAVE, "### Run Current Job ");
#endif
								if( !job->run() )
								{
#ifdef PLUTO_DEBUG
								g_pPlutoLogger->Write(LV_ZWAVE, "### Run Current Job - not possible");
#endif
									// the current job is finished, let's run the next one
									secvential = true;
									
									delete job;
									it = d->jobsQueue.erase(it);
								}
								
								break;
							}
						}
					}
					
					// remove the closed jobs
					if( job == NULL || ZWaveJob::STOPPED == job->state() )
					{
						g_pPlutoLogger->Write(LV_WARNING, "-------------------------- Job Timeout _________");
						
						// the current job is finished, let's run the next one
						secvential = true;
						
						delete job;
						it = d->jobsQueue.erase(it);
					}
					else
					{
						++it;
					}
				}
			}
			else
			{
				if( iTry > 3 )
				{
					// let's set it since we didn't get any ack in the last time
					d->connection->ack = true;
				}
			}
#ifdef _WIN32
			Sleep(READ_DELAY);
#else
			usleep(READ_DELAY);
#endif
		}
	}
	else
	{
#ifdef _WIN32
		Sleep(WAIT_DELAY);
#else
		usleep(WAIT_DELAY);
#endif
	}
	
	return true;
}

bool PlutoZWSerialAPI::waitForJob(ZWaveJob * job, time_t timeout)
{
	time_t waittingTime = time(NULL);
	time_t currentTime = waittingTime;
	bool found = false;
	while( currentTime - waittingTime <= timeout )
	{
		{
			PLUTO_SAFETY_LOCK(am, d->asynchMutex);
			
			found = false;
			if( job == NULL )
			{
				if( !d->jobsQueue.empty() )
				{
					found = true;
				}
			}
			else
			{
				for(JobsDequeIterator it=d->jobsQueue.begin(); it!=d->jobsQueue.end(); ++it)
				{
					if( job == (*it) )
					{
						found = true;
						break;
					}
				}
			}
		}
		
		// if it's not in queue then it's finished
		if( !found )
		{
			return true;
		}
		
#ifdef _WIN32
		Sleep(READ_DELAY);
#else
		usleep(READ_DELAY);
#endif
		currentTime = time(NULL);
	}
	
	return false;
}

bool PlutoZWSerialAPI::insertJob(ZWaveJob * job)
{
	PLUTO_SAFETY_LOCK(am, d->asynchMutex);
	
// may be, we should use it
//	if( job == NULL )
//		return false;
	
	// TODO Insert INIT job as the first job if the state is STOPPED
	d->jobsQueue.push_back(job);
	return true;
}

bool PlutoZWSerialAPI::clearJobs()
{
	PLUTO_SAFETY_LOCK(am, d->asynchMutex);
	
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
	if( node != NULL && d->nodes.end() == d->nodes.find( node->nodeID() ) )
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
#ifdef _WIN32
			Sleep(WRITE_DELAY);
#else
			usleep(WRITE_DELAY);
#endif

	if( !d->connection->send(buffer, length) )
	{
		return true;
	}
	
	// TODO: error
	
	return false;
}

bool PlutoZWSerialAPI::processData(const char * buffer, size_t length)
{
	if( d->listenType == PlutoZWSerialAPI::SYNCHRONOUS )
	{
		SerialConnection::printDataBuffer( buffer, length, "Unknown command:");
	}
	
	d->unknown = true;
	
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

