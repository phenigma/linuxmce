#!/bin/bash

. /home/WorkNew/src/MakeRelease/MR_Conf.sh

set -ex

if [[ -z "$MakeRelease_Flavor" ]]; then
	if [[ -z "$1" ]]; then
		exit 1
	fi
	MR_ConfEval "$1"
	export MakeRelease_Flavor="$1"
fi

# variables taken from environment:
# MakeRelease_Flavor
# MakeRelease_Kernel

Packages=(nvidia lirc asterisk kernel_upgrade)

PackageDir_nvidia=nvidia-graphics-drivers-1.0.9755
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
	./"$Cmd" ${MakeRelease_Kernel}
	popd >/dev/null
done

if [[ "$MakeRelease_Flavor" == via ]]; then
	Packages=(via_xorg)
	PackageDir_via_xorg=VIA_X_drivers
	PackageCmd_via_xorg="BuildViaXorg.sh"
	Pkg="via_xorg"

	pushd "$build_dir"/private/trunk/src/"$PackageDir_via_xorg" >/dev/null
	Var="PackageCmd_$Pkg"
	Cmd="${!Var}"
	./"$Cmd" ${MakeRelease_Kernel}
	popd >/dev/null
fi
