#!/bin/bash

. /root/Ubuntu_Helpers/BuildUbuntu_Functions.sh

#Create_Diskless_Archive
Import_Pluto_Skins
Checkout_Pluto_Svn
Build_Pluto_Replacements
Build_MakeRelease_Binary
Import_Build_Database
Create_Fake_Windows_Binaries
Build_Pluto_Stuff
Create_Local_Repository
Create_ISO

## Create Diskless and Hybrid Image in VmWare
pushd /root/UnatendedInstall/
	./install_in_vmware.sh
popd

## Get diskless images for all the architectures
SyncDisklessImage "10.0.2.1" "PlutoMD-i386.tar.bz2"
SyncDisklessImage "10.0.2.2" "PlutoMD-amd64.tar.bz2"

## Generate DVD
pushd /root/R/ubuntu-lite-installer/
	./build-cd.sh
popd

echo "Finished"
date -R
read

