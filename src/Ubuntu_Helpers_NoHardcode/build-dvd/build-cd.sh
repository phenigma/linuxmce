#!/bin/bash -x
. /etc/lmce-build/builder.conf

WorkDir="${build_dir}/lite-installer"
LiveCDtgz="$WorkDir/ubuntu-livecd.tgz"
SquashFStgz="$WorkDir/ubuntu-squashfs-base.tgz"

## TODO: change paths to SVN checkout directory
InstallerScript="lite-installer.sh"
RcSScript="install-lmce"
FirstRunScript="firstboot"

LiveCDISO="$WorkDir/kubuntu-linuxmce.iso"
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
	cp ${build_dir}/DisklessSync/i386/PlutoMD-i386.tar.bz2  "$LiveCDDir/diskless-images/"
	cp ${build_dir}/DisklessSync/amd64/PlutoMD-amd64.tar.bz2  "$LiveCDDir/diskless-images/"

	mkdir -p "$LiveCDDir/deb-cache/"
	cp ${build_dir}/DisklessSync/i386/deb-cache/* "$LiveCDDir/deb-cache/"
	cp ${build_dir}/DisklessSync/amd64/deb-cache/* "$LiveCDDir/deb-cache/"

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

	rm -f "/var/www/LinuxMCE-DVD-${arch}.iso"
	ln "$LiveCDISO" "/var/www/LinuxMCE-DVD-${arch}.iso"
}

CleanupWorkDir()
{
	rm -rf "$LiveCDDir" "$SquashFSDir"
}

PrepareWorkDir
CreateSquashFS
CreateLiveCD
CleanupWorkDir
