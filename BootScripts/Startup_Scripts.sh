#!/bin/bash
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/pluto.func

Parameter="$1"
Device="$PK_Device"

[ "$Parameter" == "stop" ] && exit 0

Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Processing startup scripts for device $Device; parameter: $Parameter"
if [ -e /usr/pluto/install/.notdone ]; then
	Logging "$TYPE" "$SEVERITY_CRITICAL" "$0" "It appears the installation was not successful. Pluto's startup scripts are disabled. To enable them, complete the installation process, or manually remove the file /usr/pluto/install/.notdone"
	exit 1
fi

RunSQL()
{
	local Q;

	Headers="$1"; shift
	Q="$*"
	if [ "$Headers" -eq 0 ]; then
		echo "$Q;" | mysql pluto_main | tail +2
	else
		echo "$Q;" | mysql pluto_main
	fi
}

Q="SELECT Command,Enabled
FROM Device_StartupScript
JOIN StartupScript ON FK_StartupScript=PK_StartupScript
WHERE FK_Device='$Device'
ORDER BY Boot_Order"

# susceptibile to "spaces in name" problem
result=$(RunSQL 0 "$Q" | sed 's/\t/,/')

[ -z $Result ] && echo "No boot scripts were configured for device $Device" && exit;

echo "$result" |
while read line; do
	script=$(echo "$line" | cut -d, -f1)
	enabled=$(echo "$line" | cut -d, -f2)

	if [ "$enabled" -eq 0 ]; then
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Skipping '$script' (not enabled)"
	fi
	
	if [ -e "$script" ]; then
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Running '$script' '$Parameter'"
		"$script" $Parameter || Logging "$TYPE" "$SEVERITY_WARNING" "$0" "Failed to run '$script' '$Parameter'"
	elif [ -e "/usr/pluto/bin/$script" ]; then
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Running '/usr/pluto/bin/$script' '$Parameter'"
		"/usr/pluto/bin/$script" $Parameter || Logging "$TYPE" "$SEVERITY_WARNING" "$0" "Failed to run '$script' '$Parameter'"
	else
		Logging "$TYPE" "$SEVERITY_WARNING" "$0" "Boot Script: Command '$script' not found"
	fi
done
