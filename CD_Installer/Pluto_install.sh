#!/bin/bash

echo "Starting Pluto installation"
echo

dmesg -n 1 &>/dev/null

echo "Mounting proc filesystem... "
mkdir -p /proc &>/dev/null
mount -t proc proc /proc &>/dev/null

echo -n "Loading RAID drivers (ft3xx)... "
insmod -f /lib/modules/ft3xx.o &>/dev/null && echo OK || echo Failed

ok=0
while [ "$ok" -ne 1 ]; do
	echo "Searching for the installation CDROM (assumption: first found CDROM unit)"
	for i in /proc/ide/hd[a-h]/media; do
		media=`cat $i`
		device_long=`dirname $i`
		device_short=`basename $device_long`
		if [ "$media" == "cdrom" ]; then
			echo "Found CDROM: /dev/$device_short"
			cdrom="/dev/$device_short"
			ok=1
		fi
	done
	
	if [ -z "$cdrom" ]; then
		echo "Can't find the CDROM you are using to boot this from"
		echo "If you are booting from a network or some other device, we don't support that yet"
		read
	fi
done

echo -n "Mounting CDROM ($cdrom) in /cdrom ... "
mkdir -p /cdrom &>/dev/null
mount $cdrom /cdrom &>/dev/null && echo OK || echo Failed

while ! fdisk -l | grep '^Disk' &>/dev/null; do
	echo "Sorry, we cannot install. We did not find any hard drives"
	read -n 1 -s
done

ok=0
while [ "$ok" -ne 1 ]; do
	disks=`fdisk -l | grep '^Disk'`
	count=`fdisk -l | grep -c '^Disk'`

	echo
	echo $disks | awk '{ print NR".", $0 }'

	echo
	echo "Which drive do you want to install to?"
	echo "Drive 1 will be used by default."
	echo -n "Enter the drive number or \"y\" to accept the defaults: "

	read answer

	if [ "$answer" == "y" -o "$answer" == "Y" ]; then
		drivenum=1
	else
		drivenum="$answer"
	fi

	if [ "$drivenum" -ge 1 -a "$drivenum" -le "$count" ] &>/dev/null; then
		ok=1
	fi
done

device=`echo $disks | awk 'NR == '$drivenum' { printf "%s",$2 }' | sed 's/:$//'`

ok=0
while [ "$ok" -ne 1 ]; do
	echo
	echo "We are using disk \"$device\""
	echo
	echo Following are the partitions on the disk:

	partitions=`fdisk -l $device | grep ^/dev`
	count=`fdisk -l $device | grep -c ^/dev`

	echo "$partitions"
	echo

	echo Type \"f\" to run fdisk
	echo Type \"c\" to run cfdisk
	echo Type a partition number to install on this partition
	echo Type \"y\" to have Pluto automatically set partitions for you
	echo '*** WARNING ***' Using the defaults means all data on the disk will be erased
	echo -n "Choice: "
	read answer

	if [ "$answer" == "f" -o "$answer" == "F" ]; then
		fdisk $device
	elif [ "$answer" == "c" -o "$answer" == "F" ]; then
		cfdisk $device
	elif [ "$answer" == "y" -o "$answer" == "Y" ]; then
		userpartition="$device"1
		autorepartition=1
		ok=1
	elif [ "$answer" -ge 1 -a "$answer" -le "$count" ] &>/dev/null; then
		userpartition=`fdisk -l $device | grep ^/dev | awk "NR == $answer"' { printf "%s",$1 }'`
		ok=1
	fi
done

echo
if [ ! -z "$autorepartition" ]; then
	echo -n "Autorepartition: $userpartition (currently standardizing on a single partition system that takes the entire disk)... "
	#echo TODO: erase partitions and repartition like joe did
	fdisk $device < /cdrom/Config/fdisk.standard-partition &>/dev/null && echo OK || echo Failed
	wipepartition=yes
else
	echo "(This can be extended to ask the used if he wants to delete all except /home)"
	echo -n "Wipe out the partition \"$userpartition\"? [yes/N]: "
	read wipepartition
fi

if [ "$wipepartition" == "YES" -o "$wipepartition" == "yes" -o "$wipepartition" == "Yes" ]; then
	echo -n "Creating ext3 filesystem on \"$userpartition\" ... "
	mkfs.ext3 $userpartition &>/dev/null && echo OK || echo Failed
fi

echo

echo -n "Mounting user partition in /mnt ... "
#echo "TODO: copy files from CD to user partition: $userpartition"
mkdir -p /mnt &>/dev/null
mount $userpartition /mnt &>/dev/null && echo OK || echo Failed
mkdir -p /mnt/proc
mount -t proc proc /proc &>/dev/null

echo -n "Creating swap file... (defaulting to 1 GB, and you can't do a thing about it for now) "
dd if=/dev/zero of=/mnt/.swap-file bs=1048576 count=1024 &>/dev/null && mkswap /mnt/.swap-file && echo "OK" || echo "Failed"

echo -n "Enablind swap... "
swapon /mnt/.swap-file && echo "OK" || echo "Failed"

#echo -n "Extracting Pluto... "
#if [ -f /cdrom/pluto-base.tar.bz2 ]; then
#	echo "OK. Found bzip2 image"
#	compression=j
#	pluto_base=pluto-base.tar.bz2
#elif [ -f /cdrom/pluto-base.tar.gz ]; then
#	echo "OK. Found gzip image"
#	compression=z
#	pluto_base=pluto-base.tar.gz
#else
#	echo "Failed. Your installation CD is broken. No installation image found!!!"
#	read -n 1 -s
#fi

#pushd /mnt &>/dev/null
#tar --numeric-owner -psxv${compression}f /cdrom/$pluto_base && echo "Finished extracting Pluto" || echo "Error extracting Pluto"
#popd &>/dev/null

#echo "Copying Pluto stuff to disk"
##mkdir -p /mnt/pluto/scripts
#cp -R /pluto/Packages /mnt/pluto
#cp -R /pluto/Post_Install /mnt/pluto
#cp /pluto/auto_install /mnt/pluto

echo "Installing packages"
/cdrom/Install_Scripts/installer.sh 2>/mnt/install.err.log | tee /mnt/install.log

echo Doing postinstall
cp -a /cdrom/Post_Install /mnt
chroot /mnt find /Post_Install -type f -exec '{}' ';'
chroot /mnt rm -rf /Post_Install

fstab="# Generated at installation
# /etc/fstab: static filesystem information
#
# <file system>	<mount point>	<type>	<options>	<dump>	<pass>
$userpartition	/	ext3	errors=remount-ro	0	1
/.swap-file	none	swap	sw	0	0
"

echo "Creating fstab (this is not suitable for non-Pluto-standard partitions yet)"
echo "$fstab" >/mnt/etc/fstab

lilo_conf="# Generated at installation
boot=$device
lba32
delay=20
vga=normal
root=$userpartition
install=/boot/boot-menu.b
map=/boot/map
image=/vmlinuz
	label=Pluto
	read-only
"
echo "Creating lilo.conf"
echo "$lilo_conf" >/mnt/etc/lilo.conf
echo "Installing LILO bootloader (currenly you, the user, don't have any choice here)"
echo "(support for GRUB, FreeLoader and the like will be added in the future)"
chroot /mnt /sbin/lilo

echo "Unmounting filesystems"
swapoff /mnt/.swap-file
umount /mnt/proc &>/dev/null
umount /mnt &>/dev/null
umount /cdrom &>/dev/null

echo
echo "Installation complete."
echo "Thank you for choosing Pluto"
echo "-- the Pluto team"
echo
echo "Please take out the CD before you reboot"
echo "Press any key to reboot"
read -s -n 1

