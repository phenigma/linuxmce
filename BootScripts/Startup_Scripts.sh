#!/bin/bash
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/pluto.func

Parameter="$1"; shift
Device="$PK_Device"

# TODO: script parameter is obsolete (already); remove

rm /var/log/pluto/running.pids

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

RunSQL()
{
	local Q;

	Headers="$1"; shift
	Q="$*"
	if [ "$Headers" -eq 0 ]; then
		echo "$Q;" | mysql -h $MySqlHost pluto_main | tail +2
	else
		echo "$Q;" | mysql -h $MySqlHost pluto_main
	fi
}

if [ -n "$Script" ]; then
	Q="SELECT PK_Device
FROM Device
WHERE IPaddress='$moonIP'
LIMIT 1"
	Device=$(RunSQL 0 "$Q")
fi

Q="SELECT Command,Enabled
FROM Device_StartupScript
JOIN StartupScript ON FK_StartupScript=PK_StartupScript
WHERE FK_Device='$Device' AND StartupScript.When='$When'
ORDER BY Boot_Order"

if [ ! -e /etc/pluto.startup ]; then
	result=$(RunSQL 0 "$Q" | tr '\t ' ',~')
else
	result=$(cat /etc/pluto.startup)
fi

if [ -n "$Script" ]; then
	echo "$result"
	exit
fi
[ -z "$result" ] && echo "No boot scripts were configured for device $Device for '$When'" && exit

echo "$result" |
while read line; do
	script=$(echo "$line" | cut -d, -f1 | tr '~' ' ')
	enabled=$(echo "$line" | cut -d, -f2)

	if [ "$enabled" -eq 0 ]; then
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Skipping '$script' (not enabled)"
		continue
	fi
	
	if [ -e "./$script" ]; then
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Running '$script' '$Parameter'"
		"./$script" $Parameter || Logging "$TYPE" "$SEVERITY_WARNING" "$0" "Failed to run '$script' '$Parameter'"
	elif [ -e "/usr/pluto/bin/$script" ]; then
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Running '/usr/pluto/bin/$script' '$Parameter'"
		"/usr/pluto/bin/$script" $Parameter || Logging "$TYPE" "$SEVERITY_WARNING" "$0" "Failed to run '$script' '$Parameter'"
	elif [ -e "$script" ]; then
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Running '$script' '$Parameter'"
		"$script" $Parameter || Logging "$TYPE" "$SEVERITY_WARNING" "$0" "Failed to run '$script' '$Parameter'"
	else
		Logging "$TYPE" "$SEVERITY_WARNING" "$0" "Boot Script: Command '$script' not found"
	fi
done
