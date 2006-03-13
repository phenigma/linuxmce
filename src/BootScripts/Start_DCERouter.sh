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
# wait for DCERouter to start listening on socket
while ! nc -z localhost 3450; do
	sleep 0.1
done

/usr/pluto/bin/SetupRemoteAccess.sh &
disown -a

SysLogCfg="*.*;auth,authpriv.none	/dev/tty12"
if ! grep -qF "$SysLogCfg" /etc/syslog.conf; then
	echo "$SysLogCfg" >>/etc/syslog.conf
	/etc/init.d/sysklogd reload
fi

if ! grep -q '^SYSLOGD="-r"$' /etc/init.d/sysklogd; then
	sed -i 's/^SYSLOGD=.*$/SYSLOGD="-r"/' /etc/init.d/sysklogd
	/etc/init.d/sysklogd restart
fi
