#!/bin/bash

## UPGRADE 2.0.0.40
## - change /usr/pluto/diskless/$IP to /usr/pluto/diskless/$PK_Device
## - change /tftpboot/$IP to /tftpboot/$PK_Device

if [[ -d /etc/diskless.conf ]] ;then
	exit 0
fi

DlPath="/usr/pluto/diskless"

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
        Q="SELECT PK_Device FROM Device WHERE IPaddress='$IPaddress' LIMIT 1"
        PK_Device=$( echo "$Q" | mysql pluto_main | grep -v '^PK_Device' )
        [[ -z "$PK_Device" ]] && continue

        ## Move the filesystem
        mv -f $DlPath/$IPaddress $DlPath/$PK_Device || /bin/true

        ## Move the tftpboot directories
        mv -f /tftpboot/$IPaddress /tftpboot/$PK_Device || /bin/true

        ## Mark to run diskless setup on the MD
        Q="UPDATE Device SET NeedConfigure='1' WHERE PK_Device='$PK_Device'"
        echo "$Q" | mysql pluto_main
done
