#!/bin/bash

. /usr/pluto/bin/LockUtils.sh
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

IP="$1"
PackageName="$2"

Q="SELECT PK_Device FROM Device WHERE IPAddress='$IP'"
Row="$(RunSQL "$Q")"
PK_Device="$(Field 1 "$Row")"

Q="SELECT COUNT(*) FROM Software WHERE FK_Device='$PK_Device' and Installation_status in ('Ins','Rem')"
Row="$(RunSQL "$Q")"
if [[ "$R" -ne 0 ]]; then
	Result="Some package yet is installing or removing"
	echo "$Result"
	exit 10
fi

Q="UPDATE Software SET Installation_status='Rem' WHERE FK_Device='$PK_Device' and PackageName='$PackageName'"
$(RunSQL "$Q")

LogFile=/var/log/pluto/remote_remove.log
date -R >>"$LogFile"

Result=""
WaitLock "RemoveNewDevice" "RemoveSoftware_Remote"
apt-get -y remove "$PackageName" 
RetCode="$?"
echo "($RetCode) apt-get -f remove $PackageName" >> $LogFile
if [[ "$RetCode" != 0 ]]; then
	Result="Removing failed"
fi
Unlock "InstallNewDevice" "InstallSoftware_Remote"

if [ -z "$Result" ]; then
	Q="UPDATE Software SET Installation_status='No' WHERE FK_Device='$PK_Device' and PackageName='$PackageName'"
else
	Q="UPDATE Software SET Installation_status='Yes' WHERE FK_Device='$PK_Device' and PackageName='$PackageName'"
fi
$(RunSQL "$Q")

[ -z "$Result" ] && Result="Removing complete" || Err="yes"

echo "$Result"
[ -n "$Err" ] && exit 10 || exit 0
