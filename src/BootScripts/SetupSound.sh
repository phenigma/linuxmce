#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Utils.sh

DEVICECATEGORY_Sound_Cards=124

SoundCard=$(FindDevice_Category "$PK_Device" "$DEVICECATEGORY_Sound_Cards")

if [[ -z "$SoundCard" ]]; then
	# no sound card device found; nothing to do
	exit 0
fi

Q="
	SELECT CommandLine
	FROM DeviceTemplate
	JOIN Device ON FK_DeviceTemplate=PK_DeviceTemplate AND PK_Device='$SoundCard'
"
CommandLine="$(RunSQL "$Q")"
/usr/pluto/bin/"$CommandLine"
