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
 * DmxTriDevice.h
 * The Jese DMX TRI device. This just wraps the DmxTriWidget.
 * Copyright (C) 2010 Simon Newton
 */

#include <string>
#include "ola/Callback.h"
#include "ola/Logging.h"
#include "ola/network/SelectServerInterface.h"
#include "plugins/usbpro/DmxTriDevice.h"
#include "plugins/usbpro/DmxTriWidget.h"

namespace ola {
namespace plugin {
namespace usbpro {

using std::string;


/*
 * New DMX TRI device
 */
DmxTriDevice::DmxTriDevice(ola::network::SelectServerInterface *ss,
                           ola::AbstractPlugin *owner,
                           const string &name,
                           UsbWidget *widget,
                           uint16_t esta_id,
                           uint16_t device_id,
                           uint32_t serial,
                           bool use_raw_rdm):
    UsbDevice(owner, name, widget),
    m_tri_widget(NULL) {
  std::stringstream str;
  str << std::hex << esta_id << "-" << device_id << "-" << serial;
  m_device_id = str.str();

  m_tri_widget = new DmxTriWidget(ss, widget, 20, use_raw_rdm);

  ola::BasicOutputPort *output_port = new DmxTriOutputPort(this);
  AddPort(output_port);

  m_tri_widget->SetUIDListCallback(
      ola::NewCallback(output_port, &DmxTriOutputPort::NewUIDList));
}


/*
 * Shutdown
 */
DmxTriDevice::~DmxTriDevice() {
  delete m_tri_widget;
}


/*
 * Kick off the RDM discovery process
 */
bool DmxTriDevice::StartHook() {
  m_tri_widget->RunRDMDiscovery();
  return true;
}


void DmxTriDevice::SendUIDUpdate() {
  m_tri_widget->SendUIDUpdate();
}


/*
 * Remove the rdm timeout if it's still running
 */
void DmxTriDevice::PrePortStop() {
  m_tri_widget->Stop();
}


/*
 * Send a dmx msg
 * @returns true if we sent ok, false otherwise
 */
bool DmxTriDevice::SendDMX(const DmxBuffer &buffer) const {
  return m_tri_widget->SendDMX(buffer);
}


/*
 * Handle an RDM Request, ownership of the request object is transferred to us.
 */
void DmxTriDevice::HandleRDMRequest(const ola::rdm::RDMRequest *request,
                                    ola::rdm::RDMCallback *callback) {
  m_tri_widget->SendRDMRequest(request, callback);
}


/*
 * Kick off the discovery process if it's not already running
 */
void DmxTriDevice::RunRDMDiscovery() {
  m_tri_widget->RunRDMDiscovery();
}
}  // usbpro
}  // plugin
}  // ola
