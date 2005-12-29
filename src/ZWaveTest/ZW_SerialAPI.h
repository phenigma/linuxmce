#ifndef _ZW_SERIAL_API_H_
#define _ZW_SERIAL_API_H_

#include <deque>
#include <map>

#include "ZWaveNode.h"
#include "ZWaveJob.h"

using namespace std;
typedef map<int, ZWaveNode*, less<int> > NodesMap;
typedef NodesMap::iterator NodesMapIterator;
typedef NodesMap::const_iterator NodesMapCIterator;

typedef deque<ZWaveJob*> JobsDeque;
typedef JobsDeque::iterator JobsDequeIterator;
typedef JobsDeque::const_iterator JobsDequeCIterator;

class ZW_SerialAPI
{
	public:

		enum SerialState { STOPPED, IDLE, RUNNING, WAITTING };
		
		static ZW_SerialAPI * instance();

		virtual ~ZW_SerialAPI();

		virtual bool start();
		
		virtual bool listen();
		
		virtual bool stop();
		
		virtual bool insertJob(ZWaveJob*);
		
		virtual bool insertNode(ZWaveNode*);
		
		virtual bool removeNode(unsigned short id);
		
		virtual ZWaveNode * getNode(unsigned short id);
		
		virtual const NodesMap& getNodes() const;
		
		virtual bool processData(const char * buffer, size_t length);

	private:

		ZW_SerialAPI();
		
		static ZW_SerialAPI * ref;

		class Private;
		Private * d;
};

#endif
