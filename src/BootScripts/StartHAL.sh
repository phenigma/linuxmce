#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Utils.sh

DEVICETEMPLATE_HAL=1808

HALdev=$(FindDevice_Template "$PK_Device" "$DEVICETEMPLATE_HAL" "norecursion")
if [[ -z "$HALdev" ]]; then
	# HAL device not found
	exit 1
fi

screen -d -m -S "HAL-$HALdev" /usr/pluto/bin/Spawn_Device.sh $HALdev $DCERouter HAL
