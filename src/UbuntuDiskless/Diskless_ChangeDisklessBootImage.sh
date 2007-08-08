#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh 2>/dev/null
. /usr/pluto/bin/Utils.sh 2>/dev/null
. /usr/pluto/bin/Network_Parameters.sh 2>/dev/null

Moon_DeviceID="$1"
ImageName="$2"

DEVICEDATA_DisklessImages=258

Moon_DisklessImages=$(GetDeviceData "$Moon_DeviceID" "$DEVICEDATA_DisklessImages")

Q="SELECT MACAddress FROM Device WHERE PK_Device='$Moon_DeviceID'"
Moon_MAC=$(RunSQL "$Q")

if [[ -z "$Moon_MAC" ]]; then
	exit 1
fi

NewBootName=
for line in $Moon_DisklessImages; do
	NameKernel="${line%%=*}"
	if [[ "$NameKernel" == *-* ]]; then
		Name="${NameKernel%%-*}"
		Kernel="${NameKernel#*-}"
	else
		Name="$NameKernel"
		Kernel=
	fi
	[[ -z "$Kernel" ]] && continue
	Value="${line#*=}"
	if [[ "$Name" == "$ImageName" ]]; then
		NewBootName="$Name"
		break
	fi
done

if [[ -z "$NewBootName" ]]; then
	exit 1
fi

BootConf="${BootConf}DEFAULT Pluto\n"
BootConf="${BootConf}LABEL Pluto\n"
BootConf="${BootConf}KERNEL ${Moon_DeviceID}/$NewBootName/vmlinuz\n"
BootConf="${BootConf}APPEND initrd=${Moon_DeviceID}/$NewBootName/initrd.img ramdisk=10240 rw root=/dev/nfs boot=nfs nfsroot=${IntIP}:/usr/pluto/diskless/${Moon_DeviceID}/$NewBootName ${BootParams_Extra}\n"

Moon_BootConfFile="/tftpboot/pxelinux.cfg/01-$(echo ${Moon_MAC//:/-} | tr 'A-Z' 'a-z')"
echo -e "$BootConf" > "$Moon_BootConfFile"
