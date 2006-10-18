#!/bin/bash

. /usr/pluto/bin/Utils.sh
. /usr/pluto/bin/Config_Ops.sh

DEVICEDATA_ALSA_Master_Mixer_Name=174
DEVICECATEGORY_Sound_Cards=124

Option="$1"

# get Master mixer name
Sound_Device=$(FindDevice_Category "$PK_Device" "$DEVICECATEGORY_Sound_Cards" "no-recursion")
if [[ -n "$Sound_Device" ]]; then
	Q="
		SELECT IK_DeviceData
		FROM Device_DeviceData
		WHERE FK_Device='$Sound_Device'
			AND FK_DeviceData='$DEVICEDATA_ALSA_Master_Mixer_Name'
	"
	Sound_Master_Mixer=$(RunSQL "$Q")
fi

if [[ -z "$Sound_Master_Mixer" ]]; then
	Sound_Master_Mixer=Master
fi

Settings=$(amixer sget "$Sound_Master_Mixer")

VolMin=$(echo "$Settings" | grep -F 'Limits:' | sed 's/^  *//' | cut -d' ' -f3)
VolMax=$(echo "$Settings" | grep -F 'Limits:' | sed 's/^  *//' | cut -d' ' -f5)
VolCur=$(echo "$Settings" | grep -F 'Front Left:' | sed 's/^  *//' | cut -d' ' -f4)
case "$Option" in
	up)
		VolNew=$((VolCur+1))
		amixer sset "$Sound_Master_Mixer" -- "$VolNew" unmute >/dev/null
	;;

	down)
		VolNew=$((VolCur-1))
		amixer sset "$Sound_Master_Mixer" -- "$VolNew" unmute >/dev/null
	;;

	get-absolute)
		echo "$VolCur"
	;;

	get-percent)
		VolPercent=$((100 * $VolCur / $VolMax))
		echo "$VolPercent"
	;;

	*)
		echo "ERROR: bad option '$Option'"
		exit 1
	;;
esac

exit 0
