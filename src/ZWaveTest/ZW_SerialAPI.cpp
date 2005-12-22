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

class ZW_SerialAPI::Private
{
	public:

		Private(ZW_SerialAPI*);

		~Private();

		SerialConnection * connection;
		JobsDeque jobsQueue;
		NodesMap nodes;

	private:

		ZW_SerialAPI * parent_;
};

ZW_SerialAPI::Private::Private(ZW_SerialAPI * parent)
	: connection(NULL),
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
}

bool ZW_SerialAPI::start()
{
	return true;
}

bool ZW_SerialAPI::stop()
{
	return true;
}
		
bool ZW_SerialAPI::insertJob(ZWaveJob * job)
{
	return true;
}

