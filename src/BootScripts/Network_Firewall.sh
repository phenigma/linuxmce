#!/bin/bash

. /usr/pluto/bin/Network_Parameters.sh
. /usr/pluto/bin/LockUtils.sh

OpenPort()
{
	local Protocol Port FilterIP
	local FilterMsg
	local parmPort
	
	Protocol="$1"
	Port="$2"
	FilterIP="${3:-0.0.0.0/0}"

	[[ "$FilterIP" != "0.0.0.0/0" ]] && FilterMsg="; Limited to: $FilterIP"

	echo "  Port: $Port/$Protocol$FilterMsg"

	if [[ -n "$Port" && ( "$Port" == *:* || "$Port" -ne 0 ) ]]; then
		parmPort="--dport $Port"
	fi
	iptables -A INPUT -p "$Protocol" -s "$FilterIP" $parmPort -j ACCEPT
}

ForwardPort()
{
	local Protocol ExtIP SrcPort DestIP DestPort FilterIP
	local FilterMsg

	Protocol="$1"
	ExtIP="$2"
	SrcPort="$3"
	DestIP="$4"
	DestPort="$5"
	FilterIP="${6:-0.0.0.0/0}"

	[[ "$DestPort" -eq 0 ]] && DestPort="$SrcPort"
	[[ "$FilterIP" != "0.0.0.0/0" ]] && FilterMsg="; Limited to: $FilterIP"
	[[ "$ExtIP" == dhcp ]] && ExtIP="0.0.0.0/0"
	
	echo "  Source port: $SrcPort/$Protocol; Destination: $DestIP:$DestPort$FilterMsg"
	iptables -t nat -A PREROUTING -p "$Protocol" -s "$FilterIP" -d "$ExtIP" --dport "$SrcPort" -j DNAT --to-destination "$DestIP:$DestPort"
}

ClearFirewall()
{
	local table
	
	echo "Clearing firewall"
	for table in $(< /proc/net/ip_tables_names); do
		iptables -t "$table" -F
		iptables -t "$table" -X
		for chain in $(iptables -t "$table" -nL|grep ^Chain|cut -d' ' -f2); do
			iptables -t "$table" -P "$chain" ACCEPT
		done
	done
}

trap 'Unlock "Firewall" "Firewall"' EXIT
WaitLock "Firewall" "Firewall"

ClearFirewall

echo "Enabling packet forwarding"
echo 1 >/proc/sys/net/ipv4/ip_forward

if [[ "$DisableFirewall" == 1 ]]; then
	echo "Firewall disabled"
	exit
fi

echo "Setting up firewall"
iptables -P INPUT DROP
iptables -F INPUT
iptables -t nat -F POSTROUTING
iptables -t nat -F PREROUTING

iptables -A INPUT -i lo -j ACCEPT
iptables -A INPUT -m state --state ESTABLISHED,RELATED -j ACCEPT
if [[ -n "$IntIP" ]]; then
	#TODO: use 4 byte netmask in these calculations
	IntNet="$(echo "$IntIP" | cut -d. -f-3).0"
	IntBitmask=24
	iptables -A INPUT -p udp --dport 67 -j ACCEPT # BOOTP/DHCP
	iptables -A INPUT -s "$IntNet/$IntBitmask" -j ACCEPT

	if [[ "$ExtIP" != "dhcp" ]]; then
		iptables -t nat -A POSTROUTING -s "$IntNet/$IntBitmask" -d \! "$IntNet/$IntBitmask" -o $ExtIf -j SNAT --to-source $ExtIP
	else
		iptables -t nat -A POSTROUTING -s "$IntNet/$IntBitmask" -d \! "$IntNet/$IntBitmask" -o $ExtIf -j MASQUERADE
	fi
fi

echo "Setting up forwarded ports"
Q="SELECT Protocol,SourcePort,SourcePortEnd,DestinationPort,DestinationIP,SourceIP FROM Firewall WHERE RuleType='port_forward' ORDER BY PK_Firewall"
R=$(RunSQL "$Q")

for Port in $R; do
	Protocol=$(Field 1 "$Port")
	SrcPort=$(Field 2 "$Port")
	SrcPortEnd=$(Field 3 "$Port")
	DestPort=$(Field 4 "$Port")
	DestIP=$(Field 5 "$Port")
	SrcIP=$(Field 6 "$Port")

	if [[ "$SrcPortEnd" -eq 0 ]]; then
		ForwardPort "$Protocol" "$ExtIP" "$SrcPort" "$DestIP" "$DestPort"
	else
		DPort="$DestPort"
		for ((SPort=SrcPort; SPort<=SrcPortEnd; SPort++)); do
			ForwardPort "$Protocol" "$ExtIP" "$SPort" "$DestIP" "$DPort" "$SrcIP"
			[[ "$DPort" -ne 0 ]] && ((++DPort))
		done
	fi
done

echo "Opening specified ports to exterior"
Q="SELECT Protocol,SourcePort,SourcePortEnd,SourceIP FROM Firewall WHERE RuleType='core_input' ORDER BY PK_Firewall"
R=$(RunSQL "$Q")

for Port in $R; do
	Protocol=$(Field 1 "$Port")
	Port1=$(Field 2 "$Port")
	Port2=$(Field 3 "$Port")
	SrcIP=$(Field 4 "$Port")

	[[ "$Port2" -eq 0 ]] && Port2="$Port1"
	OpenPort "$Protocol" "$Port1:$Port2" "$SrcIP"
done
