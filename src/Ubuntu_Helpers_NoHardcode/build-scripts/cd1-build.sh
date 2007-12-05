#!/bin/bash

. /etc/lmce-build/builder.conf

# Decide on the name that the iso will have
iso_name="$1"
if [[ "$iso_name" == "" ]] ;then
	iso_name="linuxmce-1.1"
fi

# Prepare the directory in whith the cd will be built
if [ ! -e ${build_dir}/cache-cd/deb-cache ] ;then
	mkdir -p ${build_dir}/cache-cd/deb-cache
else
	rm -f ${build_dir}/cache-cd/deb-cache/*
	rm -f ${build_dir}/cache-cd/mce-installer*.deb
fi

# Remove old iso
rm -f /var/www/$iso_name-packages.iso

# Copy the debs that where created in /var/www on this cd
find /var/www/ -maxdepth 1 -type f -name '*.deb' -print0 | xargs -0 -i -t cp '{}' "${build_dir}/cache-cd/deb-cache"


# Remove blacklisted packages from this cd
while read file; do
	rm -f "${build_dir}/cache-cd/deb-cache/$file"_*.deb
done </etc/lmce-build/cd1-packages-blacklist

# Add some extra packages to this cd
for pack in "${build_dir}/cd1-packages/"*.deb ;do
	cp "$pack" "${build_dir}/cache-cd/deb-cache"
done

# Generate the iso
pushd "${build_dir}"
	cp /var/www/mce-installer*.deb ./cache-cd/
	mkisofs -f -joliet-long -r -o "${iso_name}-packages.iso" ./cache-cd
	mv "${iso_name}-packages.iso" /var/www
popd

## Clean the temporary dir
rm -rf "${build_dir}/cache-cd"
