#!/bin/bash

. /usr/pluto/bin/NAS_Common.sh

NFS_Options="tcp,intr,rsize=8192,wsize=8192,retrans=10,timeo=50"

NAS_Init "nfs" "$@"
NAS_GetData
NAS_Mount "nfs" "$NAS_IP_Address:$NAS_Share_Name" "/mnt/$NAS_Description" "$NFS_Options"
