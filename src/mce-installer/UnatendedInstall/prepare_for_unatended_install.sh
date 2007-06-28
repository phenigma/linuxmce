#!/bin/bash
set -x
set -e

VMWARE_DIR="/var/plutobuild/vmware/Kubuntu7.04"
VMWARE_DISK_IMAGE="${VMWARE_DIR}/Kubuntu7.04.vmdk"
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

}

function copy_installer_on_virtual_machine {
	decho "Copying installer on virutal machine"
	ssh root@"$VMWARE_IP" "mkdir -p /usr/pluto/install"

	scp ./mce-installer-unattended/* root@"$VMWARE_IP":/usr/pluto/install

	decho "Finished copying installer on virtual machine"
}

#create_virtual_machine
#start_virtual_machine
#create_debcache_on_virtual_machine
copy_installer_on_virtual_machine
