/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * SelectServerInterface.h
 * An interface to a SelectServer that enforces clean separation.
 * Copyright (C) 2010 Simon Newton
 */

#ifndef INCLUDE_OLA_NETWORK_SELECTSERVERINTERFACE_H_
#define INCLUDE_OLA_NETWORK_SELECTSERVERINTERFACE_H_

#include <ola/Clock.h>  // NOLINT
#include <ola/Callback.h>  // NOLINT

namespace ola {
namespace network {


class Event;
typedef Event* timeout_id;

static const timeout_id INVALID_TIMEOUT = 0;


class SelectServerInterface {
  public :
    SelectServerInterface() {}
    virtual ~SelectServerInterface() {}

    virtual bool AddSocket(class Socket *socket) = 0;
    virtual bool AddSocket(class ConnectedSocket *socket,
                   bool delete_on_close = false) = 0;
    virtual bool RemoveSocket(class Socket *socket) = 0;
    virtual bool RemoveSocket(class ConnectedSocket *socket) = 0;
    virtual timeout_id RegisterRepeatingTimeout(
        unsigned int ms,
        Callback0<bool> *closure) = 0;
    virtual timeout_id RegisterSingleTimeout(
        unsigned int ms,
        SingleUseCallback0<void> *closure) = 0;
    virtual void RemoveTimeout(timeout_id id) = 0;

    virtual const TimeStamp *WakeUpTime() const = 0;
};
}  // network
}  // ola
#endif  // INCLUDE_OLA_NETWORK_SELECTSERVERINTERFACE_H_
