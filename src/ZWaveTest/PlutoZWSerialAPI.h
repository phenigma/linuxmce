#ifndef _ZW_SERIAL_API_H_
#define _ZW_SERIAL_API_H_

#include <deque>
#include <map>
#include <string>

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

		virtual bool start();
		
		virtual bool listen();
		
		virtual bool stop();
		
		virtual bool insertJob(ZWaveJob*);
		
		virtual bool insertNode(ZWaveNode*);
		
		virtual bool removeNode(unsigned short id);
		
		virtual ZWaveNode * getNode(unsigned short id);
		
		virtual const NodesMap& getNodes() const;
		
		virtual unsigned long homeID() const;
		
		virtual void setHomeID(unsigned long);
		
		virtual unsigned short nodeID() const;
		
		virtual void setNodeID(unsigned short);
		
		virtual bool sendData(char *buffer, size_t length);
		
		virtual bool processData(const char * buffer, size_t length);

		virtual std::string version() const;

		virtual void setVersion(std::string& version);

	private:

		PlutoZWSerialAPI();
		
		static PlutoZWSerialAPI * ref;

		class Private;
		Private * d;
};

#endif
