#!/bin/bash

. /usr/pluto/bin/pluto.func

if [[ "$#" -ne 5 ]]; then
	echo "Syntax: $0 <IP> <MAC> <Device> <Architecture>"
	exit 1
fi

IP="$1"
MAC="$2"
Device="$3"
Architecture="${4:-686}"

FSarchive=PlutoMD.tar.bz2
DlPath="/usr/pluto/diskless/$IP"
HexIP=$(gethostip -x "$IP")

Modules="pcnet32 tg3 e1000 sk98lin fealnx natsemi r8169 b44 via_velocity"

KERNEL_VERSION="$(uname -r)"
KERNEL_VERSION="${KERNEL_VERSION%-*}-$Architecture" # Our kernels always report the architecture they're compiled for last

InstallKernel()
{
	local KERNEL_VERSION="$1"
	local Module
	local Kout

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
	if [[ "$words" -eq 0 ]]; then
		Logging "$TYPE" "$SEVERITY_CRITICAL" "$0" "No kernel matching 'kernel-image-$KERNEL_VERSION' was found"
		exit 1
	elif [[ "$words" -gt 1 ]]; then
		kernel="$(echo "$kernel" | cut -d' ' -f1)"
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "More than one kernel found (this shouldn't happen). Using '$kernel'."
	fi
	Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Installing kernel '$kernel' on '$IP'"
	rm -rf lib/modules/"$KERNEL_VERSION"

	cp "$kernel" tmp/
	mount -t proc proc proc
	if ! Kout=$(echo | chroot . dpkg -i "/tmp/${kernel##*/}" 2>&1); then
		Logging "$TYPE" "$SEVERITY_CRITICAL" "$0" "Failed to install kernel '$KERNEL_VERSION' on '$IP'"
		echo "Kernel package output was:"
		echo "$Kout"
	fi
	umount ./proc
}

Upgrade_Essential()
{
	shopt -s nullglob
	# TODO: don't do anything (i.e. also skip download) for packages that match the requirements
	local Requirements="initrd-netboot-tools=0.5.3cvs.20040906-16
		e2fsprogs=1.37-2sarge1
		e2fslibs=1.37-2sarge1
		libc6=2.3.2.ds1-22
		sysv-rc=2.86.ds1-1.1pluto34
		pluto-boot-scripts=2.0.0.36
		lsb-base
		module-init-tools=3.2.2
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

# Pre-upgrade some packages
set -x
Upgrade_Essential
set +x

# Make sure the right kernel version is installed
InstallKernel $KERNEL_VERSION || exit 1
mkdir -p "/tftpboot/$IP"
ln -sf "$DlPath/boot/initrd.img-$KERNEL_VERSION" "/tftpboot/$IP/"
ln -sf "$DlPath/boot/vmlinuz-$KERNEL_VERSION" "/tftpboot/$IP/"

# Create the archives-cache directory
mkdir -p "$DlPath"/usr/pluto/var

cd -

# Use Core's apt sources.list
#cp /etc/apt/sources.list "$DlPath"/etc/apt/sources.list
if ! egrep -q 'http://deb\.plutohome\.com/debian.+replacements.+main' "$DlPath"/etc/apt/sources.list; then
	echo "deb http://deb.plutohome.com/debian/ replacements main" >>"$DlPath"/etc/apt/sources.list
fi
