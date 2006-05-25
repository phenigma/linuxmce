#!/bin/bash

DeviceID="$1"
DeviceTemplate="${2:-0}"

PlutohomeHost="http://10.0.0.175/plutohome-com/"

if [[ -z "$URL" ]]; then
	echo "ERROR. URL not specified"
	exit 1
fi

if [[ -z "$DeviceID" && -z "$DeviceTemplate" ]]; then
	echo "ERROR. Either the DeviceID or the DeviceTemplate pamaters has to be specified"
	exit 1
fi

if [[ -z "$DeviceTemplate" ]]; then
	Q="
		SELECT FK_DeviceTemplate,FK_InfraredGroup,FK_DeviceCategory,FK_Manufacturer
		FROM Device
		INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
		WHERE PK_Device='$DeviceID'
	"
else
	Q="
		SELECT PK_DeviceTemplate,FK_InfraredGroup,FK_DeviceCategory,FK_Manufacturer
		FROM DeviceTemplate
		WHERE PK_DeviceTemplate='$DeviceTemplate'
	"
fi
Data="$(RunSQL "$Q")"
if [[ -z "$Data" ]]; then
	echo "ERROR. Empty result"
	exit 1
fi

FK_DeviceTemplate=$(Field 1 "$Data")
FK_InfraredGroup=$(Field 2 "$Data")
FK_DeviceCategory=$(Field 3 "$Data")
FK_Manufacturer=$(Field 4 "$Data")

Q="SHOW TABLES LIKE 'InfraredGroup_Command'"
R="$(RunSQL "$Q")"

if [[ -n "$R" ]]; then
	/usr/pluto/bin/WebDB_Get.sh "$PlutohomeHost/GetInfraredCodes.php?PK_InfraredGroup='$FK_InfraredGroup'&PK_DeviceTemplate='$FK_DeviceTemplate'" "InfraredGroup_Command"
	Ret1=$?
	/usr/pluto/bin/WebDB_Get.sh "$PlutohomeHost/GetInfraredCodes.php?PK_Manufacturer='$FK_Manufacturer'&PK_DeviceCategory='$FK_DeviceCategory'" "InfraredGroup_Command"
	Ret2=$?
	Ret=$((Ret1 || Ret2))
else
	if /usr/pluto/bin/WebDB_Get.sh "$PlutohomeHost/GetInfraredCodes.php?Create=1"; then
		/usr/pluto/bin/WebDB_Get.sh "$PlutohomeHost/GetInfraredCodes.php?PK_InfraredGroup='$FK_InfraredGroup'&PK_DeviceTemplate='$FK_DeviceTemplate'" "InfraredGroup_Command"
		Ret=$?
	else
		Ret=1
	fi
fi

exit "$Ret"
