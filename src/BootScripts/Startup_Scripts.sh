#!/bin/bash
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/SQL_Ops.sh

Parameter="$1"; shift
Device="$PK_Device"

# TODO: script parameter is obsolete (already); remove

rm /var/log/pluto/running.pids
rm -rf /tmp/* # I doubt that this is safe to do here

# remove all ttyS_* (created by gc100s) entries from /dev
rm -f /dev/ttyS_*

# clean up locks
mkdir -p /usr/pluto/locks
rm -f /usr/pluto/locks/*

# assure some settings
Q="SELECT FK_Installation FROM Device WHERE PK_Device='$PK_Device'"
R="$(RunSQL "$Q")"
ConfSet PK_Installation "$R"
Q="SELECT PK_Users FROM Users LIMIT 1"
R="$(RunSQL "$Q")"
ConfSet PK_Users "$R"

if [ "$Parameter" != "start" -a "$Parameter" != "script" -a "$Parameter" != "stop" ]; then
	echo "Usage: $0 start|script|stop"
	exit 1
fi

if [ "$Parameter" == "script" ]; then
	Script=1
	moonIP="$1"
fi

if [ "$Parameter" == "start" -o "$Parameter" == "script" ]; then
	When="S"
elif [ "$Parameter" == "stop" ]; then
	When="P"
fi

Parameter="$1"
[ -z "$Parameter" ] && unset Parameter

[ -z "$Script" ] && Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Processing startup scripts for device $Device; parameter: $Parameter"
if [ -e /usr/pluto/install/.notdone ]; then
	[ -z "$Script" ] && Logging "$TYPE" "$SEVERITY_CRITICAL" "$0" "It appears the installation was not successful. Pluto's startup scripts are disabled. To enable them, complete the installation process, or manually remove the file /usr/pluto/install/.notdone"
	exit 1
fi

if [ -n "$Script" ]; then
	Q="SELECT PK_Device
FROM Device
WHERE IPaddress='$moonIP'
LIMIT 1"
	Device=$(RunSQL 0 "$Q")
fi

Q="SELECT Command,Enabled,Background,FK_DeviceTemplate
FROM Device_StartupScript
JOIN StartupScript ON FK_StartupScript=PK_StartupScript
WHERE FK_Device='$Device' AND StartupScript.When='$When'
ORDER BY Boot_Order"

if [ ! -e /etc/pluto.startup ]; then
	result="$(RunSQL "$Q")"
else
	result="$(cat /etc/pluto.startup)"
fi

if [ -n "$Script" ]; then
	echo "$result"
	exit
fi
[ -z "$result" ] && echo "No boot scripts were configured for device $Device for '$When'" && exit

FindDevice()
{
	local PK_Device_Parent="${1//'}" FK_DeviceTemplate="${2//'}"

	if [ -z "$PK_Device_Parent" -o -z "$FK_DeviceTemplate" ]; then
		echo ""
		return 1
	fi

	local i R Q
	Q="SELECT PK_Device FROM Device WHERE FK_Device_ControlledVia='$PK_Device_Parent' AND FK_DeviceTemplate='$FK_DeviceTemplate'"
	R="$(RunSQL "$Q")"

	if [ -z "$R" ]; then
		Q="SELECT PK_Device FROM Device WHERE FK_Device_ControlledVia='$PK_Device_Parent'"
		R="$(RunSQL "$Q")"
		for i in $R; do
			FindDevice "$i" "$FK_DeviceTemplate" && return 0
		done
	else
		echo "$R"
		return 0
	fi
	return 1
}

for line in $result; do
	script="$(Field 1 $line)"
	enabled="$(Field 2 $line)"
	Background="$(Field 3 "$line")"
	FK_DeviceTemplate="$(Field 4 $line)"

	if [ "$enabled" -eq 0 ]; then
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Skipping '$script' (not enabled)"
		continue
	fi
	
	if [ "$FK_DeviceTemplate" != NULL ]; then
		FoundDevice="$(FindDevice "$PK_Device" "$FK_DeviceTemplate")"
		if [ -z "$FoundDevice" ]; then
			Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Skipping '$script' (no corresponding device found)"
			continue
		fi
	fi
	
	Screen=""
	[ "$Background" -ne 0 ] && Screen="screen -d -m -S \"BkgSS-$script\""
	
	if [ -e "./$script" ]; then
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Running '$script' '$Parameter'"
		eval $Screen "\"./$script\"" $Parameter || Logging "$TYPE" "$SEVERITY_WARNING" "$0" "Failed to run '$script' '$Parameter'"
	elif [ -e "/usr/pluto/bin/$script" ]; then
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Running '/usr/pluto/bin/$script' '$Parameter'"
		eval $Screen "\"/usr/pluto/bin/$script\"" $Parameter || Logging "$TYPE" "$SEVERITY_WARNING" "$0" "Failed to run '$script' '$Parameter'"
	elif [ -e "$script" ]; then
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Running '$script' '$Parameter'"
		eval $Screen "\"$script\"" $Parameter || Logging "$TYPE" "$SEVERITY_WARNING" "$0" "Failed to run '$script' '$Parameter'"
	else
		Logging "$TYPE" "$SEVERITY_WARNING" "$0" "Boot Script: Command '$script' not found"
	fi
done

# Someone said this fixed his NFS problems; I'm putting it here for both Core and MDs
echo 1 >/proc/sys/net/ipv4/ip_no_pmtu_disc

# apt-proxy was replaced in 2.0.0.12 with the polipo+frox combination
# translating all apt-proxy references in sources.list
if grep -qF dcerouter:9999 /etc/apt/sources.list 2>/dev/null; then
	sed -i 's,http://dcerouter:9999/,,g' /etc/apt/sources.list
fi

# Errors are too verbose by default
dmesg -n 1
