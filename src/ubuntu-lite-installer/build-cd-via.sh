#!/bin/bash -x

WorkDir="/var/plutobuild/lite-installer-via"
WorkDirUbuntu="/var/plutobuild/lite-installer"
LiveCDDir="$WorkDir/livecd"
SquashFSDir="$WorkDir/squashfs"
LiveCDtgz="$WorkDirUbuntu/ubuntu-livecd.tgz"
SquashFStgz="$WorkDirUbuntu/ubuntu-squashfs-base.tgz"

## TODO: change paths to SVN checkout directory
InstallerScript="lite-installer-via.sh"
RcSScript="install-lmce-via"
FirstRunScript="firstboot-via"

LiveCDISO="$WorkDir/via-linuxmce.iso"
FilesystemArchives=("boot.tgz" "kubuntu.tgz" "pluto44.tgz")

PrepareWorkDir()
{
	CleanupWorkDir
	mkdir -p "$LiveCDDir" "$SquashFSDir"
	tar -C "$LiveCDDir" -xzf "$LiveCDtgz"
	tar -C "$SquashFSDir" -xzf "$SquashFStgz"
}

CreateSquashFS()
{
	cp "$InstallerScript" "$SquashFSDir/usr/bin/"
	cp "$RcSScript" "$SquashFSDir/etc/init.d/"
	ln -snf "/etc/init.d/$(basename "$RcSScript")" "$SquashFSDir/etc/rcS.d/S99install-lmce-via"
	chroot "$SquashFSDir" dpkg-query -W --showformat='${Package} ${Version}\n' \
	   > "$LiveCDDir/casper/filesystem.manifest"
	mksquashfs "$SquashFSDir" "$LiveCDDir/casper/filesystem.squashfs"
}

CreateLiveCD()
{
	local Archive

	mkdir -p "$LiveCDDir/lmce-image-via/"
	for Archive in "${FilesystemArchives[@]}"; do
		ln "$WorkDir/$Archive" "$LiveCDDir/lmce-image-via/"
	done
	cp "$FirstRunScript" "$LiveCDDir/lmce-image-via/"
	(cd "$LiveCDDir" && find . -type f -not -path ./md5sum.txt -print0 | xargs -0 md5sum | tee md5sum.txt)
	mkisofs -o "$LiveCDISO" \
		-b isolinux/isolinux.bin \
		-c isolinux/boot.cat \
		-no-emul-boot \
		-boot-load-size 4 \
		-boot-info-table \
		-r \
		-V "Custom VIA LMCE-Core" \
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
