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
Q="SELECT Protocol,SourcePort,SourcePortEnd,DestinationPort,DestinationIP FROM Firewall WHERE RuleType='port_forward' ORDER BY PK_Firewall"
R=$(RunSQL "$Q")

ForwardPort()
{
	local Protocol ExtIP SrcPort DestIP DestPort

	Protocol="$1"
	ExtIP="$2"
	SrcPort="$3"
	DestIP="$4"
	DestPort="$5"

	[ "$DestPort" -eq 0 ] && DestPort="$SrcPort"
	
	echo "  Source port: $SrcPort/$Protocol; Destination: $DestIP:$DestPort"
	iptables -t nat -A PREROUTING -p "$Protocol" -d "$ExtIP" --dport "$SrcPort" -j DNAT --to-destination "$DestIP:$DestPort"
}

for Port in $R; do
	Protocol=$(Field 1 "$Port")
	SrcPort=$(Field 2 "$Port")
	SrcPortEnd=$(Field 3 "$Port")
	DestPort=$(Field 4 "$Port")
	DestIP=$(Field 5 "$Port")

	if [ "$SrcPortEnd" -eq 0 ]; then
		ForwardPort "$Protocol" "$ExtIP" "$SrcPort" "$DestIP" "$DestPort"
	else
		DPort="$DestPort"
		for SPort in $(seq $SrcPort $SrcPortEnd); do
			ForwardPort "$Protocol" "$ExtIP" "$SPort" "$DestIP" "$DPort"
			: $((++DPort))
		done
	fi
done

echo "Opening specified ports"
Q="SELECT Protocol,SourcePort,SourcePortEnd FROM Firewall WHERE RuleType='core_input' ORDER BY PK_Firewall"
R=$(RunSQL "$Q")

OpenPort()
{
	local Protocol Port
	Protocol="$1"
	Port="$2"

	echo "  Port: $Port/$Protocol"
	iptables -A INPUT -p "$Protocol" --dport "$Port" -j ACCEPT
}

for Port in $R; do
	Protocol=$(Field 1 "$Port")
	Port1=$(Field 2 "$Port")
	Port2=$(Field 3 "$Port")

	[ "$Port2" -eq 0 ] && Port2="$Port1"
	for FPort in $(seq $Port1 $Port2); do
		OpenPort "$Protocol" "$FPort"
	done
done
