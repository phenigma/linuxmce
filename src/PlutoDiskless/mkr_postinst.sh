#!/bin/bash

## UPGRADE 2.0.0.40
## - change /usr/pluto/diskless/$IP to /usr/pluto/diskless/$PK_Device
## - change /tftpboot/$IP to /tftpboot/$PK_Device

. /usr/pluto/bin/SQL_Ops.sh

DlPath="/usr/pluto/diskless"

ChangesToBeMade='n'
for directory in $( ls $DlPath ) ;do
	
	## Test to see if is a directory
	[[ -d $DlPath/$directory ]] || continue
	
	## Test to see if looks like an ip address
	IPaddress=
	if echo $directory | grep -q '[0-9]*.[0-9]*.[0-9]*.[0-9]*$' ;then
		IPaddress="$directory"
	fi
	[[ -z "$IPaddress" ]] && continue

	## Get the PK_Device associated with that ip address
	Q="SELECT PK_Device FROM Device WHERE IPaddress='$IPaddress'"
	PK_Device=$(RunSQL "$Q")
	[[ -z "$PK_Device" ]] && continue

	## Move the filesystem
	mv -f $DlPath/$IPaddress $DlPath/$PK_Device

	## Mark to run diskless setup on the MD
        Q="UPDATE Device SET NeedConfigure='1' WHERE PK_Device='$PK_Device'"
	RunSQL "$Q"
			
	ChangesToBeMade='y'
done

if [[ $ChangesToBeMade == 'y' ]] ;then
        /usr/pluto/bin/Diskless_Setup.sh --skiplock
        /usr/pluto/bin/DHCP_config.sh
        /usr/pluto/bin/Diskless_ExportsNFS.sh				
fi
