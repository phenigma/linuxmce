#!/bin/bash

sed -i 's!\<pin_helper.*$!pin_helper /usr/pluto/bin/pluto-pinhelper.sh;!' /etc/bluetooth/hcid.conf
invoke-rc.d bluetooth restart || /bin/true
