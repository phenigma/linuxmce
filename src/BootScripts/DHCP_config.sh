#!/bin/bash

. /usr/pluto/bin/LockUtils.sh

WaitLock "PlutoDHCP" "PlutoDHCP_config"
/usr/pluto/bin/PlutoDHCP >/etc/dhcp3/dhcpd.conf
/etc/init.d/dhcp3-server restart
Unlock "PlutoDHCP" "PlutoDHCP_config"
