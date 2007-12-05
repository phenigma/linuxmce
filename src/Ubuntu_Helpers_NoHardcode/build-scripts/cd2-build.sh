#!/bin/bash

. /etc/lmce-build/builder.conf

# Decide what name to use for the iso
iso_name="$1"
if [[ "$iso_name" == "" ]] ;then
	iso_name="linuxmce-1.1"
fi

# Remove old iso
rm -f "${local_mirror_dir}/${iso_name}-cache.iso"

# Build our new iso
pushd "${build_dir}"
	mkisofs -f -J -r -o "${iso_name}-cache.iso" ./cd2-packages
	mv "${iso_name}-cache.iso" "${local_mirror_dir}"
popd
