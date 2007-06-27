#!/bin/bash 

set -e
set -x

WORK_DIR="/var/plutobuild/qemu"
mkdir -p "$WORK_DIR"

HDD_SIZE="$(( 10240 / 2 ))" # MBytes
HDD_FILE="$WORK_DIR/qemu.img"
PART_DIR="$WORK_DIR/partlinks"
#PART_FILE="$PART_DIR/$(basename $HDD_FILE)_p"
PART_FILE="/dev/mapper/qemu_p"
DEBOOTSTRAP_DIR="$WORK_DIR/debootstrap"

LoopDev="$(losetup -f)"

function partition_links {
	local Pattern="${HDD_FILE}p?([[:digit:]]+)[*[:space:]]+([[:digit:]]+)[[:space:]]+([[:digit:]]+)[[:space:]]+([[:digit:]]+)[[:space:]]+([[:xdigit:]]+)[[:space:]].*"
	local line
	local Partition Start End Blocks Type Rest

	losetup "$LoopDev" "$HDD_FILE"

	modprobe dm-mod
	while read Partition Start End Blocks Type Rest; do
		if [[ " 82 83 " != *" $Type "* ]]; then
			continue
		fi

		## Setup device mapper devices
		echo "0 $Blocks linear $LoopDev $Start" >"$PART_FILE$Partition.cf"
		dmsetup create "qemu_p$Partition" "$PART_FILE$Partition.cf"
		rm -f "$PART_FILE$Partition.cf"
	done < <(fdisk -lu "$HDD_FILE" | sed -nre "s,$Pattern,\1 \2 \3 \4 \5,p")
}

function partition_clear {
	local Dev
	for Dev in /dev/mapper/qemu_p*; do
		dmsetup remove "$(basename "$Dev")"
	done
	losetup -d "$LoopDev"
}

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
	partition_links

	## Format the '/' and swap partitions of the harddrive
	mkfs.ext3 "${PART_FILE}1"
	mkswap "${PART_FILE}5"
}

function hdd_debootstrap {
	mkdir -p $DEBOOTSTRAP_DIR

	mount "${PART_FILE}1" "$DEBOOTSTRAP_DIR"
	debootstrap "feisty" "$DEBOOTSTRAP_DIR" "http://ro.archive.ubuntu.com/ubuntu/"
	umount "${PART_FILE}1"
}

function hdd_prepare_for_install {
	mount "${PART_FILE}1" "$DEBOOTSTRAP_DIR"
	# TODO: configure apt in chroot to skip authentication
	chroot "$DEBOOTSTRAP_DIR" apt-get -f -y install ssh
	umount "${PART_FILE}1"
}

function hdd_cleanup {
	partition_clear
}

hdd_create
hdd_debootstrap
#hdd_prepare_for_install
hdd_cleanup
