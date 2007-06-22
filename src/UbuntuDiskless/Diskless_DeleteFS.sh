#!/bin/bash

. /usr/pluto/bin/LockUtils.sh
. /usr/pluto/bin/SQL_Ops.sh

while ! TryLock "Diskless_Setup" "Diskless_DeleteFS"; do
	sleep 1
done
trap 'Unlock "Diskless_Setup" "Diskless_DeleteFS"' EXIT

IP="$1"
MAC="$2"
lcdMAC=$(echo ${MAC//:/-} | tr 'A-Z' 'a-z')

if [[ -z "$IP" ]]; then
	echo "Error: Empty IP"
	exit
fi

Q="SELECT PK_Device FROM Device WHERE IPaddress LIKE '$IP' LIMIT 1"
PK_Device=$(RunSQL "$Q")

echo "Deleting MD files. MAC=$MAC IP=$IP ID=$PK_Device"

echo "Deleting SSH host key from Core"
sed -i "/$IP/ d" /root/.ssh/known_hosts &>/dev/null

echo "Filesystem"
rm -rf /usr/pluto/diskless/"$PK_Device"

echo "TFTP"
rm -rf /tftpboot/"$PK_Device"
rm -rf /tftpboot/pxelinux.cfg/"01-$lcdMAC"
