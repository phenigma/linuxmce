#!/bin/bash

. /etc/lmce-build/builder.conf

rm -rf "${build_dir}/cd2-packages"
mkdir -p "${build_dir}/cd2-packages/cachecd1-cache"

pushd "${build_dir}/cd2-packages/cachecd1-cache"
	aptitude download `cat /root/Ubuntu_Helpers/cd2-packages-whitelist.gutsy` || exit 1

	## Kernels
	aptitude download linux-image-`uname -r`

	## Video Wizard Videos
	cp "${replacements_dir}"/video-wizard-videos_*.deb ./
	cp "${replacements_dir}"/tee-pluto_*.deb ./

	## Packages only i386 compatible
	if [[ "$(uname -m)" != "x86_64" ]] ;then
		aptitude download flashplugin-nonfree
	else
		aptitude download libc6-i386
	fi
popd

