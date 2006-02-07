#!/bin/bash

. /usr/pluto/bin/Network_Parameters.sh
. /usr/pluto/bin/Config_Ops.sh

DlDir="/usr/pluto/diskless"

Vars="CORE_INTERNAL_ADDRESS INTERNAL_SUBNET INTERNAL_SUBNET_MASK MOON_ADDRESS DYNAMIC_IP_RANGE KERNEL_VERSION MOON_HOSTS MOON_IP NOBOOT_ENTRIES"
INTERNAL_SUBNET_MASK="$IntNetmask"
CORE_INTERNAL_ADDRESS="$IntIP"

for i in 1 2 3 4; do
	IPDigit=$(echo $CORE_INTERNAL_ADDRESS | cut -d. -f$i)
	MaskDigit=$(echo $INTERNAL_SUBNET_MASK | cut -d. -f$i)
	NetDigit=$(($IPDigit & $MaskDigit))
	INTERNAL_SUBNET="$INTERNAL_SUBNET$Dot$NetDigit" && Dot="."
done
				
Q="
    SELECT PK_Device, IPaddress
    FROM Device
    JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
    WHERE PK_DeviceTemplate=28 AND FK_Device_ControlledVia IS NULL
"
R=$(RunSQL "$Q")

cp /usr/pluto/templates/exports.tmpl /etc/exports.$$

for Client in $R; do
        PK_Device=$(Field 1 "$Client")
        IP=$(Field 2 "$Client")

        if [[ -z "$IP" ]]; then
                echo "*** WARNING *** Device $PK_Device is not associated to any Ip address"
		continue
        fi

	
	DlPath="$DlDir/$IP"

	Q="
	    SELECT PK_Device
	    FROM Device_DeviceData
	    JOIN Device ON PK_Device=FK_Device
	    JOIN DeviceTemplate ON PK_DeviceTemplate=FK_DeviceTemplate
	    WHERE FK_DeviceTemplate=28 AND FK_DeviceData=9 AND IK_DeviceData='1' AND PK_Device='$PK_Device'
	"
	Diskless=$(RunSQL "$Q")

	if [ -n "$Diskless" ]; then
		echo "# Exporting nfs root for ip $IP"
		echo "$DlPath $IP/255.255.255.255(rw,no_root_squash,no_all_squash,sync)" >>/etc/exports.$$
	fi
done

ReplaceVars /etc/exports.$$

mv /etc/exports.$$ /etc/exports

if ! /sbin/showmount -e localhost &>/dev/null; then
	/etc/init.d/nfs-common start
	/etc/init.d/nfs-kernel-server start
fi
/usr/sbin/exportfs -ra		
