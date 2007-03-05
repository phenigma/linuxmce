#!/bin/bash
/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :

sed -i 's!\<pin_helper.*$!pin_helper /usr/pluto/bin/pluto-pinhelper.sh;!' /etc/bluetooth/hcid.conf
invoke-rc.d bluez-utils restart
