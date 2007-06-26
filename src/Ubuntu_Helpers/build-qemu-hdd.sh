#!/bin/bash

TEMP_DIR=$(mktemp -d)

ImgFilename=qemu.img
HDDSize=$((10*1024)) # size in MB
HDDImg="$TEMP_DIR/$ImgFilename"
RootPartSize=$(((HDDSize-1)/1024)) # size in GB
PartLinks="$TEMP_DIR/partlinks"
PartPrefix="$PartLinks/${ImgFilename}_p"
DebootstrapDir="$TEMP_DIR/debootstrap"

dd if=/dev/zero of="$HDDImg" count="$HDDSize" bs=1M

parted -s "$HDDImg" -- mklabel msdos
parted -s "$HDDImg" -- mkpart primary ext2 0 "$RootPartSize"GB
parted -s "$HDDImg" -- mkpart extended "$RootPartSize"GB -1
parted -s "$HDDImg" -- mkpart logical linux-swap "$RootPartSize"GB -1

mkdir -p "$PartLinks"
/root/misc_utils/plosetup "$HDDImg" "$PartLinks"

mkfs.ext3 "${PartPrefix}1"

mkdir -p "$DebootstrapDir"
mount "${PartPrefix}1" "$DebootstrapDir"
debootstrap "feisty" "$DebootstrapDir" "http://ro.archive.ubuntu.com/ubuntu/"
umount "$DebootstrapDir"

/root/misc_utils/plosetup -c "$PartLinks"

mv "$HDDImg" /var/plutobuild/

rm -rf "$TEMP_DIR"
