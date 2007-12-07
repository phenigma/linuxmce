#!/bin/bash

. /root/Ubuntu_Helpers/BuildUbuntu_Functions.sh

#Backup_Last_Build
#Install_Build_Needed_Packages
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


echo "select VersionName from Version" | mysql -A -N pluto_main_build  | head -1 > /var/plutobuild/DisklessSync/i386/version

## Wait diskless images for all the architectures
WaitDisklessImage "i386"
WaitDisklessImage "amd64"

## Generate DVD
pushd /root/R/ubuntu-lite-installer/
	./build-cd.sh
popd

echo "Finished"
date -R
read

