#!/bin/bash -x

. /etc/lmce-build/builder.conf
. /usr/local/lmce-build/common/logging.sh

set -e
set -x

DisplayMessage "*** STEP: Creating Install DVD [ $1 ]"
trap 'Error "Undefined error in $0"' EXIT

WorkDir="${build_dir}/lite-installer"
LiveCDtgz="$WorkDir/ubuntu-livecd.tgz"
SquashFStgz="$WorkDir/ubuntu-squashfs-base.tgz"

## TODO: change paths to SVN checkout directory
InstallerScript="lite-installer.sh"
RcSScript="install-lmce"
FirstRunScript="firstboot"

if [[ "$1" == "big" ]] ;then
	LiveCDISO="$WorkDir/kubuntu-linuxmce-big.iso"
	LiveCDISO_Link="${local_mirror_dir}/LinuxMCE-DVD-DL-${arch}.iso"
	DoubleLayer="true"
else
	LiveCDISO="$WorkDir/kubuntu-linuxmce.iso"
	LiveCDISO_Link="${local_mirror_dir}/LinuxMCE-DVD-${arch}.iso"
fi

InstallerArchive="${build_dir}/vmware/Kubuntu/linux-mce.tar.gz"

LiveCDDir="$WorkDir/livecd"
SquashFSDir="$WorkDir/squashfs"


PrepareWorkDir()
{
	CleanupWorkDir
	mkdir -p "$LiveCDDir" "$SquashFSDir"
	tar -C "$LiveCDDir" -xzf "$LiveCDtgz"
	tar -C "$SquashFSDir" -xzf "$SquashFStgz"
}

CreateSquashFS()
{
	echo -n "# Linux MCE installer
start on runlevel 2
start on runlevel 3
start on runlevel 4
start on runlevel 5


stop on runlevel 0
stop on runlevel 1
stop on runlevel 6

script
	/usr/bin/$(basename "$InstallerScript")
end script
" >"$SquashFSDir/etc/event.d/installer-lmce"

	rm -f "$SquashFSDir/etc/event.d/tty1"

	cp "$InstallerScript" "$SquashFSDir/usr/bin/"
	cp "$RcSScript" "$SquashFSDir/etc/init.d/"

	rm -f "$SquashFSDir"/etc/rcS.d/S99install-lmce
	chroot "$SquashFSDir" dpkg-query -W --showformat='${Package} ${Version}\n' \
	   > "$LiveCDDir/casper/filesystem.manifest"
	mksquashfs "$SquashFSDir" "$LiveCDDir/casper/filesystem.squashfs"
}

CreateLiveCD()
{
	mkdir -p "$LiveCDDir/lmce-image/"
	cp "$InstallerArchive"_* "$LiveCDDir/lmce-image/"
	cp "$FirstRunScript" "$LiveCDDir/lmce-image/"

	mkdir -p "$LiveCDDir/diskless-images/"
	cp ${build_dir}/DisklessSync/i386/PlutoMD-i386.tar.bz2  "$LiveCDDir/diskless-images/"	|| Warning "Diskless image for i386 architecture missing"
	cp ${build_dir}/DisklessSync/amd64/PlutoMD-amd64.tar.bz2  "$LiveCDDir/diskless-images/" || Warning "Diskless image for amd64 architecture missing"

	mkdir -p "$LiveCDDir/deb-cache/"
	cp ${build_dir}/DisklessSync/i386/deb-cache/* "$LiveCDDir/deb-cache/"	|| Warning "Can't find crossover debs for i386 architecture"
	cp ${build_dir}/DisklessSync/amd64/deb-cache/* "$LiveCDDir/deb-cache/"	|| Warning "Can't find crossover debs for amd64 architecture"

	if [[ "$DoubleLayer" == "true" ]] ;then
		mkdir -p "$LiveCDDir/lmce-videos"
		cp "${build_dir}/lmce-videos"/* "$LiveCDDir/lmce-videos/" || Warning "Could not find lmce-videos for the double layer dvd"
	fi

	(cd "$LiveCDDir" && find . -type f -not -path ./md5sum.txt -print0 | xargs -0 md5sum | tee md5sum.txt)

	mkisofs -o "$LiveCDISO" \
		-b isolinux/isolinux.bin \
		-c isolinux/boot.cat \
		-no-emul-boot \
		-boot-load-size 4 \
		-boot-info-table \
		-r \
		-V "Custom LMCE-Core" \
		-cache-inodes \
		-J \
		-l \
		"$LiveCDDir"

	rm -f "$LiveCDISO_Link"
	mv "$LiveCDISO" "$LiveCDISO_Link"
}

CleanupWorkDir()
{
	rm -rf "$LiveCDDir" "$SquashFSDir"
}

DisplayMessage "Preparing dvd filesystem"
PrepareWorkDir

DisplayMessage "Creating SquashFS"
CreateSquashFS

DisplayMessage "Generating the ISO"
CreateLiveCD

DisplayMessage "Removing temporary files used to create the dvd filesystem"
CleanupWorkDir

trap - EXIT
