#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh

[ -n "$MySqlPassword" ] && Pass="-p'$MySqlPassword'"

mkdir -p /home/backup
mysqldump -AQ --opt -u "$MySqlUser" $Pass >/home/backup/entire_database.sql

Q="SELECT IPaddress
FROM Device
JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
WHERE PK_DeviceTemplate=28 AND FK_Device_ControlledVia IS NULL AND MACaddress IS NOT NULL AND IPaddress IS NOT NULL"
R=$(RunSQL "$Q")

DlD="/usr/pluto/diskless"
for IP in $R; do
	cp "$DlD/$IP/etc/pluto.conf" "/home/backup/pluto.conf-$IP"
done
cp "/etc/pluto.conf" "/home/backup/pluto.conf-Core"
