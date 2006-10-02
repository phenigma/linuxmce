#!/bin/bash
# this script create versioned bonuscd for Monster build
# edited by VooDoo on 4 aug 2006 - for new cd build

# vars
TYPE="$1"
VERSION="$2"
DEVREPO="/home/samba/repositories/pluto/20dev/main/binary-i386"
REPLREPO="/home/samba/repositories/pluto/replacements/main/binary-i386"
CACHEDIR="/old-server/home/radu/pkg/bonuscd-cache/bonuscd1-cache"
BONUSCDDIR="/home/builds/bonuscd"
AUTOINSTDIR="$BONUSCDDIR/bonuscd1"
BONUSCACHEDIR="$BONUSCDDIR/bonuscd1-cache"
BONUSCDBUILDDIR="/home/samba/builds/$VERSION"
MD5FILE="$BONUSCDDIR/md5.file"

# packages to get from repositories
case "$TYPE" in
	pluto)
		DEVPKGLIST=( pluto-bluetooth-dongle pluto-usb-uirt pluto-slimserver-plugin pluto-slim-server-streamer pluto-irtrans-wrapper pluto-generic-serial-device )
		REPLPKGLIST=( slimdevices-slim-server libxine1-dbg)
		AUTOINSTLIST=( video-wizard-videos-pluto )
	;;
	monster)
		DEVPKGLIST=( monster-skin pluto-avwizard-skin-monster pluto-monster-database pluto-bluetooth-dongle pluto-usb-uirt pluto-slimserver-plugin pluto-slim-server-streamer pluto-irtrans-wrapper pluto-generic-serial-device )
		REPLPKGLIST=( slimdevices-slim-server bootsplash-theme-monster libxine1-dbg )
		AUTOINSTLIST=( video-wizard-videos-monster monster-nucleus )
	;;
	*)
		echo "Unknown build type: '$TYPE'"
		exit 1
	;;
esac

# create iso folders
rm -rf $BONUSCDDIR
mkdir -p $BONUSCDDIR $AUTOINSTDIR

# copy cache folder 
cp -r $CACHEDIR $BONUSCDDIR

# copy packages lists to bonuscd
# 20dev
for pkgs in ${DEVPKGLIST[@]}
do
cp $DEVREPO/$pkgs*.deb $BONUSCACHEDIR 
done
# replacements
for pkgs in ${REPLPKGLIST[@]}
do
	find "$REPLREPO" -name "$pkgs"*.deb -exec cp '{}' "$BONUSCACHEDIR" ';'
done
# autoinstall dir
for pkgs in ${AUTOINSTLIST[@]}
do
cp $DEVREPO/$pkgs*.deb $AUTOINSTDIR
cp $REPLREPO/$pkgs*.deb $AUTOINSTDIR
done

# create md5 file
touch $MD5FILE
cd $AUTOINSTDIR
md5sum *.deb > $MD5FILE
cd $BONUSCACHEDIR 
md5sum *.deb >> $MD5FILE

# create iso 
mkisofs -V "$TYPE BonusCD 1" -J -o $BONUSCDBUILDDIR/installation-cd-2-$VERSION.$TYPE.iso $BONUSCDDIR
md5sum $BONUSCDBUILDDIR/installation-cd-2-$VERSION.$TYPE.iso > $BONUSCDBUILDDIR/installation-cd-2-$VERSION.$TYPE.md5
# clean after iso is finished
rm -rf $BONUSCDDIR

