//
// C++ Interface: %{MODULE}
//
// Description: 
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef DCENETWORKIOPOOL_H
#define DCENETWORKIOPOOL_H

#include "IOPool.h"
#include "NetworkIOConnection.h"

namespace DCE {

/**
@author Igor Spac,,,
*/
class NetworkIOPool : public IOPool {
public:
    NetworkIOPool(bool usemain = false);
    virtual ~NetworkIOPool();

public:
	virtual IOConnection* getConnection();
	
protected:	
	/*callback from IOPool*/
	virtual bool handleIteration();

protected:
	class NetworkIOState : public IOPoolState {
		friend class NetworkIOPool;
	protected:
		NetworkIOState(NetworkIOPool* psm) 
			: IOPoolState(psm) {};
		virtual void handleOpen(IOConnection* pconn);
		virtual void handleRead(IOConnection* pconn);
		virtual void handleClose(IOConnection* pconn);
	} defstate_;
	
protected:
	NetworkIOConnection netconn;
};

};

#endif
