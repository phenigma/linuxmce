#!/bin/bash

. /usr/pluto/bin/pluto.func

if [ "$#" -ne 4 ]; then
	echo "Syntax: $0 <IP> <MAC> <Device> <Activation Code>"
	exit 1
fi

IP="$1"
MAC="$2"
Device="$3"
Code="$4"

FSarchive=PlutoMD.tar.bz2
DlPath="/usr/pluto/diskless/$IP"
HexIP=$(gethostip -x "$IP")

Modules="pcnet32 tg3 e1000 sk98lin fealnx natsemi r8169"

KERNEL_VERSION="$(uname -r)"

InstallKernel()
{
	local KERNEL_VERSION="$1"
	local Module

	: >etc/mkinitrd/modules
	for Module in $Modules; do
		echo $Module >>etc/mkinitrd/modules
	done

	if chroot . dpkg -s "kernel-image-$KERNEL_VERSION" 2>/dev/null | grep -q 'Status: install ok installed'; then
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Kernel already installed."
		return 0
	fi
	
	kernel="$(find /usr/pluto/deb-cache/dists/sarge/main/binary-i386/ -name "kernel-image-$KERNEL_VERSION*.deb")"
	words="$(echo "$kernel" | wc -w)"
	if [ "$words" -eq 0 ]; then
		Logging "$TYPE" "$SEVERITY_CRITICAL" "$0" "No kernel matching 'kernel-image-$KERNEL_VERSION' was found"
		exit 1
	elif [ "$words" -gt 1 ]; then
		kernel="$(echo "$kernel" | cut -d' ' -f1)"
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "More than one kernel found (this shouldn't happen). Using '$kernel'."
	fi
	Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Installing kernel '$kernel' on '$IP'"

	cp "$kernel" tmp/
	mount -t proc proc proc
	if ! echo | chroot . dpkg -i "/tmp/${kernel##*/}" &>/dev/null; then
		Logging "$TYPE" "$SEVERITY_CRITICAL" "$0" "Failed to install kernel '$KERNEL_VERSION' on '$IP'"
	fi
	umount ./proc
}

mkdir -p "$DlPath"
if ! cd "$DlPath"; then
	Logging "$TYPE" "$SEVERITY_CRITICAL" "$0" "Couldn't switch to diskless image directory. Not extracting."
	exit 1
fi

if [ ! -d "$DlPath" -o ! -f "$DlPath/etc/diskless.conf" ]; then
	Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Extracting filesystem for diskless client '$IP , $MAC' ($Device)"

	tar -xjvf "/usr/pluto/install/$FSarchive" >/dev/null

	conf="IP=$IP
MAC=$MAC
Device=$Device
Code=$Code"
	echo "$conf" >etc/diskless.conf
	[ -d /home/backup -a -f "/home/backup/pluto.conf-$IP" ] && cp "/home/backup/pluto.conf-$IP" "$DlPath/etc"
	touch "$DlPath"/usr/pluto/install/.notdone
fi

# Make sure the right kernel version is installed
InstallKernel $KERNEL_VERSION || exit 1
mkdir -p "/tftpboot/$IP"
[ -e "/tftpboot/$IP/initrd.img-$KERNEL_VERSION" ] || cp "boot/initrd.img-$KERNEL_VERSION" "/tftpboot/$IP"
[ -e "/tftpboot/$IP/vmlinuz-$KERNEL_VERSION" ] || cp "boot/vmlinuz-$KERNEL_VERSION" "/tftpboot/$IP"

cd -

if ! egrep -q 'http://deb\.plutohome\.com/debian.+replacements.+main' "$DlPath"/etc/apt/sources.list; then
	echo "deb http://deb.plutohome.com/debian/ replacements main" >>"$DlPath"/etc/apt/sources.list
fi
