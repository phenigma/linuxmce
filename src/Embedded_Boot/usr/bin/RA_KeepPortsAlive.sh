#!/bin/ash

Website="cisco.fiire.com"
InstID=$(cat /etc/pluto/installation_number)
RaPass=$(cat /etc/pluto/ra_password 2>/dev/null)

if [[ -z "$InstID" ]]; then
	echo "Unusable installation number: '$InstID'"
	exit 1
fi

if [[ -z "$RaPass" ]]; then
	echo "Remote assistance is not enabled. Nothing to keep alive"
	exit 0
fi

PortPage="http://$Website/get_ra_ports.php?installationID=$InstID"
wget -qO /dev/null "$PortPage" &>/dev/null
