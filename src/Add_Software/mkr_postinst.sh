#!/bin/bash
/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :

ln -s /etc/init.d/instcheck.sh /etc/rc2.d/S23instcheck.sh || :

if [[ ! -f /etc/diskless.conf ]] ;then
	/usr/pluto/bin/getxmls &>/var/log/pluto/getxmls.postinst.addsoftware.log </dev/null &
	disown -a
fi											
