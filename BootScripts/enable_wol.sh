#!/bin/bash
# enable Wake-on-LAN on eth0

echo "Enabling Wake-on-LAN (WOL) for eth0"
ethtool -s eth0 wol g

