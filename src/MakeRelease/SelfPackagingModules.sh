#!/bin/bash

set -ex

# variables taken from environment:
# MakeRelease_Flavor
# MakeRelease_Kernel

Packages=(nvidia lirc asterisk)

PackageDir_nvidia=nvidia-graphics-drivers-1.0.8774
PackageDir_lirc=lirc
PackageDir_asterisk=asterisk

PackageCmd_nvidia="build.sh"
PackageCmd_lirc="build.sh"
PackageCmd_asterisk="make_package.sh"

for Pkg in "${Packages[@]}"; do
	Var="PackageDir_$Pkg"
	Dir="${!Var}"
	
	pushd /home/MakeRelease/trunk/external/"$Dir" >/dev/null

	Var="PackageCmd_$Pkg"
	Cmd="${!Var}"
	./"$Cmd" "$MakeRelease_Flavor" "${MakeRelease_Kernel[@]}"
	popd >/dev/null
done
