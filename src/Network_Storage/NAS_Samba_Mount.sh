#!/bin/bash

. /usr/pluto/bin/NAS_Common.sh

NAS_Init "samba" "$@"
NAS_GetData

Opts="password=$NAS_Password"
if [[ -n "$NAS_Username" ]]; then
	Opts="$Opts,username=$NAS_Username"
fi
NAS_Mount "smbfs" "//$NAS_IP_Address/$NAS_Share_Name" "/mnt/$NAS_Description" "$Opts"
