#!/bin/bash

RunSQL()
{
	local Q
	Q="$*"
	[ -z "$Q" ] || echo "$Q;" | mysql pluto_main | tail +2 | tr '\n\t ' ' ,~'
}

Field()
{
	local Row FieldNumber
	FieldNumber="$1"; shift
	Row="$*"
	echo "$Row" | cut -d, -f"$FieldNumber" | tr '~' ' '
}

CORE_INTERNAL_INTERFACE="$1"
CORE_INTERNAL_ADDRESS="$2"

IfConf="auto $CORE_INTERNAL_INTERFACE
iface $CORE_INTERNAL_INTERFACE inet static
	address $CORE_INTERNAL_ADDRESS
	netmask 255.255.255.0"

if ! grep "$CORE_INTERNAL_INTERFACE" /etc/network/interfaces &>/dev/null; then
	echo "$IfConf" >>/etc/network/interfaces
	ifup "$CORE_INTERNAL_INTERFACE"
fi

NS=$(grep nameserver /etc/resolv.conf | sed 's/#.*$//g; s/nameserver//g; s/^ *//g; s/ *$//g' | tr '\n' ' ')

Fwd="forwarders {"
for i in $NS; do
	Fwd=$(printf "%s" "$Fwd~!$i;")
done
Fwd=$(printf "%s" "$Fwd~}~")
echo "$Fwd" | tr '~!' '\n\t' >/etc/bind9/named.conf.forwarders

awk 'BEGIN { Replace = 0 }
/\/\/ forwarders/ {
	Replace = 3;
	print("include \"/etc/bind9/named.conf.forwarders\";\n");
}
Replace == 0 { print }
Replace != 0 { Replace-- }
' /etc/bind/named.conf.options >/etc/bind/named.conf.options.$$
mv /etc/bind/named.conf.options.$$ /etc/bind/named.conf.options
rndc reload
