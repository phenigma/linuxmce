#!/bin/bash

. /usr/pluto/bin/Network_Parameters.sh
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/pluto.func

EnableMaxtorNFS()
{
	[[ -z "$IntIP" ]] && return 1
	
	local IP="$1" LAN="${IntIP%.*}.0/24" # the LAN is semi-hardcoded like this in Network_Firewall too

	[[ -z "$IP" ]] && return 2
	

	Logging "Maxtor-openmss" $SEVERITY_STATUS "EnableMaxtorNFS" "Enabling NFS on Maxtor with IP '$IP'"
	# How this works (because I'm sure I won't understand it later either):
	# purpose: send a list of commands via a free-access telnet server running on the Maxtor Network Storage (1.2.2-openmss2) device
	# 1st part echoes the commands, one per line, ending with "exit"
	# then, it echoes dots
	# this output is sent to socat
	# socat's output is send through another while loop
	# the last while loop exits when it reads "exit", which means it closes socat's stdout
	# socat ends because of no stdout
	# the dot displaying thingy exits because socat exited
	# the dots don't get to the telnet server, but they serve for the ending part, where the "no stdout" condition needs to be triggered
	
	# WARNING: the telnet server seems to crash after I don't know how may attempts like this one; no clue to why
	(echo "mkdir -p /shares/mss-hdd/Pluto
		portmap
		rpc.mountd
		rpc.nfsd
		exportfs -io rw,no_root_squash,no_all_squash $LAN:/shares/mss-hdd/Pluto
		exit"
	while :; do echo -n "."; sleep 1; done) | socat STDIO TCP4:"$IP":23 | while read line; do [[ "$line" == *exit* ]] && break; done

	Logging "Maxtor-openmss" $SEVERITY_STATUS "EnableMaxtorNFS" "Enabled NFS on Maxtor with IP '$IP'"
	return 0
}

Params=("$@")
for ((i = 0; i < ${#Params[@]}; i++)); do
	case "${Params[$i]}" in
		-d) ((i++)); NAS_PK_Device="${Params[$i]}" ;;
	esac
done

[[ -z "$NAS_PK_Device" ]] && exit 1

SQL="SELECT IPaddress FROM Device WHERE PK_Device=$NAS_PK_Device"
NAS_IP_Address=$(RunSQL "$SQL")

[[ -z "$NAS_IP_Address" ]] && exit 2

EnableMaxtorNFS "$NAS_IP_Address" || exit 3

/usr/pluto/bin/Net_Mount.sh "$@"
