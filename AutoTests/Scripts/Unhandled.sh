#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Config_Ops.sh

FK_Room=$(RunSQL "SELECT FK_Room FROM Device WHERE PK_Device='${PK_Device}'")
Q="SELECT PK_Device FROM Device JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE ImplementsDCE=1 AND FK_Room=$FK_Room"
Devs=$(RunSQL "$Q")

echo >> /Unhalded.log
echo "*LIST OF DEVICES THAT DIDN'T RESPOND WITH UNHANDLED*" >> /Unhalded.log
echo "====================================================" >> /Unhalded.log
echo "unhalded" >> /tmp/status.log
for dev in $Devs ; do
	resp=$(/usr/pluto/bin/MessageSend $DCERouter -r 0 $dev 1 99999)	
	if [[ "$resp" == 'RESP: Delivery failed' ]] ;then
                Q1="SELECT Description FROM Device WHERE PK_Device=$dev"
                DevName=$(RunSQL "$Q1")
                echo "Device #$dev/'$DevName' didn't respond with UNHANDLED: $resp" >> /Unhalded.log
	fi
done


