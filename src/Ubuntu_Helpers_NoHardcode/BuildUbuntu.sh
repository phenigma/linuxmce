#!/bin/bash

. /root/Ubuntu_Helpers/BuildUbuntu_Functions.sh

Build_MakeRelease_Binary
Import_Build_Database
Build_Pluto_Stuff

## Create Diskless and Hybrid Image in VmWare
pushd /root/UnatendedInstall/
	./install_in_vmware.sh
popd

## Generate DVD
pushd /root/R/ubuntu-lite-installer/
	./build-cd.sh
popd

echo "Finished"
date -R
read

