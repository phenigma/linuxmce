#!/bin/bash

. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Network_Parameters.sh

P="$1"

if [ "$P" == "D" ]; then
	# Selecting diskless MDs only
	Q="SELECT MACaddress
	FROM Device_DeviceData
	JOIN Device ON PK_Device=FK_Device
	JOIN DeviceTemplate ON PK_DeviceTemplate=FK_DeviceTemplate
	WHERE FK_DeviceTemplate=28 AND FK_DeviceData=9 AND IPaddress IS NOT NULL AND MACaddress IS NOT NULL AND IK_DeviceData='1'"
else
	# Selecting all MDs
	Q="SELECT MACaddress
	FROM Device
	JOIN DeviceTemplate ON PK_DeviceTemplate=FK_DeviceTemplate
	WHERE FK_DeviceTemplate=28 AND IPaddress IS NOT NULL AND MACaddress IS NOT NULL"
fi

R=$(RunSQL "$Q")

MaxIt=5
for i in $(seq 1 $MaxIt); do
	for HostMAC in $R; do
		echo "Sending WOL magic packet (iteration $i of $MaxIt) to $HostMAC"
		/usr/sbin/etherwake -b -i "$IntIf" "$HostMAC"
	done
	sleep 1
done
