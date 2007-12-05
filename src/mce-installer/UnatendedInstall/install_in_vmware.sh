#!/bin/bash
set -x
set -e

WORK_DIR="/var/plutobuild/vmware/"
VMWARE_DIR="${WORK_DIR}/Kubuntu"
VMWARE_DISK_IMAGE="${VMWARE_DIR}/Kubuntu-flat.vmdk"
VMWARE_TARGZ="${VMWARE_DIR}/linux-mce.tar.gz"
VMWARE_WORK_MACHINE="${VMWARE_DIR}/Kubuntu.vmx"
VMWARE_IP="192.168.124.128"


# Export display so vmware will run in X11

export DISPLAY=:1
screen -d -m -S X11-Display1 X -ac :1
sleep 2
DISPLAY=:1 kwin &


function decho {
	echo "$(date -R) $*"
}

function create_virtual_machine {
	decho "Creating virual machine"
	mkdir -p "$VMWARE_DIR"
	killall -9 vmplayer || :
	killall -9 vmware-vmx || :
	cp -r /var/Kubuntu/* "$VMWARE_DIR"
	decho "Finished creating virtual machine"
}

function start_virtual_machine {
	decho "Starting virtual machine"
	killall vmplayer || :
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

	## CACHE Kubuntu CD
	decho "Caching Kubuntu CD"
	scp /var/plutobuild/kubuntu-cd/*.deb root@"$VMWARE_IP":/usr/pluto/deb-cache
	decho "Finished Caching Kubuntu CD"
	
	## Build Packages.gz
	decho "Building Pacakges.gz on virtual machine"
	scp /usr/bin/dpkg-scanpackages root@"$VMWARE_IP":/usr/pluto/deb-cache
	ssh root@"$VMWARE_IP" "apt-get -y install dpkg-dev; cd /usr/pluto/deb-cache && ./dpkg-scanpackages -m . /dev/null > Packages && gzip -c Packages > Packages.gz && rm dpkg-scanpackages"
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

	
	while [[ "$(pidof vmware-vmx)" != "" ]] ;do
		sleep 5
	done
	decho "Finished installing in virtual machine"


}

function cleanup_filesystem {
	FILESYSTEM_ROOT="$1"
	if [[ "$FILESYSTEM_ROOT" == "" ]] ;then
		return
	fi

	## Remove xorg.conf cause it was build under vmware
	rm -f "${FILESYSTEM_ROOT}"/etc/X11/xorg.conf

	## Remove iftab
	rm -f "${FILESYSTEM_ROOT}"/etc/iftab

	## Create some dirs it they are not the
	mkdir -p "${FILESYSTEM_ROOT}"/dev 
	mkdir -p "${FILESYSTEM_ROOT}"/sys
	mkdir -p "${FILESYSTEM_ROOT}"/proc

	## Set AVWizardOverride to 1 so AVWizard Starts on the first boot
	if grep -q "^AVWizardOverride " ${FILESYSTEM_ROOT}/etc/pluto.conf ;then
		sed -i "s/^AVWizardOverride .*/AVWizardOverride = 1/g" "${FILESYSTEM_ROOT}/etc/pluto.conf"
	else
		echo "AVWizardOverride = 1" >> "${FILESYSTEM_ROOT}/etc/pluto.conf"
	fi

	## Set PK_Installation to 1
	if grep -q "^PK_Installation " ${FILESYSTEM_ROOT}/etc/pluto.conf ;then
		sed -i "s/^PK_Installation .*/PK_Installation = 1/g" "${FILESYSTEM_ROOT}/etc/pluto.conf"
	else
		echo "PK_Installation = 1" >> "${FILESYSTEM_ROOT}/etc/pluto.conf"
	fi

	## Remove the ssh key used for installation
	rm -f "${FILESYSTEM_ROOT}"/root/.ssh/authorized_keys

	COLUMNS=1024 chroot "${FILESYSTEM_ROOT}" dpkg -l | awk '/^ii/ {print $2}' >"$FILESYSTEM_ROOT/tmp/pkglist-hybrid.txt"

	local PkgNonGrata_Fixed
	PkgNonGrata_Fixed="
		asterisk-pluto mce-diskless-tools pluto-asterisk pluto-avwizard-sounds pluto-dhcpd-plugin pluto-mythtv-plugin
		pluto-orbiterinstaller pluto-skins-basic pluto-slimserver-plugin pluto-std-plugins pluto-vdr-plugin pluto-xine-plugin
		pluto-xml-data-plugin video-wizard-videos pluto-avwizard pluto-dcerouter pluto-sample-media mysql-server-5.0
		openoffice.org-core wine pluto-orbiter python-kde3
	"
	PkgNonGrata_Determined=$(Determine_PkgNonGrata "$FILESYSTEM_ROOT"/tmp/pkglist-{diskless,hybrid}.txt)
	rm -f "$FILESYSTEM_ROOT"/tmp/pkglist-{diskless,hybrid}.txt

	local Pkg
	for Pkg in $PkgNonGrata_Fixed $PkgNonGrata_Determined; do
		rm -f "${FILESYSTEM_ROOT}"/usr/pluto/deb-cache/"$Pkg"_*.deb
	done
	(cd "${FILESYSTEM_ROOT}"/usr/pluto/deb-cache && dpkg-scanpackages -m . /dev/null | tee Packages | gzip -c > Packages.gz)
}

function create_disk_image_from_flat {
	local PART_FILE="/dev/mapper/qemu_p"
	local LoopDev="$(losetup -f)"
	
	# Map partitions to loop device
	decho "Map virtual machine partitions to loop device"
	local Pattern="${VMWARE_DISK_IMAGE}p?([[:digit:]]+)[*[:space:]]+([[:digit:]]+)[[:space:]]+([[:digit:]]+)[[:space:]]+([[:digit:]]+)[[:space:]]+([[:xdigit:]]+)[[:space:]].*"
	local Partition Start End Blocks Type Rest
	dmsetup remove_all || :
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

	cleanup_filesystem "${VMWARE_MOUNT_DIR}"

	mkdir -p /var/www/DisklessImages
	mv "${VMWARE_MOUNT_DIR}"/usr/pluto/install/PlutoMD-i386.tar.bz2* /var/www/DisklessImages
	tar -C "${VMWARE_MOUNT_DIR}" --exclude=dev --exclude=proc -zc . | split --numeric-suffixes --bytes=2000m - "${VMWARE_TARGZ}_"

	umount "${VMWARE_MOUNT_DIR}"
	decho "Finish creating the tar.gz of / partition"

	# Unmap partitions from loop device
	decho "Unmap virtual machine partitions from loop device"
	local Dev
	for Dev in /dev/mapper/qemu_p*; do
		dmsetup remove "$(basename "$Dev")"
	done
	losetup -d "$LoopDev"
}

function create_disk_image_from_vmdk {
	sleep 30
	rm -f "${VMWARE_TARGZ}_"*

	/usr/bin/vmware-loop -q /var/plutobuild/vmware/Kubuntu/Kubuntu.vmdk 1 /dev/nbd0 &
	sleep 5

	VMWARE_MOUNT_DIR="${WORK_DIR}/mount"
	mkdir -p "${VMWARE_MOUNT_DIR}"
	mount /dev/nbd0 "${VMWARE_MOUNT_DIR}"
	cleanup_filesystem "${VMWARE_MOUNT_DIR}"


	mkdir -p /var/www/DisklessImages
	mv "${VMWARE_MOUNT_DIR}"/usr/pluto/install/PlutoMD-amd64.tar.bz2* /var/www/DisklessImages
	tar -C "${VMWARE_MOUNT_DIR}" --exclude=dev --exclude=proc -zc . | split --numeric-suffixes --bytes=2000m - "${VMWARE_TARGZ}_"

	umount "${VMWARE_MOUNT_DIR}"
	decho "Finish creating the tar.gz of / partition"

	sync 	
	killall vmware-loop
}

function Determine_PkgNonGrata()
{
	## Description:
	# This function determines the intersection of two or more string lists

	if (("$#" < 2)); then
		return
	fi
	
	local i
	local LeftFile="$1" RightFile ResultFile
	sort -u "$LeftFile" >"$LeftFile.sort"
	LeftFile="$LeftFile.sort"

	for ((i = 2; i <= "$#"; i++)); do
		RightFile="${!i}"
		sort -u "$RightFile" >"$RightFile.sort"
		RightFile="$RightFile.sort"

		ResultFile="/tmp/result-$i"
		diff -U -1 "$LeftFile" "$RightFile" | tail -n +4 | grep -v '^[+-]' >"$ResultFile"

		rm -f "$LeftFile" "$RightFile"
		LeftFile="$ResultFile"
	done
	cat "$ResultFile"
}

create_virtual_machine
start_virtual_machine
create_debcache_on_virtual_machine
copy_installer_on_virtual_machine
run_installer_on_virtual_machine
create_disk_image_from_flat

