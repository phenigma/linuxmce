/***************************************************************************
 *   Copyright (C) 2005 by Igor Spac,,,                                    *
 *   igor@dexx                                                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef DCEIOPOOL_H
#define DCEIOPOOL_H

#include "LoopStateMachine.h"
#include "IOConnection.h"

#include <list>

namespace DCE {

/**
@author Igor Spac,,,
*/
class IOPool : public LoopStateMachine {
public:
    IOPool(bool usemain = false);
    ~IOPool();

protected:
	virtual IOConnection* getConnection() = 0;
	
	/*pool handling*/
protected:
	virtual bool handleIteration();

	/*states*/
protected:
	class IOPoolState : public LoopState {
		friend class IOPool;
	protected:
		virtual void handleOpen(IOConnection* pconn)
		{};
		virtual void handleRead(IOConnection* pconn)
		{};
		virtual void handleClose(IOConnection* pconn)
		{};
	};

private:	
};

};

#endif
