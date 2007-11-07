#!/bin/bash

set -e

Debug=0
FromHdd=0
Upgrade=0
grep -q "install_from_hdd" /proc/cmdline && FromHdd="1"

if grep -q "noinst" /proc/cmdline; then
	clear >/dev/tty1
	echo "Installer bypassed" >/dev/tty1
	chvt 2
	exit 0
fi

TargetHdd=
RootUUID=
SwapUUID=

NukeFS()
{
	local List="$*" # "/,home,var /var,lib BasePath,Exception1,Exception2,..."
	local Item Path
	local BasePath ExceptionList
	local OldIFS
	local -a Exceptions

	for Item in $List; do
		Exceptions=(-false)
		BasePath="${Item%%,*}"
		ExceptionList="${Item#*,}"
		OldIFS="$IFS"
		IFS=","
		for Path in $ExceptionList; do
			Exceptions=("${Exceptions[@]}" -or -name "$Path")
		done
		IFS="$OldIFS"
		find "$BasePath" -mindepth 1 -maxdepth 1 '(' "${Exceptions[@]}" ')' -or -exec rm -rf '{}' ';'
	done
}

GetConsole()
{
	exec &>/dev/tty1 </dev/tty1
	dmesg -n1
	chvt 1
}

GetHddToUse()
{
	local HddList Done Drive i Choice DiskSize DiskDev
	local -a Hdd

	clear
	HddList=$(fdisk -l|grep ^Disk |cut -d' ' -f2-4|sed -r 's,[:,],,g; s, ,:,g')
	if [[ "$Debug" == 1 ]]; then
		HddList="/dev/loop0:0:GB"
	fi
	if [[ "$FromHdd" == 1 ]]; then
		for BootParm in $(</proc/cmdline); do
			if [[ "$BootParm" == "root="* ]]; then
				TargetHdd="${BootParm#root=}"
				TargetHdd="${TargetHdd//[0-9]}"
				break
			fi
		done
		if [[ -z "$TargetHdd" ]]; then
			echo "Error: Failed to determine current hard drive"
			exit 1
		fi
	elif [[ -z "$HddList" ]]; then # No hard drives
		echo "Error: No hard drives found"
		exit 1
	else
		Done=0
		mkdir -p /media/target
		for Drive in $HddList; do
				DiskDev="${Drive%%:*}"
				if mount "$DiskDev"1 /media/target; then
					if [[ -f /media/target/etc/pluto.conf ]]; then
						while [[ "$AnswerOK" -eq 0 ]]; do
							echo "Found an existing installation on drive '$DiskDev'"
							echo "Do you want to upgrade/reinstall keeping your settings?"
							echo -n "(Y/n)> "
							read Choice
							if [[ -z "$Choice" ]]; then
								Choice=Y
							fi
							if [[ "$Choice" == [YyNn] ]]; then
								AnswerOK=1
							fi
						done
						if [[ "$Choice" == [Yy] ]]; then
							Done=1
							Upgrade=1
							TargetHdd="$DiskDev"
						fi
					fi
					umount /media/target
				fi
		done
		clear

		while [[ "$Done" == 0 ]]; do
		echo "Hard drives in the system:"
			i=1
			for Drive in $HddList; do
				DiskDev="${Drive%%:*}"
				DiskSize="${Drive#*:}"
				set +e
				DiskDescription="$(hdparm -I $DiskDev | grep "Model Number:" | cut -d ':' -f2 | sed 's/^ *//g')"
				DiskSerial="$(hdparm -I $DiskDev | grep "Serial Number:" | cut -d ':' -f2 | sed 's/^ *//g')"
				set -e
				echo "$i. $DiskDev ($DiskDescription $DiskSerial ${DiskSize//:/ })"
				Hdd[$i]="$DiskDev"
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
	if [[ "$FromHdd" == 1 || "$Upgrade" == 1 ]]; then
		return
	fi
set +e
	swapoff -a
	parted -s "$TargetHdd" -- mklabel msdos
	parted -s "$TargetHdd" -- mkpart primary ext2 0 -12GB # root filesystem
	parted -s "$TargetHdd" -- mkpart extended -12GB -1s
	parted -s "$TargetHdd" -- mkpart logical linux-swap -12GB -10GB # swap
	parted -s "$TargetHdd" -- mkpart logical ext2 -10GB -1s # recovery system
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
	mkdir -p /media/target
	if [[ "$FromHdd" == 1 || "$Upgrade" == 1 ]] && mount "$TargetHdd"1 /media/target; then
		pushd /media/target &>/dev/null

		if [[ -d ./usr/pluto/diskless ]]; then
			pushd ./usr/pluto/diskless &>/dev/null
				while read MD; do
					NukeFS "./$MD",etc "./$MD/etc",pluto.conf
				done < <(find . -mindepth 1 -maxdepth 1 -type d)
			popd &>/dev/null

			mkdir .upgrade-diskless
			mv ./usr/pluto/diskless/* .upgrade-diskless/
		fi

		NukeFS .,home,var,etc,usr,.upgrade-diskless ./var,lib ./var/lib,mysql ./etc,pluto.conf,ssh,passwd,shadow,group ./usr,pluto ./usr/pluto,orbiter ./usr/pluto/orbiter,rooms,scenarios,users
			
		mkdir .upgrade-save
		find -mindepth 1 -maxdepth 1 -not -name '.upgrade-*' -exec mv '{}' .upgrade-save/ ';'
		popd &>/dev/null
		umount "$TargetHdd"1
	else
		echo y|mkfs.ext3 "$TargetHdd"1 # root filesystem
	fi
	if [[ "$FromHdd" != 1 ]]; then
		mkswap "$TargetHdd"5 # swap
		echo y|mkfs.ext3 "$TargetHdd"6 # recovery system
	fi

	blkid -w /etc/blkid.tab || :
	RootUUID=$(vol_id -u "$TargetHdd"1)
	SwapUUID=$(vol_id -u "$TargetHdd"5)
}

MountPartitions()
{
	mkdir -p /media/target /media/recovery
	mount "$TargetHdd"1 /media/target
	if [[ "$FromHdd" != 1 ]]; then
		mount "$TargetHdd"6 /media/recovery
	else
		mount -o bind / /media/recovery
	fi
}

CopyDVD()
{
	if [[ "$FromHdd" == "1" ]] ;then
		return
	fi
	echo "Copying DVD to hard drive"
	local DVDdir=$(mktemp -d)
	mount -t squashfs -o loop,ro /cdrom/casper/filesystem.squashfs "$DVDdir"
	cp -a "$DVDdir"/. /media/recovery/
	umount "$DVDdir"

	#Copy archives
	mkdir -p /media/recovery/archives/
	cp -a /cdrom/lmce-image/. /media/recovery/archives/

	#Copy the first run script
	cp /cdrom/lmce-image/firstboot /media/recovery/archives/

	#Copy demo videos, if any
	if [[ -d /cdrom/lmce-videos ]]; then
		mkdir -p /media/recovery/archives/lmce-videos
		cp -a /cdrom/lmce-videos/. /media/recovery/archives/lmce-videos/
	fi

	#Copy VIA archives
	if [[ -d /cdrom/via-archives ]]; then
		mkdir -p /media/recovery/archives/via-archives
		cp -a /cdrom/via-archives/. /media/recovery/archives/via-archives/
	fi

	local interfaces="
auto lo
iface lo inet loopback

auto eth0
iface eth0 inet dhcp

auto eth1
iface eth1 inet dhcp
"
	echo "$interfaces" >/media/recovery/etc/network/interfaces
}

ExtractArchive()
{
	echo "Extracting archive (this will take about 10 minutes)"
	cat /media/recovery/archives/linux-mce.tar.gz* | tar -C /media/target -zx --checkpoint=10000
	mkdir -p /media/target/etc/pluto
	touch /media/target/etc/pluto/install_cleandb

	# Update the UUIDs
	rm /media/target/etc/blkid.tab || :
	blkid -w /media/target/etc/blkid.tab

	#Copy the fist run script
	cp /media/recovery/archives/firstboot /media/target/etc/rc2.d/S90firstboot

	#Copy demo videos, if any
	if [[ -d /media/recovery/archives/lmce-videos ]]; then
		mkdir -p /media/target/home/public/data/videos/
		cp -a /media/recovery/archives/lmce-videos/. /media/target/home/public/data/videos/
	fi

	#Copy VIA archives
	if [[ -d /media/recovery/archives/via-archives ]]; then
		mkdir -p /media/target/usr/pluto/install/via/
		cp -a /media/recovery/archives/via-archives/. /media/target/usr/pluto/install/via/
	fi
}

NetworkSetup()
{
	# detect cards
	NCards=$(ip addr | grep -cF 'link/ether')
	if [[ "$NCards" -eq 1 ]]; then
		ExtIf="eth0"
		IntIf="eth0:0"
	else
		if host -W 1 www.google.com && ping -qc1 -I eth0 www.google.com &>/dev/null; then
			ExtIf="eth0"
			IntIf="eth1"
		else
			ExtIf="eth1"
			IntIf="eth0"
		fi
	fi

	echo "
auto lo
iface lo inet loopback

auto $ExtIf
iface $ExtIf inet dhcp

auto $IntIf
iface $IntIf inet static
	address 192.168.80.1
	netmask 255.255.255.0
" >/media/target/etc/network/interfaces
}

SetupFstab()
{
	mkdir -p /media/target/mnt/recovery

	local fstab_text="
# /etc/fstab: static file system information.
#
# <file system> <mount point>   <type>  <options>       <dump>  <pass>
proc            /proc           proc    defaults        0       0
#UUID=$RootUUID
${TargetHdd}1 /               ext3    defaults,errors=remount-ro 0       1
#UUID=$SwapUUID
${TargetHdd}5 none            swap    sw              0       0
${TargetHdd}6 /mnt/recovery   ext3    ro              0       0
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
	sed -ir "s,root=UUID=.* ro single,root=${TargetHdd}1 ro single,g" /media/target/boot/grub/menu.lst

	echo "
	title		System Recovery
	root		(hd0,5)
	kernel		/boot/vmlinuz-2.6.20-15-generic root=${TargetHdd}6 quiet install_from_hdd
	initrd		/boot/initrd.img-2.6.20-15-generic
	" >> /media/target/boot/grub/menu.lst
}

TargetCleanup()
{
	local NR ifnr ifname link ifmac rest line

	NR=0
	echo "# This file assigns persistent names to network interfaces.
# See iftab(5) for syntax.
" >/media/target/etc/iftab
	rm -f /media/target/etc/udev/rules.d/70-persistent-net.rules

	while read line; do
		if ((NR < 2)); then
			((NR++))
			continue
		fi
		if ((NR % 2 == 0)); then
			read ifnr ifname rest < <(echo "$line")
		else
			read link ifmac rest < <(echo "$line")
			echo "${ifname%:} mac $ifmac arp 1"
		fi
		((NR++))
	done < <(/sbin/ip l) >>/media/target/etc/iftab
	chroot /media/target update-initramfs -u
	#chroot /media/target update-grub

	if [[ -d /media/target/.upgrade-save ]]; then
		pushd /media/target/.upgrade-save &>/dev/null
		while read filepath; do
			filepath="${filepath#+}"
			filepath="${filepath%+}"
			directory="../$(dirname "$filepath")"
			mkdir -p "$directory"
			mv "$filepath" "$directory"
		done < <(find -not -type d -printf "+%p+\n")
		popd &>/dev/null
		rm -rf /media/target/.upgrade-save
	fi
}

UnmountPartitions()
{
set +e
	umount /media/target
	umount /media/recovery
set -e
}

Reboot()
{
	clear
	exec &>/dev/tty8 </dev/tty8
	clear
	chvt 8
	echo "**********************"
	echo "Installation complete."
	echo "**********************"
	reboot
}

GetConsole
GetHddToUse
PartitionHdd
FormatPartitions
MountPartitions
CopyDVD
ExtractArchive
SetupFstab
InstallGrub
NetworkSetup
TargetCleanup
UnmountPartitions
Reboot
