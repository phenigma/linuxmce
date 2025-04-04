/*
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * ArtNetPackets.h
 * Datagram definitions for ArtNet
 * Copyright (C) 2004-2010 Simon Newton
 */

#ifndef PLUGINS_ARTNET_ARTNETPACKETS_H_
#define PLUGINS_ARTNET_ARTNETPACKETS_H_

#include "ola/BaseTypes.h"
#include "ola/rdm/UID.h"
#include "ola/network/InterfacePicker.h"

namespace ola {
namespace plugin {
namespace artnet {

// various length enums
enum { ARTNET_LONG_NAME_LENGTH = 64 };
enum { ARTNET_MAX_PORTS = 4 };
enum { ARTNET_MAX_RDM_ADDRESS_COUNT = 32 };
// According to the rdm spec, this should be 256 bytes
// We'll set to 512 here just to be safe
enum { ARTNET_MAX_RDM_DATA = 512 };
enum { ARTNET_MAX_UID_COUNT = 200 };
enum { ARTNET_REPORT_LENGTH = 64 };
enum { ARTNET_SHORT_NAME_LENGTH = 18 };


/*
 * The subset of packets we're interested in
 */
typedef enum artnet_packet_type_e {
  ARTNET_POLL = 0x2000,
  ARTNET_REPLY = 0x2100,
  ARTNET_DMX = 0x5000,
  ARTNET_TODREQUEST = 0x8000,
  ARTNET_TODDATA = 0x8100,
  ARTNET_TODCONTROL = 0x8200,
  ARTNET_RDM = 0x8300,
  ARTNET_RDM_SUB = 0x8400,
  ARTNET_IP_PROGRAM = 0xf800,
  ARTNET_IP_PROGRAM_REPLY = 0xf900,
} artnet_packet_type;


struct artnet_poll_s {
  uint16_t version;
  uint8_t talk_to_me;
  uint8_t priority;
} __attribute__((packed));

typedef struct artnet_poll_s artnet_poll_t;

struct artnet_reply_s {
  uint8_t  ip[ola::network::IPV4_LENGTH];
  uint16_t port;
  uint16_t version;
  uint8_t  subnet_address[2];
  uint16_t oem;
  uint8_t  ubea;
  uint8_t  status1;
  uint16_t esta_id;
  char  short_name[ARTNET_SHORT_NAME_LENGTH];
  char  long_name[ARTNET_LONG_NAME_LENGTH];
  char  node_report[ARTNET_REPORT_LENGTH];
  uint8_t  number_ports[2];
  uint8_t  port_types[ARTNET_MAX_PORTS];
  uint8_t  good_input[ARTNET_MAX_PORTS];
  uint8_t  good_output[ARTNET_MAX_PORTS];
  uint8_t  sw_in[ARTNET_MAX_PORTS];
  uint8_t  sw_out[ARTNET_MAX_PORTS];
  uint8_t  sw_video;
  uint8_t  sw_macro;
  uint8_t  sw_remote;
  uint8_t  spare1;
  uint8_t  spare2;
  uint8_t  spare3;
  uint8_t  style;
  uint8_t  mac[ola::network::MAC_LENGTH];
  uint8_t  bind_ip[ola::network::IPV4_LENGTH];
  uint8_t  bind_index;
  uint8_t  status2;
  uint8_t  filler[26];
} __attribute__((packed));

typedef struct artnet_reply_s artnet_reply_t;


struct artnet_dmx_s {
  uint16_t version;
  uint8_t  sequence;
  uint8_t  physical;
  uint16_t universe;
  uint8_t  length[2];
  uint8_t  data[DMX_UNIVERSE_SIZE];
} __attribute__((packed));

typedef struct artnet_dmx_s artnet_dmx_t;


struct artnet_todrequest_s {
  uint16_t version;
  uint8_t  filler1;
  uint8_t  filler2;
  uint8_t  spare1;
  uint8_t  spare2;
  uint8_t  spare3;
  uint8_t  spare4;
  uint8_t  spare5;
  uint8_t  spare6;
  uint8_t  spare7;
  uint8_t  spare8;
  uint8_t  command;
  uint8_t  address_count;
  uint8_t  addresses[ARTNET_MAX_RDM_ADDRESS_COUNT];
} __attribute__((packed));

typedef struct artnet_todrequest_s artnet_todrequest_t;


struct artnet_toddata_s {
  uint16_t version;
  uint8_t  rdm_version;
  uint8_t  port;
  uint8_t  spare1;
  uint8_t  spare2;
  uint8_t  spare3;
  uint8_t  spare4;
  uint8_t  spare5;
  uint8_t  spare6;
  uint8_t  spare7;
  uint8_t  spare8;
  uint8_t  command_response;
  uint8_t  address;
  uint16_t  uid_total;
  uint8_t  block_count;
  uint8_t  uid_count;
  uint8_t  tod[ARTNET_MAX_UID_COUNT][ola::rdm::UID::UID_SIZE];
} __attribute__((packed));

typedef struct artnet_toddata_s artnet_toddata_t;

struct artnet_todcontrol_s {
  uint16_t version;
  uint8_t  filler1;
  uint8_t  filler2;
  uint8_t  spare1;
  uint8_t  spare2;
  uint8_t  spare3;
  uint8_t  spare4;
  uint8_t  spare5;
  uint8_t  spare6;
  uint8_t  spare7;
  uint8_t  spare8;
  uint8_t  command;
  uint8_t  address;
} __attribute__((packed));

typedef struct artnet_todcontrol_s artnet_todcontrol_t;

struct artnet_rdm_s {
  uint16_t version;
  uint8_t rdm_version;
  uint8_t filler2;
  uint8_t spare1;
  uint8_t spare2;
  uint8_t spare3;
  uint8_t spare4;
  uint8_t spare5;
  uint8_t spare6;
  uint8_t spare7;
  uint8_t spare8;
  uint8_t command;
  uint8_t address;
  uint8_t data[ARTNET_MAX_RDM_DATA];
} __attribute__((packed));

typedef struct artnet_rdm_s artnet_rdm_t;

struct artnet_ip_prog_s {
  uint16_t version;
  uint16_t filler;
  uint8_t command;
  uint8_t filler1;
  uint8_t ip[ola::network::IPV4_LENGTH];
  uint8_t subnet[ola::network::IPV4_LENGTH];
  uint16_t port;
  uint8_t spare[8];
} __attribute__((packed));

typedef struct artnet_ip_prog_s artnet_ip_prog_t;

struct artnet_ip_reply_s {
  uint16_t version;
  uint16_t filler;
  uint8_t command;
  uint8_t filler1;
  uint8_t ip[ola::network::IPV4_LENGTH];
  uint8_t subnet[ola::network::IPV4_LENGTH];
  uint16_t port;
  uint8_t spare[8];
} __attribute__((packed));

typedef struct artnet_ip_reply_s artnet_ip_reply_t;

// union of all artnet packets
typedef struct {
  uint8_t id[8];
  uint16_t op_code;
  union {
    artnet_poll_t poll;
    artnet_reply_t reply;
    artnet_dmx_t dmx;
    artnet_todrequest_t tod_request;
    artnet_toddata_t tod_data;
    artnet_todcontrol_t tod_control;
    artnet_rdm_t rdm;
    artnet_ip_prog_t ip_program;
    artnet_ip_reply_t ip_reply;
  } data;
} artnet_packet;
}  // artnet
}  // plugin
}  // ola
#endif  // PLUGINS_ARTNET_ARTNETPACKETS_H_
