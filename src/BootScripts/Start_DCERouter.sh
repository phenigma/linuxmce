#!/bin/bash
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/pluto.func

cd /usr/pluto/bin
Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Starting DCERouter"

log_file="/var/log/pluto/DCERouter";
new_log="$log_file.newlog"
real_log="$log_file.log"

module=DCERouter
device_name="$module"

screen -d -m -S DCERouter /usr/pluto/bin/Spawn_DCERouter.sh
sleep 5
# Wait 5 seconds so DCERouter has a chance to open the socket and start listening
# TODO: use nc -z and a timeout

/usr/pluto/bin/SetupRemoteAccess.sh

# Delete old packages
cd /usr/pluto/deb-cache/dists/sarge/main/binary-i386/
ThisVersion="$(dpkg -s pluto-dcerouter | grep Version)"
RemovedList="0==1"
for File in pluto-*_*.deb; do
	DiskVersion="$(dpkg --info "$File" | grep Version | cut -c2-)"
	PkgName="$(dpkg --info "$File" | grep Package | cut -d' ' -f3)"
	if [ "$ThisVersion" != "$DiskVersion" ]; then
		rm -f $File
		RemovedList="$RemovedList || \$0 == \"Package: $PkgName\""
	fi
done

if [ "$RemovedList" != "0==1" ]; then
	gunzip -c Packages.gz >Packages.orig
	awk '
		BEGIN { Flag = 0 }
		'"$RemovedList"' { Flag = 1 }
		Flag == 0 { print }
		$0 == "" { Flag = 0 }
	' Packages.orig >Packages.new
	gzip -9c Packages.new >Packages.gz
	rm Packages.new Packages.orig
	apt-get update
fi
cd -

# Workaround for 2.0.0.10 release mistake
XineVer="$(dpkg -s pluto-xine-plugin | grep Version)"
DCERouterVer="$(dpkg -s pluto-dcerouter | grep Version)"

if [ "$XineVer" != "$DCERouterVer" ]; then
	apt-get remove -y pluto-xine-plugin
	apt-get clean
	apt-get install -y pluto-xine-plugin
fi
# End of workaround

SysLogCfg="*.*;auth,authpriv.none	/dev/tty12"
if ! grep -qF "$SysLogCfg" /etc/syslog.conf; then
	echo "$SysLogCfg" >>/etc/syslog.conf
	/etc/init.d/sysklogd reload
fi

if ! grep -q '^SYSLOGD="-r"$' /etc/init.d/sysklogd; then
	sed -i 's/^SYSLOGD=.*$/SYSLOGD="-r"/' /etc/init.d/sysklogd
	/etc/init.d/sysklogd restart
fi
