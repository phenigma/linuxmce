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
 * MockUdpsocket.h
 * Header file for the Mock UDP Socket class
 * Copyright (C) 2010 Simon Newton
 */

#ifndef PLUGINS_ARTNET_MOCKUDPSOCKET_H_
#define PLUGINS_ARTNET_MOCKUDPSOCKET_H_

#include <cppunit/extensions/HelperMacros.h>
#include <string>
#include <queue>

#include "ola/network/Socket.h"

/*
 * A MockUdpSocket
 */
class MockUdpSocket: public ola::network::UdpSocketInterface {
  public:
    MockUdpSocket(): ola::network::UdpSocketInterface(),
                     m_init_called(false),
                     m_bound_to_port(false),
                     m_broadcast_set(false),
                     m_port(0),
                     m_buffer(NULL),
                     m_available(0) {}
    ~MockUdpSocket() { Close(); }

    // These are the socket methods
    bool Init();
    bool Bind(unsigned short port = INADDR_ANY);
    bool Close();
    int ReadDescriptor() const;
    int WriteDescriptor() const;
    ssize_t SendTo(const uint8_t *buffer,
                   unsigned int size,
                   const struct sockaddr_in &destination) const;
    ssize_t SendTo(const uint8_t *buffer,
                   unsigned int size,
                   const std::string &ip,
                   unsigned short port) const;
    bool RecvFrom(uint8_t *buffer,
                  ssize_t *data_read,
                  struct sockaddr_in &source,
                  socklen_t &src_size) const;
    bool RecvFrom(uint8_t *buffer, ssize_t *data_read) const;
    bool EnableBroadcast();
    bool SetMulticastInterface(const struct in_addr &interface);
    bool JoinMulticast(const struct in_addr &interface,
                       const struct in_addr &group,
                       bool loop = false);
    bool JoinMulticast(const struct in_addr &interface,
                       const std::string &address,
                       bool loop = false);
    bool LeaveMulticast(const struct in_addr &interface,
                        const struct in_addr &group);
    bool LeaveMulticast(const struct in_addr &interface,
                        const std::string &address);

    bool SetTos(uint8_t tos);


    // these are methods used for verification
    void NewData(uint8_t *buffer,
                 ssize_t *data_read,
                 struct sockaddr_in &source);

    void AddExpectedData(const uint8_t *data,
                         unsigned int size,
                         const struct sockaddr_in &destination);
    void Verify();

    bool CheckNetworkParamsMatch(bool init_called,
                                 bool bound_to_port,
                                 uint16_t port,
                                 bool broadcast_set);

    void SetInterface(const struct in_addr &interface);

  private:
    typedef struct {
      const uint8_t *data;
      unsigned int size;
      const struct sockaddr_in &destination;
    } expected_call;

    bool m_init_called;
    bool m_bound_to_port;
    bool m_broadcast_set;
    uint16_t m_port;
    uint8_t m_tos;
    mutable std::queue<expected_call> m_expected_calls;
    struct in_addr m_interface;

    mutable uint8_t *m_buffer;
    mutable ssize_t m_available;
    struct sockaddr_in m_source;
};


#endif  // PLUGINS_ARTNET_MOCKUDPSOCKET_H_
