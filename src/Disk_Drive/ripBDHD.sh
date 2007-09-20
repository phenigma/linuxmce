#!/bin/bash

SourceDevice="$1"
DestFolder="$2"

MountDir=`mktemp -d /mnt/ripXXXXXX`
mount $SourceDevice $MountDir

#extracting keys
/usr/pluto/tools/aacskeys/extract_keys.sh $MountDir

#ripping files
pushd /usr/pluto/tools/dumphd
#./dumphd.sh $MountDir $DestFolder
./dumphd.sh $MountDir/BDMV/STREAM/00017.m2ts $DestFolder
./dumphd.sh $MountDir/HVDVD_TS/BLACK.EVO $DestFolder

popd

umount $MountDir
rmdir $MountDir

