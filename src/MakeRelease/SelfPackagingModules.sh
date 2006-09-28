#!/bin/bash

. /home/WorkNew/src/MakeRelease/MR_Conf.sh

set -ex

if [[ -z "$MakeRelease_Flavor" ]]; then
	if [[ -z "$1" ]]; then
		exit 1
	fi
	ConfEval "$1"
	export MakeRelease_Flavor="$1"
fi

# variables taken from environment:
# MakeRelease_Flavor
# MakeRelease_Kernel

Packages=(nvidia lirc asterisk kernel_upgrade)

PackageDir_nvidia=nvidia-graphics-drivers-1.0.8774
PackageDir_lirc=lirc
PackageDir_asterisk=asterisk
PackageDir_kernel_upgrade=pluto-kernel-upgrade

PackageCmd_nvidia="build.sh"
PackageCmd_lirc="build.sh"
PackageCmd_asterisk="make_package.sh"
PackageCmd_kernel_upgrade="BuildKernelUpgrade.sh"

for Pkg in "${Packages[@]}"; do
	Var="PackageDir_$Pkg"
	Dir="${!Var}"
	
	pushd "$build_dir"/trunk/external/"$Dir" >/dev/null

	Var="PackageCmd_$Pkg"
	Cmd="${!Var}"
	env
	./"$Cmd" "$MakeRelease_Flavor" ${MakeRelease_Kernel}
	popd >/dev/null
done
