#!/bin/bash

rm -rf /var/plutobuild/cd2-packages
mkdir -p /var/plutobuild/cd2-packages
mkdir -p /var/plutobuild/cd2-packages/cachecd1-cache

pushd /var/plutobuild/cd2-packages/cachecd1-cache
	aptitude download `cat /root/Ubuntu_Helpers/cd2-packages-whitelist.gutsy` || exit 1

	## Kernels
	aptitude download linux-image-`uname -r`


	## Video Wizard Videos
	cp /var/plutobuild/replacements/video-wizard-videos_*.deb ./
	cp /var/plutobuild/replacements/tee-pluto_*.deb ./

	## Packages only i386 compatible
	if [[ "$(uname -m)" != "x86_64" ]] ;then
		aptitude download flashplugin-nonfree
	else
		aptitude download libc6-i386
	fi
popd

