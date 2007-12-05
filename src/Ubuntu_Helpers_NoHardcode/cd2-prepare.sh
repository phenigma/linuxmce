#!/bin/bash

. /etc/lmce-build/builder.conf

rm -rf "${build_dir}/cd2-packages"
mkdir -p "${build_dir}/cd2-packages/cachecd1-cache"

pushd "${build_dir}/cd2-packages/cachecd1-cache"
	aptitude download `cat /etc/lmce-build/cd2-packages` || exit 1

	## Video Wizard Videos
	cp "${replacements_dir}"/video-wizard-videos_*.deb ./
	cp "${replacements_dir}"/tee-pluto_*.deb ./
popd

