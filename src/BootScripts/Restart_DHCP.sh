#!/bin/bash

# XXX: This script is a workaround to the "LPF/icmp socket permission denied that we're experiencing"
# XXX: This is NOT A FIX
# XXX: It restarts the DHCP server if needed

. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/LockUtils.sh
. /usr/pluto/bin/Network_Parameters.sh

while :; do
	WaitLock "PlutoDHCP" "DHCP_Restart" nolog
	IsRunning=$(pgrep dhcpd3)
	if [[ -z "$IsRunning" && -n "$DHCPsetting" ]]; then
		Logging "$TYPE" "$SEVERITY_CRITICAL" "DHCP_Restart" "DHCP not found running; restarting it"
		/etc/init.d/dhcp3-server restart
	fi
	Unlock "PlutoDHCP" "DHCP_Restart" nolog
	sleep 5
done
