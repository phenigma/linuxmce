#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh

Q="SELECT Command
FROM Device_StartupScript
JOIN StartupScript ON FK_StartupScript=PK_StartupScript
WHERE FK_Device='865' AND StartupScript.When='S' AND Enabled=1
	AND Command IN ('Start_ratpoison.sh', 'Start_DCERouter.sh', 'Start_LocalDevices.sh')
ORDER BY Boot_Order;"

Scripts=$(echo "$Q" | mysql -N -h $MySqlHost pluto_main | tr '\n' ' ')
for script in $Scripts; do
	if [ -e "./$script" ]; then
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Running '$script' '$Parameter'"
		"./$script" $Parameter || Logging "$TYPE" "$SEVERITY_WARNING" "$0" "Failed to run '$script' '$Parameter'"
	elif [ -e "/usr/pluto/bin/$script" ]; then
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Running '/usr/pluto/bin/$script' '$Parameter'"
		"/usr/pluto/bin/$script" $Parameter || Logging "$TYPE" "$SEVERITY_WARNING" "$0" "Failed to run '$script' '$Parameter'"
	elif [ -e "$script" ]; then
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Running '/usr/pluto/bin/$script' '$Parameter'"
		"$script" $Parameter || Logging "$TYPE" "$SEVERITY_WARNING" "$0" "Failed to run '$script' '$Parameter'"
	else
		Logging "$TYPE" "$SEVERITY_WARNING" "$0" "Boot Script: Command '$script' not found"
	fi
done
