#ifndef _ZW_SERIAL_API_H_
#define _ZW_SERIAL_API_H_

#include <deque>
#include <map>
#include <string>

#include <time.h>

#include "ZWaveNode.h"
#include "ZWaveJob.h"

using namespace std;
typedef map<int, ZWaveNode*, less<int> > NodesMap;
typedef NodesMap::iterator NodesMapIterator;
typedef NodesMap::const_iterator NodesMapCIterator;

typedef deque<ZWaveJob*> JobsDeque;
typedef JobsDeque::iterator JobsDequeIterator;
typedef JobsDeque::const_iterator JobsDequeCIterator;

class PlutoZWSerialAPI
{
	public:

		enum SerialState { STOPPED, IDLE, RUNNING, WAITTING };
		
		static PlutoZWSerialAPI * instance();

		virtual ~PlutoZWSerialAPI();

#ifndef _WIN32
		virtual bool start(const char *port);
#else
		virtual bool start(const char *port);
#endif
		
		virtual bool listen(time_t timeout);
		
		virtual bool stop();
		
		virtual SerialState state() const;
		
		virtual bool insertJob(ZWaveJob*);
		
		virtual bool clearJobs();
		
		virtual bool insertNode(ZWaveNode*);
		
		virtual bool removeNode(unsigned short id);
		
		virtual ZWaveNode * getNode(unsigned short id);
		
		virtual const NodesMap& getNodes() const;
		
		virtual void clearNodes();
		
		virtual unsigned long homeID() const;
		
		virtual void setHomeID(unsigned long);
		
		virtual unsigned short nodeID() const;
		
		virtual void setNodeID(unsigned short);
		
		virtual unsigned short sucID() const;
		
		virtual void setSucID(unsigned short);
		
		virtual bool sendData(char *buffer, size_t length);
		
		virtual bool processData(const char * buffer, size_t length);

		virtual std::string version() const;

		virtual void setVersion(std::string& version);

		virtual unsigned char capabilities() const;

		virtual void setCapabilities(unsigned char capabilities);

		virtual unsigned char firmwareVersion() const;

		virtual void setFirmwareVersion(unsigned char firmwareVersion);
		
		/** Print some debug information about each node from the ZWave network.*/
		virtual void showZWaveNetwork() const;
		
		/** The time left until it gets timeout.*/
		virtual time_t timeLeft() const;

		private:

		PlutoZWSerialAPI();
		
		static PlutoZWSerialAPI * ref;

		class Private;
		Private * d;
};

#endif
