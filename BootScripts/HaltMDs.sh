#!/bin/bash

. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/SQL_Ops.sh

P="$1"

if [ "$P" == "D" ]; then
	# Selecting diskless MDs only
	Q="SELECT IPaddress
	FROM Device_DeviceData
	JOIN Device ON PK_Device=FK_Device
	JOIN DeviceTemplate ON PK_DeviceTemplate=FK_DeviceTemplate
	WHERE FK_DeviceTemplate=28 AND FK_DeviceData=9 AND IPaddress IS NOT NULL AND MACaddress IS NOT NULL AND IK_DeviceData='1'"
else
	# Selecting all MDs
	Q="SELECT IPaddress
	FROM Device
	JOIN DeviceTemplate ON PK_DeviceTemplate=FK_DeviceTemplate
	WHERE FK_DeviceTemplate=28 AND IPaddress IS NOT NULL AND MACaddress IS NOT NULL"
fi

R=$(RunSQL "$Q")
for Host in $R; do
	ShutDownRemote "$Host"
done
sleep 5
