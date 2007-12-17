#!/bin/bash

Usage()
{
	echo "Usage: $0 {--ext-dhcp|--ext-static <IP> <Netmask> <Gateway> <DNS1> <DNS2>|--help}"
	exit 0
}

if [[ "$#" == 0 ]]; then
	Usage
fi

for ((i = 1; i <= "$#"; i++)); do
	case "${!i}" in
		--ext-dhcp) IpMode=DHCP ;;
		--ext-static)
			IpMode=static
			((i++)); IP="${!i}"
			((i++)); Netmask="${!i}"
			((i++)); Gateway="${!i}"
			((i++)); DNS1="${!i}"
			((i++)); DNS2="${!i}"
		;;
		--help|*) Usage ;;
	esac
done

. /usr/pluto/bin/SQL_Ops.sh

DEVICEDATA_Network_Interfaces=32

Q="
	SELECT IK_DeviceData
	FROM Device_DeviceData
	WHERE FK_DeviceData='$DEVICEDATA_Network_Interfaces'
		AND FK_Device='$PK_Device'
"
NetworkInterfaces=$(RunSQL "$Q")
IfExtData="${NetworkInterfaces%|*}"
IfIntData="${NetworkInterfaces##*|}"
IfExt="${IfExtData%%,*}"

if [[ "$IpMode" == static ]]; then
	NewIfExtData="$IfExt,$IP,$Netmask,$Gateway,$DNS1"
	if [[ -n "$DNS2" ]]; then
		NewIfExtData="$NewIfExtData,$DNS2"
	fi
elif [[ "$IpMode" == DHCP ]]; then
	NewIfExtData="$IfExt,dhcp"
else
	Usage
fi

NewNetworkInterfaces="$NewIfExtData|$IfIntData"
Q="
	UPDATE Device_DeviceData
	SET IK_DeviceData='$NewNetworkInterfaces'
	WHERE FK_DeviceData='$DEVICEDATA_Network_Interfaces'
		AND FK_Device='$PK_Device'
"
RunSQL "$Q"

/usr/pluto/bin/Network_Setup.sh
