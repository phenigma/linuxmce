#!/bin/bash

invoke-rc.d mysql start

. /usr/pluto/bin/Config_Ops.sh

if [[ -f /etc/pluto/install_cleandb ]]; then
	. /usr/pluto/bin/SQL_Ops.sh

	# Reconfigure all devices after install
	Q="UPDATE Device SET NeedConfigure=1, Status='**RUN_CONFIG**'"
	RunSQL "$Q"

	# Install the software again for the devices
	Q="DELETE FROM Package_Device"
	RunSQL "$Q"

	/usr/pluto/bin/Diskless_Setup.sh
	/usr/pluto/bin/DHCP_config.sh

	if [[ -d /.upgrade-diskless ]]; then
		pushd /.upgrade-diskless &>/dev/null
		while read MD; do
			Dir="/usr/pluto/diskless/$MD/etc"
			if [[ -d "$Dir" ]]; then
				mv "$MD"/etc/pluto.conf "$Dir"
			fi
		done < <(find . -mindepth 1 -maxdepth 1 -type d)
		popd &>/dev/null
		rm -rf /.upgrade-diskless
	fi

	rm -f /etc/pluto/install_cleandb
fi

export DISPLAY=:${Display}
XPID=$(</var/run/plutoX$Display.pid)
if [[ -z "$XPID" || ! -d /proc/"$XPID" ]] ;then
	/usr/pluto/bin/Start_X.sh -config /etc/X11/xorg.conf
fi

export KDE_DEBUG=1
/usr/pluto/bin/lmce_launch_manager.sh


