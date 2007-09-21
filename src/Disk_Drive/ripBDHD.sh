#!/bin/bash

CurrDir=`pwd`
SourceDevice="$1"
DestFolder=`echo -n "$CurrDir/$2" | tr [:space:] _`

MountDir=`mktemp -d /mnt/ripXXXXXX`
mount $SourceDevice $MountDir

#extracting keys
/usr/pluto/tools/aacskeys/extract_keys.sh $MountDir

#ripping files
pushd /usr/pluto/tools/dumphd
#./dumphd.sh $MountDir "$DestFolder"

#rip only couple of files to make it run faster
./dumphd.sh $MountDir/BDMV/STREAM/00017.m2ts "$DestFolder"
./dumphd.sh $MountDir/HVDVD_TS/BLACK.EVO "$DestFolder"

popd

mv "$DestFolder" "$2"

umount $MountDir
rmdir $MountDir

