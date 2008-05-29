#!/bin/sh

mkdir -p /etc/pluto/save/
cp /etc/pluto/* /etc/pluto/save/
cp /etc/pluto/orig/* /etc/pluto/

uci set network.wan.proto=dhcp
uci set network.eth0.vlan0="1 2 3 4 5*"
uci set network.eth0.vlan1="0 5"
uci commit
reboot
