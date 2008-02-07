#!/bin/bash

. /etc/lmce-build/builder.conf
. /usr/local/lmce-build/common/logging.sh

set -x
set -e

DisplayMessage "*** STEP: Installing LinuxMCE in VmWare [ $1 ]"
trap 'Error "Undefined error in $0"' EXIT


WORK_DIR="${build_dir}/vmware/"
VMWARE_DIR="${WORK_DIR}/Kubuntu"
VMWARE_DISK_IMAGE="${VMWARE_DIR}/Kubuntu.vmdk"

if [[ -f "${VMWARE_DIR}/Kubuntu-flat.vmdk" ]] ;then
	VMWARE_DISK_IMAGE="${VMWARE_DIR}/Kubuntu-flat.vmdk"
fi

VMWARE_TARGZ="${VMWARE_DIR}/linux-mce.tar.gz"
VMWARE_WORK_MACHINE="${VMWARE_DIR}/Kubuntu.vmx"
VMWARE_IP="vmware"


# Export display so vmware will run in X11
export DISPLAY=:1
screen -d -m -S X11-Display1 X -ac :1
sleep 2
DISPLAY=:1 blackbox &

function create_virtual_machine {
	DisplayMessage "Creating virual machine"
	killall -9 vmplayer || :
	killall -9 vmware-vmx || :
	
	if [[ "$VMWARE_DIR" != "" ]] ;then
		rm -rf "$VMWARE_DIR"/*
	fi
	mkdir -p "$VMWARE_DIR"

	cp -r /var/Kubuntu/* "$VMWARE_DIR"
	DisplayMessage "Finished creating virtual machine"
}

function start_virtual_machine {
	DisplayMessage "Starting virtual machine"
	killall vmplayer || :
	vmplayer "$VMWARE_WORK_MACHINE" &

	local wait_time=$(( 60 * 30 )) # time in second to wait for vmware to start
	local start_time=$(date +%s)

	DisplayMessage "Waiting for ssh connnection to virtual machine"
	while ! ssh -i /etc/lmce-build/builder.key root@"$VMWARE_IP" 'echo' ;do
		if [[ $(( start_time + wait_time )) -lt $(date +%s) ]] ;then
			Error "VmVare failed to start in less then $wait_time seconds."
		fi
		sleep 1
	done

	DisplayMessage "Virtual machine ssh connection is active"
}

function create_debcache_on_virtual_machine {
	local ISO_DIR="$(mktemp -d)"
	local WMWARE_DIR="$(mktemp -d)"

	## Create deb-cache dir on vmware
	DisplayMessage "Creating deb-cache directory on vmware"
	ssh -i /etc/lmce-build/builder.key root@"$VMWARE_IP" "mkdir -p /usr/pluto/deb-cache"

	## Cache CD1
	DisplayMessage "Start Chaching CD1"
	mount -o loop "${local_mirror_dir}/LinuxMCE-CD1.iso" "$ISO_DIR"
	scp -i /etc/lmce-build/builder.key "$ISO_DIR"/deb-cache/*.deb root@"$VMWARE_IP":/usr/pluto/deb-cache
	umount "$ISO_DIR"
	DisplayMessage "Finish Caching CD1"

	## CACHE CD2
	DisplayMessage "Start Caching CD2"
	mount -o loop "${local_mirror_dir}/LinuxMCE-CD2.iso" "$ISO_DIR"
	scp -i /etc/lmce-build/builder.key "$ISO_DIR"/cachecd1-cache/*.deb root@"$VMWARE_IP":/usr/pluto/deb-cache
	umount "$ISO_DIR"
	DisplayMessage "Finish Caching CD2"

	## CACHE Kubuntu CD
	DisplayMessage "Caching Kubuntu CD"
	scp -i /etc/lmce-build/builder.key ${build_dir}/kubuntu-cd/*.deb root@"$VMWARE_IP":/usr/pluto/deb-cache
	DisplayMessage "Finished Caching Kubuntu CD"
	
	## Build Packages.gz
	DisplayMessage "Building Pacakges.gz on virtual machine"
	scp -i /etc/lmce-build/builder.key /usr/bin/dpkg-scanpackages root@"$VMWARE_IP":/usr/pluto/deb-cache
	ssh -i /etc/lmce-build/builder.key root@"$VMWARE_IP" "apt-get -y install dpkg-dev; cd /usr/pluto/deb-cache && ./dpkg-scanpackages -m . /dev/null > Packages && gzip -c Packages > Packages.gz && rm dpkg-scanpackages"
	DisplayMessage "Finish building Packages.gz on virtual machine"
}

function copy_installer_on_virtual_machine {
	DisplayMessage "Copying installer on virutal machine"
	ssh -i /etc/lmce-build/builder.key root@"$VMWARE_IP" "mkdir -p /usr/pluto/install"
	scp -i /etc/lmce-build/builder.key ./mce-installer-unattended/* root@"$VMWARE_IP":/usr/pluto/install
	DisplayMessage "Finished copying installer on virtual machine"
}

function run_installer_on_virtual_machine {
	DisplayMessage "Starting installer on virtual machine"
	ssh -i /etc/lmce-build/builder.key root@"$VMWARE_IP" "cd /usr/pluto/install && screen -d -m -S 'Install' ./mce-installer.sh"

	local wait_time=$(( 60 * 60 * 4 ))
	local start_time=$( date +%s )	
	while [[ "$(pidof vmware-vmx)" != "" ]] ;do
		sleep 5
		if [[ $(( start_time + wait_time )) -lt $( date +%s ) ]] ;then
			#Error "VmWare failed to finish installing in less than $wait_time seconds."
			echo "Please connect to the builder as vmware didn't finish installing for over $wait_time seconds" | mail -s "$mail_subject_prefix Vmware install takes to long to finish" razvan.g@plutohome.com
			read
		fi
	done
	DisplayMessage "Finished installing in virtual machine"
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
		asterisk-pluto mce-diskless-tools pluto-asterisk pluto-avwizard-sounds pluto-dhcpd-plugin 
		pluto-orbiterinstaller pluto-skins-basic pluto-std-plugins pluto-xine-plugin
		pluto-xml-data-plugin video-wizard-videos pluto-avwizard pluto-dcerouter pluto-sample-media mysql-server-5.0
		openoffice.org-core wine pluto-orbiter python-kde3 pluto-default-tftpboot
	"
	PkgNonGrata_Determined=$(Determine_PkgNonGrata "$FILESYSTEM_ROOT"/tmp/pkglist-{diskless,hybrid}.txt)
	rm -f "$FILESYSTEM_ROOT"/tmp/pkglist-{diskless,hybrid}.txt

	if [[ "$RemovePkgNonGrata" != "no" ]] ;then
		local Pkg
		for Pkg in $PkgNonGrata_Fixed $PkgNonGrata_Determined; do
			rm -f "${FILESYSTEM_ROOT}"/usr/pluto/deb-cache/"$Pkg"_*.deb
		done
	fi
	(cd "${FILESYSTEM_ROOT}"/usr/pluto/deb-cache && dpkg-scanpackages -m . /dev/null | tee Packages | gzip -c > Packages.gz)
}

function create_disk_image_from_flat {
	rm -f "${VMWARE_TARGZ}_"*

	local PART_FILE="/dev/mapper/qemu_p"
	local LoopDev="$(losetup -f)"
	
	# Map partitions to loop device
	DisplayMessage "Map virtual machine partitions to loop device"
	local Pattern="${VMWARE_DISK_IMAGE}p?([[:digit:]]+)[*[:space:]]+([[:digit:]]+)[[:space:]]+([[:digit:]]+)[[:space:]]+([[:digit:]]+)[[:space:]+]+([[:xdigit:]]+)[[:space:]].*"
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
	DisplayMessage "Creating tar.gz from virtual / partition"
	VMWARE_MOUNT_DIR="${WORK_DIR}/mount"
	mkdir -p "${VMWARE_MOUNT_DIR}"
	mount "${PART_FILE}1" "${VMWARE_MOUNT_DIR}"

	cleanup_filesystem "${VMWARE_MOUNT_DIR}"

	mkdir -p "${build_dir}/DisklessSync/${arch}"
	mv "${VMWARE_MOUNT_DIR}"/usr/pluto/install/PlutoMD-${arch}.tar.bz2 "${build_dir}/DisklessSync/${arch}/"
	cp "${VMWARE_MOUNT_DIR}"/var/log/mce-installer-*.log "/var/log/"

	tar -C "${VMWARE_MOUNT_DIR}" --exclude=dev --exclude=proc -zc . | split --numeric-suffixes --bytes=2000m - "${VMWARE_TARGZ}_"

	umount "${VMWARE_MOUNT_DIR}"
	DisplayMessage "Finish creating the tar.gz of / partition"

	# Unmap partitions from loop device
	DisplayMessage "Unmap virtual machine partitions from loop device"
	local Dev
	for Dev in /dev/mapper/qemu_p*; do
		dmsetup remove "$(basename "$Dev")"
	done
	losetup -d "$LoopDev"
}

function create_disk_image_from_vmdk {
	sleep 30
	rm -f "${VMWARE_TARGZ}_"*

	/usr/bin/vmware-loop -q ${build_dir}/vmware/Kubuntu/Kubuntu.vmdk 1 /dev/nbd0 &
	sleep 5

	VMWARE_MOUNT_DIR="${WORK_DIR}/mount"
	mkdir -p "${VMWARE_MOUNT_DIR}"
	mount /dev/nbd0 "${VMWARE_MOUNT_DIR}"
	cleanup_filesystem "${VMWARE_MOUNT_DIR}"


	mkdir -p "${build_dir}/DisklessSync/${arch}"
	mv "${VMWARE_MOUNT_DIR}"/usr/pluto/install/PlutoMD-${arch}.tar.bz2 ${build_dir}/DisklessSync/${arch}/
	tar -C "${VMWARE_MOUNT_DIR}" --exclude=dev --exclude=proc -zc . | split --numeric-suffixes --bytes=2000m - "${VMWARE_TARGZ}_"

	umount "${VMWARE_MOUNT_DIR}"
	DisplayMessage "Finish creating the tar.gz of / partition"

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

RemovePkgNonGrata='yes'
if [[ "$1" == "big" ]] ;then
	RemovePkgNonGrata='no'	
fi
create_virtual_machine
start_virtual_machine
create_debcache_on_virtual_machine
copy_installer_on_virtual_machine
run_installer_on_virtual_machine

if [[ -f "${VMWARE_DIR}/Kubuntu-flat.vmdk" ]] ;then
	create_disk_image_from_flat
else
	create_disk_image_from_vmdk
fi

trap - EXIT
