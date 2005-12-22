#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Maxtor_openmss_EnableNFS.sh

Params=("$@")
for ((i = 0; i < ${#Params[@]}; i++)); do
	case "${Params[$i]}" in
		-d) ((i++)); NAS_PK_Device="${Params[$i]}" ;;
		-i) ((i++)); NAS_IP_Address="${Params[$i]}" ;;
		-m) ((i++)); NAS_MAC_Address="${Params[$i]}" ;;
	esac
done

[[ -z "$NAS_PK_Device" ]] && exit 1

if [[ -z "$NAS_IP_Address" ]]; then
	SQL="SELECT IPaddress FROM Device WHERE PK_Device=$NAS_PK_Device"
	NAS_IP_Address=$(RunSQL "$SQL")
fi

[[ -z "$NAS_IP_Address" ]] && exit 2

EnableMaxtorNFS "$NAS_IP_Address" || exit 3

/usr/pluto/bin/Net_Mount.sh "$@"
