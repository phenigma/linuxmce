#!/bin/bash

for ((i = 1; i <= "$#"; i++)); do
	case "${!i}" in
		--cache) Cache=y ;;
		--iso-dir) ((i++)); ISO_Dir="${!i}" ;;
	*) echo "ERROR: Unknown parameter '${!i}'" ;;
	esac
done

Version=$(basename "$ISO_Dir")

if [[ "$ISO_Dir" = "" ]] ;then
	echo "ERROR: Param --iso-dir is not mandatory"
	exit 1
fi

if [[ "$MakeRelease_Flavor" == "" ]] ;then
	echo "ERROR: MakeRelease_Flavor undefined"
	exit 1
fi

if [[ "$maindeb" == "" ]] ;then
	echo "ERROR: maindeb undefined"
	exit 1
fi

if [[ "$replacementsdeb" == "" ]] ;then
	echo "ERROR: replacementsdeb undefined"
	exit 1
fi


DEVREPO="/home/samba/repositories/$MakeRelease_Flavor/$maindeb/main/binary-i386"
REPLREPO="/home/samba/repositories/$MakeRelease_Flavor/$replacementsdeb/main/binary-i386"
CACHEDIR="/old-server/home/radu/pkg/bonuscd-cache/bonuscd1-cache"
BONUSCDDIR="/home/builds/bonuscd"
AUTOINSTDIR="$BONUSCDDIR/bonuscd1"
BONUSCACHEDIR="$BONUSCDDIR/bonuscd1-cache"
MD5FILE="$BONUSCDDIR/md5.file"

# packages to get from repositories
case "$MakeRelease_Flavor" in
    pluto|pluto_debug|via)
        DEVPKGLIST=( pluto-bluetooth-dongle pluto-usb-uirt pluto-slimserver-plugin pluto-slim-server-streamer pluto-irtrans-wrapper pluto-generic-serial-device pluto-tira-wrapper pluto-powerfile-c200 )
        REPLPKGLIST=( slimdevices-slim-server libxine1-dbg)
        AUTOINSTLIST=( video-wizard-videos-pluto )
    ;;
    monster)
        DEVPKGLIST=( monster-skin pluto-avwizard-skin-monster pluto-monster-database pluto-bluetooth-dongle pluto-usb-uirt pluto-slimserver-plugin pluto-slim-server-streamer pluto-irtrans-wrapper pluto-generic-serial-device pluto-tira-wrapper pluto-powerfile-c200 )	
		REPLPKGLIST=( slimdevices-slim-server bootsplash-theme-monster libxine1-dbg )
        AUTOINSTLIST=( video-wizard-videos-monster monster-nucleus )
    ;;
    *)
        echo "Unknown build type: '$MakeRelease_Flavor'"
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
for pkgs in ${DEVPKGLIST[@]} ;do
	echo "DEV => CACHE [$pkgs]" 
	cp $DEVREPO/$pkgs*.deb $BONUSCACHEDIR
done

# replacements
for pkgs in ${REPLPKGLIST[@]}
do
	echo "REPLACE => CACHE [$pkgs]"
    find "$REPLREPO" -follow -name "${pkgs}*.deb" -exec cp '{}' "$BONUSCACHEDIR" ';'
done

# autoinstall dir
for pkgs in ${AUTOINSTLIST[@]} ;do
	echo "ANY => AUTOINST [$pkgs]"
	find "$DEVREPO" -follow -name "${pkgs}*.deb" -exec cp '{}' "$AUTOINSTDIR" ";"
	find "$REPLREPO" -follow -name "${pkgs}*.deb" -exec cp '{}' "$AUTOINSTDIR" ";"
done

if [[ "$MakeRelease_Flavor" == via ]]; then
	chmod +x /home/users/jason/*.sh
	cp /home/users/jason/* "$BONUSCACHEDIR"
fi

# create md5 file
touch $MD5FILE
cd $AUTOINSTDIR
md5sum *.deb > $MD5FILE
cd $BONUSCACHEDIR
md5sum *.deb >> $MD5FILE

# create iso
mkisofs -V "Installation CD 2" -J -input-charset iso-8859-1 -o $ISO_Dir/installation-cd-2-$Version.iso $BONUSCDDIR

pushd "$ISO_Dir" >/dev/null
md5sum installation-cd-2-$Version.iso >installation-cd-2-$Version.md5
popd >/dev/null
# clean after iso is finished
rm -rf $BONUSCDDIR

