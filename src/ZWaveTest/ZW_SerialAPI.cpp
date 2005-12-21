#include ".\zw_serialapi.h"

#include "main.h"
#include "SerialConnection.h"

class ZW_SerialAPI::Private
{
	public:

		Private(ZW_SerialAPI*);

		~Private();

		SerialConnection * connection;

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

bool ZW_SerialAPI::insertJob()
{
	return true;
}

