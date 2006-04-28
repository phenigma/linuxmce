#!/bin/bash

. /usr/pluto/bin/Network_Parameters.sh
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/pluto.func


function EnableMaxtorNFS()
{
	[[ -z "$IntIP" ]] && return 1

        local IP="$1" LAN="${IntIP%.*}.0/24" # the LAN is semi-hardcoded like this in Network_Firewall too

        [[ -z "$IP" ]] && return 2

        /usr/pluto/bin/Maxtor_openmss_EnableNFS "$IP" > >(tee /var/log/pluto/Maxtor_openmss_EnableNFS.newlog) 2>&1
        return 0
}

Params=("$@")
for ((i = 0; i < ${#Params[@]}; i++)); do
        case "${Params[$i]}" in
                -d) ((i++)); Device_ID="${Params[$i]}" ;;
                -i) ((i++)); Device_IP="${Params[$i]}" ;;
                -m) ((i++)); Device_MAC="${Params[$i]}" ;;
        esac
done

dd[[ -z "$NAS_PK_Device" ]] && exit 1

#Check PK_Device
if [[ "$Device_ID" == "" ]]; then
        echo "ERROR: No PK_Device assigned to this device"
        exit 1
fi

## Check Device ip address
if [[ "$Device_IP" == "" ]]; then
        Q="SELECT IPaddress FROM Device WHERE PK_Device = '$Device_ID'"
        Device_IP=$(RunSQL "$Q")
fi
if [[ "$Device_IP" == "" ]]; then
        echo "ERROR: No IP associated with the device $Device_ID"
        exit 2
fi

EnableMaxtorNFS "$Device_IP" || exit 3

/usr/pluto/bin/StorageDevices_PlutoDirStructure.sh -d $Device_ID
/usr/pluto/bin/StorageDevices_Setup.sh --cascade
