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

echo "Setting up forwarded ports"
Q="SELECT Protocol,SourcePort,SourcePortEnd,DestinationPort,DestinationIP FROM PortForward ORDER BY PK_PortForward"
R=$(RunSQL "$Q")

ForwardPort()
{
	local Protocol ExtIP SrcPort DestIP DestPort

	Protocol="$1"
	ExtIP="$2"
	SrcPort="$3"
	DestIP="$4"
	DesrPort="$5"
	
	echo "  Source port: $Port; Destination: $DestIP:$DNATport"
	iptables -t nat -A PREROUTING -p "$Protocol" -d "$ExtIP" --dport "$Port" -j DNAT --to-destination "$DestIP:$DNATport"
}

for Port in $R; do
	Protocol=$(Field 1 "$Port")
	SrcPort=$(Field 2 "$Port")
	SrcPortEnd=$(Field 3 "$Port")
	DestPort=$(Field 4 "$Port")
	DestIP=$(Field 5 "$Port")

	if [ "$SrcPortEnd" -eq 0 ]; then
		DNATport="$DestPort"
		[ "$DNATport" -eq 0 ] && DNATport=$SrcPort
	else
	fi
	
done
