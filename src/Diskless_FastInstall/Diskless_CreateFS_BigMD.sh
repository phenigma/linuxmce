#!/bin/bash

. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/Network_Parameters.sh
. /usr/pluto/bin/Config_Ops.sh

if [[ "$#" -ne 4 ]]; then
	echo "Syntax: $0 <IP> <MAC> <Device> <Architecture>"
	exit 1
fi

CORE_INTERNAL_INTERFACE="$IntIf"
CORE_INTERNAL_ADDRESS="$IntIP"
INTERNAL_SUBNET_MASK="$IntNetmask"
Vars="CORE_INTERNAL_ADDRESS INTERNAL_SUBNET INTERNAL_SUBNET_MASK MOON_ADDRESS DYNAMIC_IP_RANGE KERNEL_VERSION MOON_HOSTS MOON_IP NOBOOT_ENTRIES ENABLE_SPLASH MOON_BOOTPARAMS MOON_ID"

IP="$1"
MAC="$2"
Device="$3"
Architecture="${4:-686}"
FSarchive=PlutoMD.tar.bz2
DlPath="/home/BigMD"
HexIP=$(gethostip -x "$IP")

# Modules to be included in the initramfs
Modules="ne 3c509 3c59x 8139cp 8139too dmfe e100 eepro100 eexpress epic100 hp100 ne2k-pci sis900 tlan de4x5 tulip via-rhine yellowfin forcedeth pcnet32 tg3 e1000 sk98lin fealnx natsemi r8169 b44 via_velocity skge sky2"

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
		sysv-rc=2.86.ds1-1.1pluto50
		pluto-boot-scripts=2.0.0.36
		lsb-base
		module-init-tools=3.2.2-2pluto1
		busybox-cvs-static
		libklibc
		klibc-utils
		libvolume-id0
		udev=0.086-1
		initramfs-tools
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

# Pre-upgrade some packages
set -x
Upgrade_Essential
Upgrade_Monster
set +x

# Make sure the right kernel version is installed
KERNEL_VERSION="$(uname -r)"
KERNEL_VERSION="${KERNEL_VERSION%*-*86*}-$Architecture" # Our kernels always report the architecture they're compiled for last

#InstallKernel $KERNEL_VERSION || exit 1

# Create the archives-cache directory
mkdir -p "$DlPath"/usr/pluto/var

cd -

# Use Core's apt sources.list
#cp /etc/apt/sources.list "$DlPath"/etc/apt/sources.list
if ! egrep -q 'http://deb\.plutohome\.com/debian.+replacements_debug.+main' "$DlPath"/etc/apt/sources.list; then
	echo "deb http://deb.plutohome.com/debian/ replacements_debug main" >>"$DlPath"/etc/apt/sources.list
fi

if ! egrep -q 'http://deb\.plutohome\.com/debian.+20dev_debug+main' "$DlPath"/etc/apt/sources.list; then
	echo "deb http://deb.plutohome.com/debian/ 20dev_debug main" >>"$DlPath"/etc/apt/sources.list
fi

Files="Common.sh ConfirmDependencies_Debian.sh Initial_Config_MD.sh Initial_Config_Finish.sh ramdisk.tar.bz2"
for Stuff in $Files; do
	cp /usr/pluto/install/$Stuff "$DlPath"/usr/pluto/install
done
sed -i 's/^reboot$/#reboot/' "$DlPath"/usr/pluto/install/Initial_Config_MD.sh

if [[ ! -f $DlPath/usr/pluto/install/activation.sh ]]; then
	/usr/pluto/bin/ConfirmDependencies -r -D pluto_main -h dcerouter -u root -p '' -d $Device install > $DlPath/usr/pluto/install/activation.sh
fi
ln /home/Install_MD.sh "$DlPath"

## Modify a install script to run as for diskless
sed '/^Type=/ s/^.*$/Type="diskless"/' /usr/pluto/install/Initial_Config.sh >$DlPath/usr/pluto/install/Initial_Config.sh
chmod +x $DlPath/usr/pluto/install/Initial_Config.sh
mkdir -p $DlPath/usr/pluto/deb-cache

sed 's/localhost/dcerouter/g' /etc/apt/apt.conf.d/30pluto > $DlPath/etc/apt/apt.conf.d/30pluto

if [[ ! -f $DlPath/etc/pluto.conf ]] ;then
	if [[ "$OfflineMode" == "" ]]; then
		OfflineMode='false'
	fi

	pluto_conf="OfflineMode = $OfflineMode \n"
	pluto_conf="${pluto_conf}PlutoVersion = $PlutoVersion \n"
	pluto_conf="${pluto_conf}Vendor = $Vendor \n"
	echo -e $pluto_conf > $DlPath/etc/pluto.conf
fi

echo "nameserver $IntIP" >$DlPath/etc/resolv.conf

cp /usr/pluto/templates/hosts.tmpl $DlPath/etc/hosts.$$
ReplaceVars $DlPath/etc/hosts.$$
mv $DlPath/etc/hosts{.$$,}
