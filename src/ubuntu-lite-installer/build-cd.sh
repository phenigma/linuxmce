#!/bin/bash -x

WorkDir="/var/plutobuild/lite-installer"
LiveCDDir="$WorkDir/livecd"
SquashFSDir="$WorkDir/squashfs"
LiveCDtgz="$WorkDir/ubuntu-livecd.tgz"
SquashFStgz="$WorkDir/ubuntu-squashfs-base.tgz"

## TODO: change paths to SVN checkout directory
InstallerScript="lite-installer.sh"
RcSScript="install-lmce"
FirstRunScript="firstboot"

LiveCDISO="$WorkDir/kubuntu-linuxmce.iso"
InstallerArchive="/var/plutobuild/vmware/Kubuntu7.04/linux-mce.tar.gz"

RemoteAssistanceKey=/var/plutobuild/lite-installer/pf-linuxmce

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
if grep -q "recovery" /proc/cmdline ;then
	/usr/bin/RecoveryConsole.sh
else
	usr/bin/$(basename "$InstallerScript")
fi
end script
" >"$SquashFSDir/etc/event.d/installer-lmce"

	echo -n "# Recovery
start on runlevel 3

stop on runlevel 0
stop on runlevel 1
stop on runlevel 6

" > "$SquashFSDir/etc/event.d/recovery-console"

	rm -f "$SquashFSDir/etc/event.d/tty1"

	cp "$InstallerScript" "$SquashFSDir/usr/bin/"
	cp "$RcSScript" "$SquashFSDir/etc/init.d/"

	cp "EnableRemoteAssistance.sh" "$SquashFSDir/usr/bin"
	cp "RecoveryConsole.sh" "$SquashFSDir/usr/bin"
	cp "NetworkSetup.sh" "$SquashFSDir/usr/bin"

	rm -f "$SquashFSDir"/etc/rcS.d/S99install-lmce
	chroot "$SquashFSDir" dpkg-query -W --showformat='${Package} ${Version}\n' \
	   > "$LiveCDDir/casper/filesystem.manifest"
	mksquashfs "$SquashFSDir" "$LiveCDDir/casper/filesystem.squashfs"
}

CreateLiveCD()
{
	mkdir -p "$LiveCDDir/lmce-image/"
	ln "$InstallerArchive"_* "$LiveCDDir/lmce-image/"
	cp "$FirstRunScript" "$RemoteAssistanceKey" "$LiveCDDir/lmce-image/"
	chmod 600 "$LiveCDDir/lmce-image/$RemoteAssistanceKey"
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
}

CleanupWorkDir()
{
	rm -rf "$LiveCDDir" "$SquashFSDir"
}

PrepareWorkDir
CreateSquashFS
CreateLiveCD
CleanupWorkDir
