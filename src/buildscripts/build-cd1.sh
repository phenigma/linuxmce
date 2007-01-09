#!/bin/bash

for ((i = 1; i <= "$#"; i++)); do
	case "${!i}" in
		--cache) Cache=y ;;
		--iso-dir) ((i++)); ISO_Dir="${!i}" ;;
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

Debian_Mirror="/home/mirrors"
Replacements="/home/samba/repositories/$MakeRelease_Flavor/$replacementsdeb"
MainRepository="/home/samba/repositories/$MakeRelease_Flavor/$maindeb"

CD1_RemovePkgsDir="/old-server/home/radu/pkg/bonuscd-cache/cd1remove/";
CD1_AddPkgsDir="/old-server/home/radu/pkg/bonuscd-cache/cd1add/";

Version=$(basename "$ISO_Dir")

Installer_Files=(BonusCdMenu.sh BonusCdAutoInst.sh Common.sh ConfirmDependencies_Debian.sh Initial_Config.sh Initial_Config_Core.sh Initial_Config_MD.sh Initial_Config_Finish.sh Initial_DHCP_Config.sh Initial_Network_Config.sh pkgsel post-debootstrap.sh preseed.cfg PreseedStage2.sh VIA_Setup.sh post-grub-install.sh)
Mail_Dest=(testing@plutohome.com -c linux-team@plutohome.com)

if [[ "$Cache" == y ]]; then
	rm -rf "image-netinst/Debian-Cache"
	mkdir -p "image-netinst/Debian-Cache"
	
	# Copy CD boot splash
	# TODO: make this per-flavor
	cp image-netinst/isolinux/pluto.rle image-netinst/isolinux/splash.rle

	# TODO: improve this part to do searches, walk dependencies
	cp /old-server/home/radu/pkg/copy/*.deb image-netinst/Debian-Cache
	cp /old-server/home/radu/pkg/missing/*.deb image-netinst/Debian-Cache
	cp "$ISO_Dir"/debian/main/binary-i386/*.deb image-netinst/Debian-Cache
	
	find "$Replacements"/main/binary-i386/ -follow -type f -name '*.deb' -exec cp '{}' image-netinst/Debian-Cache ';'
	find image-netinst/Debian-Cache '(' -name 'kernel-image*' -or -name 'kernel-source*' -or -name 'kernel-headers*' -or -name 'kernel-doc*' ')' -exec rm '{}' ';'
	#find image-netinst/Debian-Cache -name 'linux-image*' -not -name '*2.6.16.20-pluto-2*' -exec rm -f '{}' ';'
	find image-netinst/Debian-Cache '(' -name 'linux-source*' -or -name 'linux-headers*' -or -name 'linux-doc*' ')' -exec rm '{}' ';'
	find image-netinst/Debian-Cache -name 'pluto-src-*' -exec rm '{}' ';'

	find image-netinst/Debian-Cache -name 'pluto-src-*' -exec rm '{}' ';'
	rm image-netints/Debian-Cache/kernel-*
	rm image-netinst/Debian-Cache/pluto-skins-rse*
	rm image-netinst/Debian-Cache/pluto-marbella*
	rm image-netinst/Debian-Cache/pluto-audi*
	#rm image-netinst/Debian-Cache/pluto-sample-media*
	rm image-netinst/Debian-Cache/monster*
	rm image-netinst/Debian-Cache/pluto-monster-database*
	rm image-netinst/Debian-Cache/pluto-skins-bo*
	rm image-netinst/Debian-Cache/libwxgtk2.6-dbg*
	rm image-netinst/Debian-Cache/libwxgtk2.6-dev*
	rm image-netinst/Debian-Cache/python-wx*
	rm image-netinst/Debian-Cache/slim*
	rm image-netinst/Debian-Cache/pluto-slim-server*
	rm image-netinst/Debian-Cache/pluto-slimserver-plugin*
	rm image-netinst/Debian-Cache/video-wizard-videos*
	find image-netinst/Debian-Cache '(' -name '*-dev_*.deb' -not -name '*xutils*' ')' -exec rm '{}' ';'

	rm image-netinst/Debian-Cache/*-dbg_*.deb
	rm image-netinst/Debian-Cache/vdr*.deb
	rm image-netinst/Debian-Cache/vlc*.deb
	rm image-netinst/Debian-Cache/pluto-liblinphone*.deb
	rm image-netinst/Debian-Cache/xfonts*-transcoded*.deb
	rm image-netinst/Debian-Cache/xspecs*.deb
	rm image-netinst/Debian-Cache/xvfb*.deb
	rm image-netinst/Debian-Cache/xnest*.deb
	rm image-netinst/Debian-Cache/xdmx*.deb
	rm image-netinst/Debian-Cache/xlibmesa-dri*.deb
	rm image-netinst/Debian-Cache/*gdb*.deb
	rm image-netinst/Debian-Cache/nvidia-kernel-source*.deb
	rm image-netinst/Debian-Cache/*-src_*.deb
	rm image-netinst/Debian-Cache/*-doc_*.deb
	rm image-netinst/Debian-Cache/wx2.6-doc*.deb
	rm image-netinst/Debian-Cache/pluto-avwizard-sounds*
	rm image-netinst/Debian-Cache/libsdl1.2debian-{arts,esd,nas,oss,all}*
	rm image-netinst/Debian-Cache/wx-common*
	rm image-netinst/Debian-Cache/wx2.6*
	rm image-netinst/Debian-Cache/icewm-lite*
	rm image-netinst/Debian-Cache/pluto-avwizard-skin*
	rm image-netinst/Debian-Cache/bootsplash-theme-monster*
	rm image-netinst/Debian-Cache/bootsplash-theme-linuxmce*
	rm image-netinst/Debian-Cache/*-doc-*.deb
	rm image-netinst/Debian-Cache/qt3-dev-tools*
	rm image-netinst/Debian-Cache/libwxgtk2.6*
	
	# ugly, ugly, ugly, hack
	rm image-netinst/Debian-Cache/pluto-bluetooth-dongle*
	
	# Remove old packages from CD 1
	pushd "$CD1_RemovePkgsDir" >/dev/null
	for i in $(ls); do
		rm -rf /home/installation-cd-kernel-2.6.12/image-netinst/Debian-Cache/"$i"
	done
	popd >/dev/null

	# Add new packages to CD 1
	cp -r "$CD1_AddPkgsDir"/*.deb image-netinst/Debian-Cache

	# add important packages for cd1 to work with offline installation
	#cp "$Replacements"/main/binary-i386/pluto-sample-media*.deb image-netinst/Debian-Cache
	find "$Replacements"/main/binary-i386/ -follow -name "avwizard-sounds-dummy*.deb" -exec cp '{}' image-netinst/Debian-Cache ';'
	cp "$MainRepository"/main/binary-i386/pluto-avwizard-skin-basic*.deb image-netinst/Debian-Cache

	echo "Making cached packages Indexes"
	dpkg-scanpackages image-netinst/Debian-Cache /dev/null 2>/dev/null |
		sed s,image-netinst/Debian-Cache/,dists/sarge/main/binary-i386/,g |
		gzip -9c >image-netinst/Debian-Cache/Packages.gz

	echo "Cleaning up duplicate packages"
	pushd image-netinst/Debian-Cache >/dev/null
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
	rm image-netinst/pool/main/l/linux-2.6/*.deb
	for kernel in image-netinst/Debian-Cache/linux-image-*.deb; do
		ln -s ../../../../Debian-Cache/$(basename "$kernel") image-netinst/pool/main/l/linux-2.6/$(basename "$kernel")
	done
fi

echo "Building startup CD for version '$Version'"
sed 's/\%VERSION%/'"$Version"'/g' templates/isolinux.txt.template > image-netinst/isolinux/isolinux.txt

for i in "${Installer_Files[@]}"; do
	cp "$build_dir"/trunk/src/ConfirmDependencies_Script_Offline/"$i" image-netinst/Pluto-Install
done

echo "Processing udeb config"
apt-ftparchive generate templates/config-netinst-udeb

echo "Processsing deb config"
apt-ftparchive generate templates/config-netinst-deb

echo "Making md5sums"
pushd image-netinst >/dev/null
rm md5sum.txt
find -xtype f -exec md5sum '{}' ';' | awk '!/md5sum\.txt/ && !/\.\/isolinux/' >>md5sum.txt
popd >/dev/null

echo "Building iso image $ISO_Dir/installation-cd.iso"
mkisofs -V "$Version" -J -o "$ISO_Dir/installation-cd-1-$Version.iso" -r -b isolinux/isolinux.bin -c isolinux/boot.cat -no-emul-boot -boot-load-size 4 -boot-info-table image-netinst/

pushd "$ISO_Dir" >/dev/null
md5sum installation-cd-1-$Version.iso >installation-cd-1-$Version.md5
popd >/dev/null

if [[ -f message.txt ]]; then
	(echo "New ISO version: $Version"; echo; cat message.txt) | mail -s "New ISO ready (+info)" "${Mail_Dest[@]}"
	mv message.txt message.txt.prev
else
	echo "New ISO version: $Version" | mail -s "New ISO ready" "${Mail_Dest[@]}"
fi
