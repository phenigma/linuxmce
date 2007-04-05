#!/bin/bash
/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :

cp /etc/bluetooth/hcid.conf /etc/bluetooth/hcid.conf.pbackup
sed -i 's!\<pin_helper.*$!pin_helper /usr/pluto/bin/pluto-pinhelper.sh;!' /etc/bluetooth/hcid.conf
invoke-rc.d bluez-utils restart
