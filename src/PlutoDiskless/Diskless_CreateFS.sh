#!/bin/bash

. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/Diskless_Utils.sh

if [[ "$#" -ne 4 ]]; then
	echo "Syntax: $0 <IP> <MAC> <Device> <Architecture>"
	exit 1
fi

IP="$1"
MAC="$2"
Device="$3"
Architecture="${4:-686}"
FSarchive1=PlutoMD.tar.bz2
FSarchive2=PlutoMD-pre-install.tar.bz2
DlPath="/usr/pluto/diskless/$Device"
HexIP=$(gethostip -x "$IP")

# Modules to be included in the initramfs
Modules="ne 3c509 3c59x 8139cp 8139too dmfe e100 eepro100 eexpress epic100 hp100 ne2k-pci sis900 tlan de4x5 tulip via-rhine yellowfin forcedeth pcnet32 tg3 e1000 sk98lin fealnx natsemi r8169 b44 via_velocity skge sky2"

FSarchive="$FSarchive1"
if [[ -f /usr/pluto/install/"$FSarchive2" ]]; then
	FSarchive="$FSarchive2"
fi

InstallKernel()
{
	local KERNEL_VERSION="$1"
	local kernel="$2"
	local Module
	local Kout

	local ModulesFile="etc/initramfs-tools/modules"
	: >"$ModulesFile"
	for Module in $Modules; do
		echo $Module >>"$ModulesFile"
	done

	if chroot . dpkg -s "linux-image-$KERNEL_VERSION" 2>/dev/null | grep -q 'Status: install ok installed'; then
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Kernel already installed."
		return 0
	fi
	
	Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Installing kernel '$kernel' on '$IP'"
	rm -rf lib/modules/"$KERNEL_VERSION"

	cp "$kernel" tmp/

	mount -t proc proc proc
	mount sys sys -t sysfs
	if ! Kout=$(echo | chroot . dpkg -i --force-all "/tmp/${kernel##*/}" 2>&1); then
		Logging "$TYPE" "$SEVERITY_CRITICAL" "$0" "Failed to install kernel '$KERNEL_VERSION' on '$IP'"
		echo "Kernel package output was:"
		echo "$Kout"
	fi

	## Hard way to do the inird
#	chroot . rm /boot/initrd.img-`uname -r`
#	chroot . yaird --output /boot/initrd.img-`uname -r`
#	chroot . chmod 644 /boot/initrd.img-`uname -r`

	umount ./proc
	umount ./sys
}


Upgrade_Monster()
{
	MonsterInstState=$(dpkg -s monster-nucleus 2>/dev/null|grep ^Status|cut -d' ' -f2-)	
	if [[ "$MonsterInstState" != "install ok installed" ]] ;then
		return
	fi

	shopt -s nullglob
	# TODO: don't do anything (i.e. also skip download) for packages that match the requirements
	local Requirements="bootsplash-theme-monster
	pluto-avwizard-skin-monster
	"
	local NeededReq=""
	local Pkg Name ReqVer InstVer InstState
	
	for Pkg in $Requirements; do
		if [[ "$Pkg" != *=* ]]; then
			Pkg="$Pkg="
		fi
		
		Name=${Pkg%=*}
		ReqVer=${Pkg#*=}
		InstVer=$(chroot . dpkg -s $Name 2>/dev/null|grep ^Version|cut -d' ' -f2)
		InstState=$(chroot . dpkg -s $Name 2>/dev/null|grep ^Status|cut -d' ' -f2-)
		
		if [[ -z "$InstVer" || "$InstState" != "install ok installed" ]]; then
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
		for File in $NeededReq; do
			Filename=$(echo "$File"*)
			if [[ -n "$Filename" ]]; then # it was downloaded
				continue
			fi
			cp -v /usr/pluto/deb-cache/dists/sarge/main/binary-i386/"$File"* .
		done
		popd

		local PkgList
		PkgList=()
		for Pkg in $NeededReq; do
			PkgList=("${PkgList[@]}" $(echo -n ./tmp/${Pkg}_*.deb))
		done
		if [[ "${#PkgList[@]}" -gt 0 ]]; then
			chroot . dpkg -i -GE "${PkgList[@]}"
		fi

		rm -f tmp/*.deb 2>/dev/null

		# TODO: put replaced scripts back
		#umount ./proc
	fi
}


Upgrade_Essential()
{
	shopt -s nullglob
	# TODO: don't do anything (i.e. also skip download) for packages that match the requirements
	local Requirements="
		locales
		initrd-netboot-tools=0.5.3cvs.20040906-16
		e2fsprogs=1.37-2sarge1
		e2fslibs=1.37-2sarge1
		libc6=2.3.2.ds1-22
		sysv-rc=2.86.ds1-1.1pluto68
		pluto-boot-scripts=2.0.0.36
		lsb-base
		module-init-tools=3.2.2-2pluto1
		busybox-cvs-static
		libklibc
		klibc-utils
		libvolume-id0
		udev=0.091-0bpo1pluto1
		initramfs-tools
		gzip=1.3.5-14
		libfuse2
		libkrb53
		libsasl2
		libldap2
		libsmbclient
		ucf
		fuse-utils
		pluto-fuse-smb
	"
	local NeededReq=""
	local Pkg Name ReqVer InstVer InstState
	
	for Pkg in $Requirements; do
		if [[ "$Pkg" != *=* ]]; then
			Pkg="$Pkg="
		fi
		
		Name=${Pkg%=*}
		ReqVer=${Pkg#*=}
		InstVer=$(chroot . dpkg -s $Name 2>/dev/null|grep ^Version|cut -d' ' -f2)
		InstState=$(chroot . dpkg -s $Name 2>/dev/null|grep ^Status|cut -d' ' -f2-)
		
		if [[ -z "$InstVer" || "$InstState" != "install ok installed" ]]; then
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
		for File in $NeededReq; do
			Filename=$(echo "$File"*)
			if [[ -n "$Filename" ]]; then # it was downloaded
				continue
			fi
			cp -v /usr/pluto/deb-cache/dists/sarge/main/binary-i386/"$File"* .
		done
		popd

		for Pkg in $NeededReq; do
			if [[ -z "$(echo ./tmp/$Pkg*)" ]]; then
				Logging "$TYPE" "$SEVERITY_CRITICAL" "$0" "Package '$Pkg' can't be installed. No deb file."
			fi
			chroot . dpkg -i -GE ./tmp/$Pkg*.deb
		done

		rm -f tmp/*.deb 2>/dev/null

		# TODO: put replaced scripts back
		#umount ./proc
	fi
}


mkdir -p "$DlPath"
if ! cd "$DlPath"; then
	Logging "$TYPE" "$SEVERITY_CRITICAL" "$0" "Couldn't switch to diskless image directory. Not extracting."
	exit 1
fi

if [[ ! -d "$DlPath" || ! -f "$DlPath/etc/diskless.conf" ]]; then
	Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Extracting filesystem for diskless client '$IP , $MAC' ($Device)"

	if ! tar -xjf "/usr/pluto/install/$FSarchive" >/dev/null; then
		Logging "$TYPE" "$SEVERITY_CRITICAL" "$0" "Failed to extract filesystem"
		exit 1
	fi

	conf="IP=$IP
MAC=$MAC
Device=$Device
Code="
	echo "$conf" >etc/diskless.conf
	[[ -d /home/backup && -f "/home/backup/pluto.conf-$IP" ]] && cp "/home/backup/pluto.conf-$IP" "$DlPath/etc"
	mkdir -p "$DlPath"/usr/pluto/install
	touch "$DlPath"/usr/pluto/install/.notdone
fi

RequiredModules="ide-cd ide-disk psmouse mousedev"
for Module in $RequiredModules; do
	if ! grep -q "$Module" "$DlPath"/etc/modules; then
		echo $Module >>"$DlPath"/etc/modules
	fi
done

SuppressModules="ide-generic"
for Module in $SuppressModules; do
	sed -i "/$Module/ d" "$DlPath"/etc/modules
done

# Use core's locale
cp /etc/locale.gen "$DlPath"/etc/

## Setup debconf interface to 'noninteractive'
echo "Setting Debconf interface to 'noninteractive' and default priority 'critical'"
awk '
/^Name: debconf\/frontend$/,/^$/ {
	if ($1 == "Value:")
		print "Value: noninteractive";
	else
		print;
	next;
}
/^Name: debconf\/priority$/,/^$/ {
	if ($1 == "Value:")
		print "Value: critical";
	else
		print;
	next;
}
{print}
' "$DlPath"/var/cache/debconf/config.dat >"$DlPath"/var/cache/debconf/config.dat.$$
mv "$DlPath"/var/cache/debconf/config.dat{.$$,}

# Make sure the right kernel version is installed
set -x
KernelDecision=$(DecideKernelVersion "$Architecture" | tail -2)
KERNEL_VERSION=$(echo "$KernelDecision" | tail -1)
kernel=$(echo "$KernelDecision" | head -1)

# Pre-upgrade some packages
Upgrade_Essential
Upgrade_Monster
set +x

InstallKernel "$KERNEL_VERSION" "$kernel"

set -x
mkdir -p "/tftpboot/$Device"
ln -sf "$DlPath/boot/initrd.img-$KERNEL_VERSION" "/tftpboot/$Device/"
ln -sf "$DlPath/boot/vmlinuz-$KERNEL_VERSION" "/tftpboot/$Device/"
set +x

# Create the archives-cache directory
mkdir -p "$DlPath"/usr/pluto/var

cd -

# Use Core's apt sources.list
#cp /etc/apt/sources.list "$DlPath"/etc/apt/sources.list
if ! egrep -q 'http://deb\.plutohome\.com/debian.+<-mkr_t_replacementsdeb->.+main' "$DlPath"/etc/apt/sources.list; then
	echo "deb http://deb.plutohome.com/debian/ <-mkr_t_replacementsdeb-> main" >>"$DlPath"/etc/apt/sources.list
fi

if ! egrep -q 'http://deb\.plutohome\.com/debian.+<-mkr_t_maindeb->.+main' "$DlPath"/etc/apt/sources.list; then
	echo "deb http://deb.plutohome.com/debian/ <-mkr_t_maindeb-> main" >>"$DlPath"/etc/apt/sources.list
fi
