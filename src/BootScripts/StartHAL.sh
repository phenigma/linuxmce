#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Utils.sh

DEVICETEMPLATE_HAL=1808
DEVICETEMPLATE_APPSERVER=26

HALdev=$(FindDevice_Template "$PK_Device" "$DEVICETEMPLATE_HAL" "norecursion")
if [[ -z "$HALdev" ]]; then
	# HAL device not found
	exit 1
fi

screen -d -m -S "HAL-$HALdev" /usr/pluto/bin/Spawn_Device.sh $HALdev $DCERouter HAL

ASdev=$(FindDevice_Template "$PK_Device" "$DEVICETEMPLATE_APPSERVER" "norecursion")
if [[ -z "$ASdev" ]]; then
	# App Server device not found
	exit 1
fi

screen -d -m -S "APP-$ASdev" /usr/pluto/bin/Spawn_Device.sh $ASdev $DCERouter APP

