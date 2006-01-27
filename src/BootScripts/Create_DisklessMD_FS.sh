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

Modules="pcnet32 tg3 e1000 sk98lin fealnx natsemi r8169 b44 via_velocity"

KERNEL_VERSION="$(uname -r)"

InstallKernel()
{
	local KERNEL_VERSION="$1"
	local Module

	: >etc/mkinitrd/modules
	for Module in $Modules; do
		echo $Module >>etc/mkinitrd/modules
	done

	sed -i 's/^ROOT=.*$/ROOT=/g' etc/mkinitrd/mkinitrd.conf
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

Upgrade_Essential()
{
	# TODO: don't do anything (i.e. also skip download) for packages that match the requirements
	local Requirements="initrd-netboot-tools=0.5.3cvs.20040906-16 e2fsprogs=1.37-2sarge1 e2fslibs=1.37-2sarge1 libc6=2.3.2.ds1-22 sysv-rc=2.86.ds1-1.1pluto7"
	local NeededReq=""
	local Pkg Name ReqVer InstVer
	
	for Pkg in $Requirements; do
		if [[ "$Pkg" != *=* ]]; then
			Pkg="$Pkg="
		fi
		
		Name=${Pkg%=*}
		ReqVer=${Pkg#*=}
		InstVer=$(dpkg -s $Name 2>/dev/null|grep ^Version|cut -d' ' -f2)
		
		if [[ -z "$InstVer" ]]; then
			NeededReq="$NeededReq $Name"
		elif [[ -n "$ReqVer" ]] && dpkg --compare-versions "$InstVer" '<<' "$ReqVer"; then
			NeededReq="$NeededReq $Name"
		fi
	done

	if [[ -n "$NeededReq" ]]; then
		#mount -t proc proc proc
		# TODO: replace scripts that would normally start processes (start-stop-daemon) so they don't -- currently not needed

		pushd tmp/
		aptitude download $NeededReq
		popd

		for Pkg in $NeededReq; do
			chroot . dpkg -i -GE ./tmp/$Pkg*.deb
		done

		rm -f tmp/*.deb 2>/dev/null

		# TODO: put replaced scripts back
		#umount ./proc
	fi

	local Conf=etc/lessdisks/mkinitrd/initrd-netboot.conf
	local Setting="nfs_opts=ro,async,nolock,tcp"
	if ! grep -q 'nfs_opts=' "$Conf" 2>/dev/null; then
		echo "$Setting" >>"$Conf"
	else
		sed -i 's/^.*nfs_opts=.*$/'"$Setting"'/g' "$Conf"
	fi
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
	mkdir -p "$DlPath"/usr/pluto/install
	touch "$DlPath"/usr/pluto/install/.notdone
fi

# Pre-upgrade some packages
set -x
Upgrade_Essential
set +x

# Make sure the right kernel version is installed
InstallKernel $KERNEL_VERSION || exit 1
mkdir -p "/tftpboot/$IP"
[ -e "/tftpboot/$IP/initrd.img-$KERNEL_VERSION" ] || cp "boot/initrd.img-$KERNEL_VERSION" "/tftpboot/$IP"
[ -e "/tftpboot/$IP/vmlinuz-$KERNEL_VERSION" ] || cp "boot/vmlinuz-$KERNEL_VERSION" "/tftpboot/$IP"

cd -

# Use Core's apt sources.list
#cp /etc/apt/sources.list "$DlPath"/etc/apt/sources.list
if ! egrep -q 'http://deb\.plutohome\.com/debian.+replacements.+main' "$DlPath"/etc/apt/sources.list; then
	echo "deb http://deb.plutohome.com/debian/ replacements main" >>"$DlPath"/etc/apt/sources.list
fi
