#!/bin/bash

set -e

Debug=0

TargetHdd=
RootUUID=
SwapUUID=

GetConsole()
{
	exec &>/dev/tty1 </dev/tty1
	dmesg -n1
	chvt 1
}

GetHddToUse()
{
	local HddList Done Drive i Choice
	local -a Hdd

	HddList=$(fdisk -l|grep ^Disk |cut -d' ' -f2|cut -d: -sf1)
	if [[ "$Debug" == 1 ]]; then
		HddList="/dev/loop0"
	fi
	if [[ -z "$HddList" ]]; then # No hard drives
		echo "Error: No hard drives found"
		exit 1
	else
		echo "Hard drives in the system:"
		Done=0
		while [[ "$Done" == 0 ]]; do
			i=1
			for Drive in $HddList; do
				echo "$i. $Drive"
				Hdd[$i]="$Drive"
				((i++))
			done
			((i--))
			echo -n "Choose which drive to install the system on: "
			read Choice
			if [[ "$Choice" == *[^0-9]* || "$Choice" -lt 1 || "$Choice" -gt "$i" ]]; then
				echo "Invalid selection. Try again"
			else
				TargetHdd="${Hdd[$Choice]}"
				Done=1
			fi
		done
	fi

	echo "Chosen hdd: $TargetHdd"
	if [[ -z "$TargetHdd" ]]; then
		echo "Got out of loop with empty selection. This shouldn't be possible."
		exit 1
	fi
}

PartitionHdd()
{
set +e
	swapoff -a
	parted -s "$TargetHdd" -- mklabel msdos
	parted -s "$TargetHdd" -- mkpart primary ext2 0 -2GB
	parted -s "$TargetHdd" -- mkpart extended -2GB -1s
	parted -s "$TargetHdd" -- mkpart logical linux-swap -2GB -1s
	sleep 1
	blockdev --rereadpt "$TargetHdd"
	sleep 5
set -e
}

FormatPartitions()
{
	if [[ "$Debug" == 1 ]]; then
		return 0
	fi
	echo y|mkfs.ext3 "$TargetHdd"1
	mkswap "$TargetHdd"5

	blkid -c /etc/blkid.tab || :
	RootUUID=$(vol_id -u "$TargetHdd"1)
	SwapUUID=$(vol_id -u "$TargetHdd"5)
}

MountPartitions()
{
	mkdir -p /media/target
	mount "$TargetHdd"1 /media/target
}

ExtractArchive()
{
	echo "Extracting archive (this will take about 10 minutes)"
	cat /cdrom/lmce-image/linux-mce.tar.gz* | tar -C /media/target -zx --checkpoint=10000

	# Update the UUIDs
	rm /media/target/etc/blkid.tab || :
	blkid -c /media/target/etc/blkid.tab

	#Copy the fist run script
	cp /cdrom/lmce-image/firstrun /etc/rc2.d/S90firstrun
}

UnmountPartitions()
{
set +e
	umount /media/target
set -e
}

SetupFstab()
{
	local fstab_text="
# /etc/fstab: static file system information.
#
# <file system> <mount point>   <type>  <options>       <dump>  <pass>
proc            /proc           proc    defaults        0       0
#UUID=$RootUUID
${TargetHdd}1 /               ext3    defaults,errors=remount-ro 0       1
#UUID=$SwapUUID
${TargetHdd}5 none            swap    sw              0       0
/dev/cdrom        /media/cdrom0   udf,iso9660 user,noauto     0       0
"
	echo "$fstab_text" > /media/target/etc/fstab
}

InstallGrub()
{
	mkdir -p /media/target/proc
	mkdir -p /media/target/dev
	mkdir -p /media/target/sys
	mount --bind /dev/ /media/target/dev/
	mount -t proc -o bind /proc /media/target/proc
	grub-install --recheck --root-directory=/media/target "$TargetHdd"
	umount /media/target/dev/
	cp -r /dev/.static/dev/* /media/target/dev/

	sed -ir "s,root=UUID=.* ro quiet splash,root=${TargetHdd}1 ro quiet splash,g" /media/target/boot/grub/menu.lst
	sed -ir "s,root=UUID=.* ro single,root={$TargetHdd}1 ro single,g" /media/target/boot/grub/menu.lst
}

TargetCleanup()
{
	echo "" > /media/target/etc/iftab
	chroot /media/target update-initramfs
	#chroot /media/target update-grub
}

Reboot()
{
	shutdown -h now
}

GetConsole
GetHddToUse
PartitionHdd
FormatPartitions
MountPartitions
ExtractArchive
SetupFstab
InstallGrub
TargetCleanup
UnmountPartitions
Reboot
