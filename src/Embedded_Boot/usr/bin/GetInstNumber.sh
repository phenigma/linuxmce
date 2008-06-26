#!/bin/ash

if [[ -f /etc/pluto/installation_number ]]; then
	exit 1
fi

if /usr/bin/NetworkTest fiire.com 80 >/dev/null; then
	InstNr=$(wget -qO- http://cisco.fiire.com/new_installation.php)
	echo "$InstNr" >/etc/pluto/installation_number
fi
