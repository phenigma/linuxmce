#!/bin/bash

. /usr/pluto/bin/Network_Parameters.sh

echo "Enabling packet forwarding"
echo 1 >/proc/sys/net/ipv4/ip_forward
modprobe ip_conntrack
modprobe ip_conntrack_ftp
modprobe ip_conntrack_irc

echo "Setting up firewall"
iptables -P INPUT DROP
iptables -F INPUT
iptables -t nat -F POSTROUTING

#TODO: use 4 byte netmask in these calculations
IntNet="$(echo "$IntIP" | cut -d. -f-3).0"
IntBitmask=24

iptables -A INPUT -i lo -j ACCEPT
iptables -A INPUT -m state --state ESTABLISHED,RELATED -j ACCEPT
iptables -A INPUT -p udp --dport 67 -j ACCEPT # BOOTP/DHCP
iptables -A INPUT -s "$IntNet/$IntBitmask" -j ACCEPT
iptables -t nat -A POSTROUTING -s "$IntNet/$IntBitmask" -j MASQUERADE
