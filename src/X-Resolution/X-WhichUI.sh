#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

Q="
	SELECT COUNT(IK_DeviceData)
	FROM Device_DeviceData
	JOIN Device ON Device_DeviceData.FK_Device=Device.PK_Device
	LEFT JOIN Device as P1 on Device.FK_Device_ControlledVia=P1.PK_Device
	WHERE FK_DeviceData=104 and IK_DeviceData=4 AND (P1.PK_Device=$PK_Device OR P1.FK_Device_ControlledVia=$PK_Device)
"
R=$(RunSQL "$Q")

if [[ -n "$R" && "$R" -gt 0 ]]; then
	return 2
else
	return 1
fi
