#!/bin/bash

. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/LockUtils.sh
. /usr/pluto/bin/Network_Parameters.sh
. /usr/pluto/bin/Utils.sh

if [ ! -e /etc/dhcp3/dhcpd.conf.pbackup ] ;then
	cp /etc/dhcp3/dhcpd.conf /etc/dhcp3/dhcpd.conf.pbackup
fi
WaitLock "PlutoDHCP" "DHCP_Restart" nolog
if ! BlacklistConfFiles '/etc/dhcp3/dhcpd.conf' ;then
	/usr/pluto/bin/PlutoDHCP >/etc/dhcp3/dhcpd.conf
	invoke-rc.d dhcp3-server restart
fi
Unlock "PlutoDHCP" "DHCP_Restart" nolog
