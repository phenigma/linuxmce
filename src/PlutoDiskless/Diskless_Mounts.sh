#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Section_Ops.sh
. /usr/pluto/bin/Network_Parameters.sh


CORE_INTERNAL_ADDRESS="$IntIP"

Q="
    SELECT
	PK_Device,
	IPaddress
    FROM
	Device
	JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
    WHERE
	PK_DeviceTemplate=28
	AND
	FK_Device_ControlledVia IS NULL
"
DisklessMDs=$(RunSQL "$Q")

DisklessFileSystemsLocation="/usr/pluto/diskless"

for Client in $DisklessMDs; do
	DisklessMD_ID=$(Field 1 "$Client")
	DisklessMD_IP=$(Field 2 "$Client")

	if [[ -z "$DisklessMD_IP" ]]; then
		echo "*** WARNING *** Diskless MD $DisklessMD_ID is not associated to any Ip address"
		continue
	fi

	DisklessMD_Root="$DisklessFileSystemsLocation/$DisklessMD_IP"

	Q="
	    SELECT PK_Device
	    FROM Device_DeviceData
	    JOIN Device ON PK_Device=FK_Device
	    JOIN DeviceTemplate ON PK_DeviceTemplate=FK_DeviceTemplate
	    WHERE FK_DeviceTemplate=28 AND FK_DeviceData=9 AND IK_DeviceData='1' AND PK_Device='$DisklessMD_ID'
	"
	IsDiskless=$(RunSQL "$Q")

	
	DisklessMD_Fstab="$DisklessFileSystemsLocation/$DisklessMD_IP/etc/fstab"
	
	if [ -n "$IsDiskless" ]; then

		## Check for a DisklessSpecific secion, if it doesn't exist that recreate the whole file
		if [[ $(CheckSectionExists "$DisklessMD_Fstab" "DisklessSpecific") == "false" ]]; then
			rm -f $DisklessMD_Fstab 2>/dev/null
			
			Fstab_DisklessSpecific="${Fstab_DisklessSpecific}proc		/proc           proc    defaults                0       0\n"
			Fstab_DisklessSpecific="${Fstab_DisklessSpecific}/dev/fd0	/floppy		auto    user,noauto             0       0\n"
			Fstab_DisklessSpecific="${Fstab_DisklessSpecific}/dev/cdrom	/cdrom          iso9660 ro,user,noauto          0       0\n"
			Fstab_DisklessSpecific="${Fstab_DisklessSpecific}sysfs		/sys            sysfs   defaults                0       0\n"
		
			PopulateSection "$DisklessMD_Fstab" "DisklessSpecific" "$Fstab_DisklessSpecific"
		fi

		## Regenerate the DisklessSecific section
		
		Fstab_DisklessGeneric="${Fstab_DisklessGeneric}$CORE_INTERNAL_ADDRESS:/usr/pluto/diskless/$DisklessMD_IP	/ 			nfs intr,tcp,rsize=8192,wsize=8192,retrans=10,timeo=50 	1 1\n"
		Fstab_DisklessGeneric="${Fstab_DisklessGeneric}$CORE_INTERNAL_ADDRESS:/home 					/home 			nfs intr,tcp,rsize=8192,wsize=8192,retrans=10,timeo=50 	1 1\n"
		Fstab_DisklessGeneric="${Fstab_DisklessGeneric}$CORE_INTERNAL_ADDRESS:/usr/pluto/orbiter 			/usr/pluto/orbiter 	nfs intr,tcp,rsize=8192,wsize=8192,retrans=10,timeo=50 	1 1\n"
		Fstab_DisklessGeneric="${Fstab_DisklessGeneric}$CORE_INTERNAL_ADDRESS:/usr/pluto/keys				/usr/pluto/keys 	nfs intr,tcp,rsize=8192,wsize=8192,retrans=10,timeo=50 	1 1\n"
		Fstab_DisklessGeneric="${Fstab_DisklessGeneric}$CORE_INTERNAL_ADDRESS:/usr/pluto/deb-cache 			/usr/pluto/deb-cache 	nfs intr,tcp,rsize=8192,wsize=8192,retrans=10,timeo=50 	1 1\n"
		Fstab_DisklessGeneric="${Fstab_DisklessGeneric}$CORE_INTERNAL_ADDRESS:/usr/pluto/var/Updates 			/usr/pluto/var/Updates 	nfs intr,tcp 						1 1\n"

		PopulateSection "$DisklessMD_Fstab" "DisklessGeneric" "$Fstab_DisklessGeneric"
	fi
done

