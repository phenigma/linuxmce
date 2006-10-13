#!/bin/bash

# This script looks for samba servers on the network and trigers a device detected event
# for every server that it finds.

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/Network_Parameters.sh

if [[ $1 != "background" ]] ;then
	echo "Backgrounding ..."
	screen -d -m -S "SambaRadar" "$0" "background"
	exit 0
fi

## Loging function
function Log() {
	echo "$(date) $*" >> /var/log/pluto/SambaScanner.log
}

## Transform decimal-dot netmask in decimal netmask
function netmaskLen() {
        netmask=$1
        len=0
        for i in 1 2 3 4 ;do
                netmaskPart=$(echo $netmask | cut -d'.' -f$i)

                while [ $netmaskPart -gt 0 ] ;do
                        len=$(( len + $netmaskPart % 2 ))
                        netmaskPart=$(( $netmaskPart / 2 ))
                done
        done

        echo $len
}


while : ;do
	## Store in IntNetwork the network address with netmask in decimal format
	IntNetwork="${IntIP}/$(netmaskLen "${IntNetmask}")"


	## Scan the network for samba servers
	Log "Scanning network $IntNetwork"
	for outputLine in $( nbtscan $IntNetwork -s '#' -q | tr -d ' ') ;do
		## Get the important info
		serverIP=$(echo $outputLine | cut -d'#' -f1)
		serverName=$(echo $outputLine | cut -d'#' -f2)
		serverMAC=$(echo $outputLine | cut -d'#' -f5)

		Log "Processing $serverIP"

		## Check to see if the ip is allready in the database
		Q="
			SELECT IPaddress FROM Device WHERE IPaddress='${serverIP}' LIMIT 1;
			SELECT IPaddress FROM UnknownDevices WHERE IPaddress='${serverIP}' LIMIT 1;
			SELECT IPaddress FROM PnpQueue WHERE IPaddress='${serverIP}' LIMIT 1
		"
		Result=$(RunSQL "$Q")

		## If the IP is not there allready
		if [[ "$Result" == "" ]] ;then
			## Report it to the router
			/usr/pluto/bin/MessageSend $DCERouter 0 -1001 2 65 56 "fileserver" 52 3 53 2 49 1837 5 "$serverMAC" 28 "$serverIP" 55 "186|$serverName" 54 "\\\\$serverIP"
			Log "Reporting $serverIP [$serverName] to the router"
			
		## If the ip allready exists in the database
		else
			Log "$serverIP is allready in the database"
		fi 

	done

	## Wait 60 seconds before doing another scan
	sleep 60
done
