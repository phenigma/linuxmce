#!/bin/bash

rm -rf /var/plutobuild/cd2-packages
mkdir -p /var/plutobuild/cd2-packages
mkdir -p /var/plutobuild/cd2-packages/cachecd1-cache

pushd /var/plutobuild/cd2-packages/cachecd1-cache
	aptitude download `cat /root/Ubuntu_Helpers/cd2-packages-whitelist`

	## Kernels
	aptitude download linux-image-2.6.20-15-generic


	## Video Wizard Videos
	cp /var/www/video-wizard-videos_*.deb ./

	## Packages only i386 compatible
	if [[ "$(uname -m)" != "x86_64" ]] ;then
		aptitude download flashplugin-nonfree
	fi
popd

