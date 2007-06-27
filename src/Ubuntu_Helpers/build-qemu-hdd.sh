#!/bin/bash 

set -e
set -x

WORK_DIR="/var/plutobuild/qemu"
mkdir -p "$WORK_DIR"

HDD_SIZE="$(( 10240 / 2 ))" # MBytes
HDD_FILE="$WORK_DIR/qemu.img"
PART_DIR="$WORK_DIR/partlinks"
PART_FILE="$PART_DIR/$(basename $HDD_FILE)_p"


function hdd_create {
	local ROOT_PARTITION_SIZE="$((($HDD_SIZE/1024)-1))" # GBytes

	## Generate a HDD_SIZE filled with 0 bytes
	dd of="$HDD_FILE" bs=1M count=0 seek="$HDD_SIZE"
#	dd if=/dev/zero of="$HDD_FILE" bs=1M count="$HDD_SIZE"
	
	## Create partitions on the new generated hdd file
	parted -s "$HDD_FILE" -- mklabel msdos
	parted -s "$HDD_FILE" -- mkpart primary ext2 0 "$ROOT_PARTITION_SIZE"GB
	parted -s "$HDD_FILE" -- mkpart extended "$ROOT_PARTITION_SIZE"GB -1
	parted -s "$HDD_FILE" -- mkpart logical linux-swap "$ROOT_PARTITION_SIZE"GB -1

	## Generate the loopback devices for all the partitions of the harddrive
	mkdir -p "$PART_DIR"
	/root/misc_utils/plosetup "$HDD_FILE" "$PART_DIR" "82 83 fd"

	## Fromat the '/' partition of the harddrive
	mkfs.ext3 "${PART_FILE}1"
	mkswap "${PART_FILE}5"
}

function hdd_debootstrap {
	local DEBOOTRAP_DIR="$WORK_DIR/debootstrap"
	mkdir -p $DEBOOTRAP_DIR

	mount "${PART_FILE}1" "$DEBOOTRAP_DIR"
	debootstrap "feisty" "$DEBOOTRAP_DIR" "http://ro.archive.ubuntu.com/ubuntu/"
	umount "${PART_FILE}1"
}

function hdd_prepare_for_install {
	mount "${PART_FILE}1" "$DEBOOTRAP_DIR"
	chroot "${PART_FILE}1" apt-get -f -y install sshd
	umount "${PART_FILE}1"
}

function hdd_cleanup {
	/root/misc_utils/plosetup -c "$PART_DIR"
}

hdd_create
hdd_debootstrap
hdd_prepare_for_install
hdd_cleanup
