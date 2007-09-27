#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/Utils.sh

DEVICETEMPLATE_PVR_250=1824

DEVICEDATA_Port=171
DEVICEDATA_Block_Device=152

Devices=$(FindDevice_Template "$PK_Device" "$DEVICETEMPLATE_PVR_250" "" "" "all")
for Device in $Devices; do
	Port="$(GetDeviceData "$Device" "$DEVICEDATA_Port")"
	BlockDevice="$(GetDeviceData "$Device" "$DEVICEDATA_Block_Device")"
	/usr/pluto/bin/CaptureCards_SelectInput_PVR-250.sh "$BlockDevice" "$Port"
done
