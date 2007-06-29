#!/bin/bash
set -x
set -e

WORK_DIR="/var/plutobuild/vmware/"
VMWARE_DIR="${WORK_DIR}/Kubuntu7.04"
VMWARE_DISK_IMAGE="${VMWARE_DIR}/Kubuntu7.04-flat.vmdk"
VMWARE_WORK_MACHINE="${VMWARE_DIR}/Kubuntu7.04.vmx"
VMWARE_IP="192.168.76.128"


function decho {
	echo "$(date -R) $*"
}

function create_virtual_machine {
	decho "Creating virual machine"
	mkdir -p "$VMWARE_DIR"
	cp -r /var/Kubuntu7.04/* "$VMWARE_DIR"
	decho "Finished creating virtual machine"
}

function start_virtual_machine {
	decho "Starting virtual machine"
	vmplayer "$VMWARE_WORK_MACHINE" &

	decho "Waiting for ssh connnection to virtual machine"
	while ! ssh root@"$VMWARE_IP" 'echo' ;do
		sleep 1
	done

	decho "Virtual machine ssh connection is active"
}

function create_debcache_on_virtual_machine {
	local ISO_DIR="$(mktemp -d)"
	local WMWARE_DIR="$(mktemp -d)"

	## Create deb-cache dir on vmware
	decho "Creating deb-cache directory on vmware"
	ssh root@"$VMWARE_IP" "mkdir -p /usr/pluto/deb-cache"

	## Cache CD1
	decho "Start Chaching CD1"
	mount -o loop /var/www/linuxmce-1.1-packages.iso "$ISO_DIR"
	scp "$ISO_DIR"/deb-cache/*.deb root@"$VMWARE_IP":/usr/pluto/deb-cache
	umount "$ISO_DIR"
	decho "Finish Caching CD1"

	## CACHE CD2
	decho "Start Caching CD2"
	mount -o loop /var/www/linuxmce-1.1-cache.iso "$ISO_DIR"
	scp "$ISO_DIR"/cachecd1-cache/*.deb root@"$VMWARE_IP":/usr/pluto/deb-cache
	umount "$ISO_DIR"
	decho "Finish Caching CD2"

	## Build Packages.gz
	decho "Building Pacakges.gz on virtual machine"
	scp /usr/bin/dpkg-scanpackages root@"$VMWARE_IP":/usr/pluto/deb-cache
	ssh root@"$VMWARE_IP" "cd /usr/pluto/deb-cache && ./dpkg-scanpackages . /dev/null > Packages && gzip -c Packages > Packages.gz && rm dpkg-scanpackages"
	decho "Finish building Packages.gz on virtual machine"


}

function copy_installer_on_virtual_machine {
	decho "Copying installer on virutal machine"
	ssh root@"$VMWARE_IP" "mkdir -p /usr/pluto/install"
	scp ./mce-installer-unattended/* root@"$VMWARE_IP":/usr/pluto/install
	decho "Finished copying installer on virtual machine"
}

function run_installer_on_virtual_machine {
	decho "Starting installer on virtual machine"
	ssh root@"$VMWARE_IP" "cd /usr/pluto/install && screen -d -m -S 'Install' ./mce-installer.sh"
	
	while [[ "$(pidof vmplayer)" != "" ]] ;do
		sleep 5
	done
	decho "Finished installing in virtual machine"


}

function create_disk_image {
	local PART_FILE="/dev/mapper/qemu_p"
	local LoopDev="$(losetup -f)"
	
	# Map partitions to loop device
	decho "Map virtual machine partitions to loop device"
	local Pattern="${VMWARE_DISK_IMAGE}p?([[:digit:]]+)[*[:space:]]+([[:digit:]]+)[[:space:]]+([[:digit:]]+)[[:space:]]+([[:digit:]]+)[[:space:]]+([[:xdigit:]]+)[[:space:]].*"
	local Partition Start End Blocks Type Rest
	losetup "$LoopDev" "${VMWARE_DISK_IMAGE}"
	modprobe dm-mod
	
	while read Partition Start End Blocks Type Rest; do
		if [[ " 82 83 " != *" $Type "* ]] ;then
			continue
		fi

		echo "0 $(( End - Start + 1 )) linear $LoopDev $Start" >"${PART_FILE}${Partition}.cf"
		dmsetup create "qemu_p${Partition}" "${PART_FILE}${Partition}.cf"
		rm -f "${PART_FILE}${Partition}.cf"
	done  < <(fdisk -lu "${VMWARE_DISK_IMAGE}" | sed -nre "s,${Pattern},\1 \2 \3 \4 \5,p")


	# Mount the / partition and create tar.gz
	decho "Creating tar.gz from virtual / partition"
	VMWARE_MOUNT_DIR="${WORK_DIR}/mount"
	mkdir -p "${VMWARE_MOUNT_DIR}"
	mount "${PART_FILE}1" "${VMWARE_MOUNT_DIR}"
	bash
	umount "${VMWARE_MOUNT_DIR}"


	# Unmap partitions from loop device
	decho "Unmap virtual machine partitions from loop device"
	local Dev
	for Dev in /dev/mapper/qemu_p*; do
		dmsetup remove "$(basename "$Dev")"
	done
	losetup -d "$LoopDev"


}

#create_virtual_machine
#start_virtual_machine
#create_debcache_on_virtual_machine
#copy_installer_on_virtual_machine
#run_installer_on_virtual_machine
create_disk_image
