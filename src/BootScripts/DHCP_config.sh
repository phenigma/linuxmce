#!/bin/bash

. /usr/pluto/bin/LockUtils.sh
. /usr/pluto/bin/Network_Parameters.sh

WaitLock "PlutoDHCP" "PlutoDHCP_config"
if [[ -z "$DHCPsetting" ]]; then
	rm -f /etc/dhcp3/dhcpd.conf
	invoke-rc.d dhcp3-server stop
	exit
fi
if [ ! -e /etc/dhcp3/dhcpd.conf.pbackup ] ;then
	cp /etc/dhcp3/dhcpd.conf /etc/dhcp3/dhcpd.conf.pbackup
fi
/usr/pluto/bin/PlutoDHCP >/etc/dhcp3/dhcpd.conf
invoke-rc.d dhcp3-server restart
Unlock "PlutoDHCP" "PlutoDHCP_config"
