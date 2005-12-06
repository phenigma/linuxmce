#!/bin/bash

. /usr/pluto/bin/NAS_Common.sh

NAS_Init "samba" "$@"
NAS_GetData
NAS_Mount "smbfs" "//$NAS_IP_Address/$NAS_Share_Name" "/mnt/$NAS_Description"
