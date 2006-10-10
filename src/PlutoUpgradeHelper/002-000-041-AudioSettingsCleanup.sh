#!/bin/bash

# Remove the 'K' setting from the AudioSettings DeviceData if upgrading from before 2.0.0.41

PrevVer="$1"

if /usr/bin/dpkg --compare-versions "$PrevVer" lt-nl "2.0.0.41"; then
	. /usr/pluto/bin/SQL_Ops.sh
	. /usr/pluto/bin/Config_Ops.sh

	DEVICECATEGORY_Media_Director=8
	DEVICEDATA_Audio_Settings=88

	Q="
		SELECT FK_DeviceCategory
		FROM Device
		JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
		WHERE PK_Device='$PK_Device'
	"
	Category=$(RunSQL "$Q")

	if [[ -z "$Category" ]]; then
		exit 1
	elif [[ "$Category" != "$DEVICECATEGORY_Media_Director" ]]; then
		# it's the Core; search for its embedded MD
		Q="
			SELECT PK_Device
			FROM Device
			JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
			JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory
			WHERE FK_Device_ControlledVia='$PK_Device' AND PK_DeviceCategory='$DEVICECATEGORY_Media_Director'
		"
		MD=$(RunSQL "$Q")
		if [[ -z "$MD" ]]; then
			# No embedded MD found
			exit 1
		fi
	else
		MD="$PK_Device"
	fi

	Q="
		UPDATE Device_DeviceData
		SET IK_DeviceData=REPLACE(IK_DeviceData, 'K', '')
		WHERE FK_Device='$MD' AND FK_DeviceData='$DEVICEDATA_Audio_Settings'
	"
	RunSQL "$Q"
fi
