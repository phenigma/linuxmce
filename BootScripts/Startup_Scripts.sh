#!/bin/bash
. /usr/pluto/bin/Config_Ops.sh

Parameter="$1"
Device="$PK_Device"

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

Q="SELECT Command
FROM Device_StartupScript
JOIN StartupScript ON FK_StartupScript=PK_StartupScript
WHERE Enabled=1 AND FK_Device='$Device'
ORDER BY Boot_Order"

# susceptibile to "spaces in name" problem
scripts=$(RunSQL 0 "$Q" | tr '\n' ' ')
for i in $scripts; do
	"$i" $Parameter
done
