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

## Build preinstalled dvd
pushd /root/UnatendedInstall/
	./install_in_vmware.sh
popd
pushd /root/R/ubuntu-lite-installer/
	./build-cd.sh
popd

echo "Finished"
date -R
read

