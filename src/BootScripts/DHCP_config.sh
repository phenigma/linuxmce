#!/bin/bash

. /usr/pluto/bin/LockUtils.sh
. /usr/pluto/bin/Network_Parameters.sh

WaitLock "PlutoDHCP" "PlutoDHCP_config"
if [[ -z "$DHCPsetting" ]]; then
	rm -f /etc/dhcp3/dhcpd.conf
	/etc/init.d/dhcp3-server stop
	exit
fi
/usr/pluto/bin/PlutoDHCP >/etc/dhcp3/dhcpd.conf
/etc/init.d/dhcp3-server restart
Unlock "PlutoDHCP" "PlutoDHCP_config"
