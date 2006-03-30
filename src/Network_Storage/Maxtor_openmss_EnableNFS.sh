#!/bin/bash

. /usr/pluto/bin/Network_Parameters.sh
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/pluto.func

EnableMaxtorNFS()
{
	[[ -z "$IntIP" ]] && return 1
	
	local IP="$1" LAN="${IntIP%.*}.0/24" # the LAN is semi-hardcoded like this in Network_Firewall too

	[[ -z "$IP" ]] && return 2
	
	Logging "Maxtor-openmss" $SEVERITY_NORMAL "EnableMaxtorNFS" "Enabling NFS on Maxtor with IP '$IP'"
	Logging "Maxtor-openmss" $SEVERITY_NORMAL "EnableMaxtorNFS" "Cmd: /usr/pluto/bin/Maxtor_openmss_EnableNFS \"$IP\""
	/usr/pluto/bin/Maxtor_openmss_EnableNFS "$IP" > >(tee -a /var/log/pluto/Maxtor_openmss_EnableNFS.log) 2>&1
	Logging "Maxtor-openmss" $SEVERITY_NORMAL "EnableMaxtorNFS" "Enabled NFS on Maxtor with IP '$IP'"

	return 0
}
