#!/bin/bash

. /home/WorkNew/src/BootScripts/LockUtils.sh

Content_Dir=image-netinst

for ((i = 1; i <= "$#"; i++)); do
	case "${!i}" in
		--cache) Cache=y ;;
		--iso-dir) ((i++)); ISO_Dir="${!i}" ;;
		--content-dir) ((i++)); Content_Dir="${!i}" ;;
		*) echo "ERROR: Unknown parameter '${!i}'" ;;
	esac
done

if [[ -z "$MakeRelease_Flavor" ]]; then
	echo "ERROR: 'MakeRelease_Flavor' variable not found"
	exit 1
fi

if [[ -z "$ISO_Dir" ]]; then
	echo "ERROR: Build directory not specified"
	exit 1
fi

TryLock "BuildCD1" "BuildCD1"
trap 'Unlock "BuildCD1" "BuildCD1"' EXIT

Debian_Mirror="/home/mirrors"
Replacements="/home/samba/repositories/$MakeRelease_Flavor/$replacementsdeb"
MainRepository="/home/samba/repositories/$MakeRelease_Flavor/$maindeb"

CD1_RemovePkgsDir="/old-server/home/radu/pkg/bonuscd-cache/cd1remove/";
CD1_AddPkgsDir="/old-server/home/radu/pkg/bonuscd-cache/cd1add/";

Version=$(basename "$ISO_Dir")

Installer_Files=(BonusCdMenu.sh BonusCdAutoInst.sh Common.sh ../AptUtils/AptSources.sh ConfirmDependencies_Debian.sh Initial_Config.sh Initial_Config_Core.sh Initial_Config_MD.sh Initial_Config_Finish.sh Initial_DHCP_Config.sh Initial_Network_Config.sh pkgsel post-debootstrap.sh preseed.cfg PreseedStage2.sh VIA_Setup.sh post-grub-install.sh)
Mail_Dest=(testing@plutohome.com -c linux-team@plutohome.com)

if [[ "$Cache" == y ]]; then
	rm -rf "$Content_Dir/Debian-Cache"
	mkdir -p "$Content_Dir/Debian-Cache"
	
	# Copy CD boot splash
	# TODO: make this per-flavor
	cp "$Content_Dir"/isolinux/pluto.rle "$Content_Dir"/isolinux/splash.rle

	# TODO: improve this part to do searches, walk dependencies
	cp /old-server/home/radu/pkg/copy/*.deb "$Content_Dir"/Debian-Cache
	cp /old-server/home/radu/pkg/missing/*.deb "$Content_Dir"/Debian-Cache
	cp "$ISO_Dir"/debian/main/binary-i386/*.deb "$Content_Dir"/Debian-Cache
	
	find "$Replacements"/main/binary-i386/ -follow -type f -name '*.deb' -exec cp '{}' "$Content_Dir"/Debian-Cache ';'
	find "$Content_Dir"/Debian-Cache '(' -name 'kernel-image*' -or -name 'kernel-source*' -or -name 'kernel-headers*' -or -name 'kernel-doc*' ')' -exec rm '{}' ';'
	#find "$Content_Dir"/Debian-Cache -name 'linux-image*' -not -name '*2.6.16.20-pluto-2*' -exec rm -f '{}' ';'
	if [[ "$MakeRelease_Flavor" == via ]]; then
		find "$Content_Dir"/Debian-Cache -name 'linux-image*-smp*' -exec rm -f '{}' ';'
	fi
	find "$Content_Dir"/Debian-Cache '(' -name 'linux-source*' -or -name 'linux-headers*' -or -name 'linux-doc*' -or -name 'linux-kbuild-*' -or -name 'linux-manual-*' -or -name 'linux-patch-debian-*' -or -name 'linux-support-*' -or -name 'linux-tree-*' ')' -exec rm '{}' ';'
	find "$Content_Dir"/Debian-Cache -name 'pluto-src-*' -exec rm '{}' ';'

	find "$Content_Dir"/Debian-Cache -name 'pluto-src-*' -exec rm '{}' ';'
	rm image-netints/Debian-Cache/kernel-*
	rm "$Content_Dir"/Debian-Cache/pluto-skins-rse*
	rm "$Content_Dir"/Debian-Cache/pluto-marbella*
	rm "$Content_Dir"/Debian-Cache/pluto-audi*
	#rm "$Content_Dir"/Debian-Cache/pluto-sample-media*
	rm "$Content_Dir"/Debian-Cache/monster*
	rm "$Content_Dir"/Debian-Cache/pluto-irtrans*
	rm "$Content_Dir"/Debian-Cache/pluto-powerfile*
	rm "$Content_Dir"/Debian-Cache/pluto-tira*
	rm "$Content_Dir"/Debian-Cache/pluto-monster-database*
	rm "$Content_Dir"/Debian-Cache/pluto-skins-bo*
	rm "$Content_Dir"/Debian-Cache/pluto-generic-serial-device*
	rm "$Content_Dir"/Debian-Cache/pluto-monster-lighting*
	rm "$Content_Dir"/Debian-Cache/libwxgtk2.6-dbg*
	rm "$Content_Dir"/Debian-Cache/libwxgtk2.6-dev*
	rm "$Content_Dir"/Debian-Cache/python-wx*
	rm "$Content_Dir"/Debian-Cache/slim*
	rm "$Content_Dir"/Debian-Cache/pluto-slim-server*
	rm "$Content_Dir"/Debian-Cache/pluto-slimserver-plugin*
	rm "$Content_Dir"/Debian-Cache/video-wizard-videos*
	rm "$Content_Dir"/Debian-Cache/python2.4-samba*
	
	# Window Manager wars
	#rm "$Content_Dir"/Debian-Cache/icewm*
	#rm "$Content_Dir"/Debian-Cache/xcompmgr*
	#rm "$Content_Dir"/Debian-Cache/xfce*
	#rm "$Content_Dir"/Debian-Cache/xfwm*
	
	find "$Content_Dir"/Debian-Cache '(' -name '*-dev_*.deb' -not -name '*xutils*' ')' -exec rm '{}' ';'

	rm "$Content_Dir"/Debian-Cache/*-dbg_*.deb
	rm "$Content_Dir"/Debian-Cache/vdr*.deb
	rm "$Content_Dir"/Debian-Cache/pluto-vdr*.deb
	rm "$Content_Dir"/Debian-Cache/pluto-videolan*.deb
	rm "$Content_Dir"/Debian-Cache/vlc*.deb
	rm "$Content_Dir"/Debian-Cache/pluto-liblinphone*.deb
	rm "$Content_Dir"/Debian-Cache/xfonts*-transcoded*.deb
	rm "$Content_Dir"/Debian-Cache/xspecs*.deb
	rm "$Content_Dir"/Debian-Cache/xvfb*.deb
	rm "$Content_Dir"/Debian-Cache/xnest*.deb
	rm "$Content_Dir"/Debian-Cache/xdmx*.deb
	rm "$Content_Dir"/Debian-Cache/xlibmesa-dri*.deb
	rm "$Content_Dir"/Debian-Cache/*gdb*.deb
	rm "$Content_Dir"/Debian-Cache/nvidia-kernel-source*.deb
	rm "$Content_Dir"/Debian-Cache/*-src_*.deb
	rm "$Content_Dir"/Debian-Cache/*-doc_*.deb
	rm "$Content_Dir"/Debian-Cache/wx2.6-doc*.deb
	rm "$Content_Dir"/Debian-Cache/pluto-avwizard-sounds*
	rm "$Content_Dir"/Debian-Cache/libsdl1.2debian-{arts,esd,nas,oss,all}*
	rm "$Content_Dir"/Debian-Cache/wx-common*
	rm "$Content_Dir"/Debian-Cache/wx2.6*
	rm "$Content_Dir"/Debian-Cache/icewm-lite*
	rm "$Content_Dir"/Debian-Cache/pluto-avwizard-skin*
	rm "$Content_Dir"/Debian-Cache/bootsplash-theme-monster*
	rm "$Content_Dir"/Debian-Cache/bootsplash-theme-linuxmce*
	rm "$Content_Dir"/Debian-Cache/*-doc-*.deb
	rm "$Content_Dir"/Debian-Cache/qt3-dev-tools*
	rm "$Content_Dir"/Debian-Cache/libwxgtk2.6*
	rm "$Content_Dir"/Debian-Cache/valgrind_*
	rm "$Content_Dir"/Debian-Cache/pluto-sample-media_*
	rm "$Content_Dir"/Debian-Cache/pluto-xml-data-plugin_*
	rm "$Content_Dir"/Debian-Cache/fglrx-driver_*
	
	# ugly, ugly, ugly, hack
	rm "$Content_Dir"/Debian-Cache/pluto-bluetooth-dongle*
	
	# Remove old packages from CD 1
	pushd "$CD1_RemovePkgsDir" >/dev/null
	for i in $(ls); do
		rm -rf /home/installation-cd-kernel-2.6.12/"$Content_Dir"/Debian-Cache/"$i"
	done
	popd >/dev/null

	# Add new packages to CD 1
	cp -r "$CD1_AddPkgsDir"/*.deb "$Content_Dir"/Debian-Cache

	# add important packages for cd1 to work with offline installation
	#cp "$Replacements"/main/binary-i386/pluto-sample-media*.deb "$Content_Dir"/Debian-Cache
	find "$Replacements"/main/binary-i386/ -follow -name "avwizard-sounds-dummy*.deb" -exec cp '{}' "$Content_Dir"/Debian-Cache ';'
	cp "$MainRepository"/main/binary-i386/pluto-avwizard-skin-basic*.deb "$Content_Dir"/Debian-Cache

	echo "Making cached packages Indexes"
	dpkg-scanpackages "$Content_Dir"/Debian-Cache /dev/null 2>/dev/null |
		sed s,"$Content_Dir"/Debian-Cache/,dists/sarge/main/binary-i386/,g |
		gzip -9c >"$Content_Dir"/Debian-Cache/Packages.gz

	echo "Cleaning up duplicate packages"
	pushd "$Content_Dir"/Debian-Cache >/dev/null
	mkdir keep
	zgrep '^Filename: ' Packages.gz |
		cut -d' ' -f2 |
		cut -d/ -f5 |
		xargs -ifile mv file keep
	rm *.deb
	mv keep/* .
	rmdir keep
	popd >/dev/null

	# Update cd kernels
	rm "$Content_Dir"/pool/main/l/linux-2.6/*.deb
	find "$Content_Dir/Debian-Cache" -name 'linux-image-*.deb'|
	while read kernel; do
		eval "kernel_path=\"\${kernel#$Content_Dir/Debian-Cache/}\""
		#ln -s ../../../../Debian-Cache/$(basename "$kernel") "$Content_Dir"/pool/main/l/linux-2.6/$(basename "$kernel")
		ln -s ../../../../Debian-Cache/"$kernel_path" "$Content_Dir"/pool/main/l/linux-2.6/$(basename "$kernel")
	done
fi

echo "Building startup CD for version '$Version'"
sed 's/\%VERSION%/'"$Version"'/g' templates/isolinux.txt.template > "$Content_Dir"/isolinux/isolinux.txt

for i in "${Installer_Files[@]}"; do
	cp "$build_dir"/trunk/src/ConfirmDependencies_Script_Offline/"$i" "$Content_Dir"/Pluto-Install
done

echo "Processing udeb config"
apt-ftparchive generate templates/config-netinst-udeb

echo "Processsing deb config"
apt-ftparchive generate templates/config-netinst-deb

echo "Making md5sums"
pushd "$Content_Dir" >/dev/null
rm md5sum.txt
find -xtype f -exec md5sum '{}' ';' | awk '!/md5sum\.txt/ && !/\.\/isolinux/' >>md5sum.txt
popd >/dev/null

echo "Building iso image $ISO_Dir/installation-cd.iso"
mkisofs -V "$Version" -J -o "$ISO_Dir/installation-cd-1-$Version.iso" -r -b isolinux/isolinux.bin -c isolinux/boot.cat -no-emul-boot -boot-load-size 4 -boot-info-table "$Content_Dir"/

pushd "$ISO_Dir" >/dev/null
md5sum installation-cd-1-$Version.iso >installation-cd-1-$Version.md5
popd >/dev/null

if [[ -f message.txt ]]; then
	(echo "New ISO version: $Version"; echo; cat message.txt) | mail -s "New ISO ready (+info)" "${Mail_Dest[@]}"
	mv message.txt message.txt.prev
else
	echo "New ISO version: $Version" | mail -s "New ISO ready" "${Mail_Dest[@]}"
fi
