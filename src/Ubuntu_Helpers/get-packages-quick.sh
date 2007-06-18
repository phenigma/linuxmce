#!/bin/bash
iso_name="$1"
if [[ "$iso_name" == "" ]] ;then
	iso_name="linuxmce-1.1"
fi

if [ ! -e /var/plutobuild/cache-cd/deb-cache ] ;then
	mkdir -p /var/plutobuild/cache-cd/deb-cache
else
	rm -f /var/plutobuild/cache-cd/deb-cache/*
	rm -f /var/plutobuild/cache-cd/mce-installer*.deb
fi
rm -f /var/www/$iso_name-packages.iso

#ls /var/ubuntu/| grep -v pluto | xargs -i -t cp /var/ubuntu/'{}' /var/plutobuild/cache-cd/deb-cache
#find /var/www/ -maxdepth 1 -type f -not -name 'pluto-src-*' -not -name 'Packages*' -print0 | xargs -0 -i -t cp '{}' /var/plutobuild/cache-cd/deb-cache
find /var/www/ -maxdepth 1 -type f -name '*.deb' -print0 | xargs -0 -i -t cp '{}' /var/plutobuild/cache-cd/deb-cache

while read file; do
	rm -f "/var/plutobuild/cache-cd/deb-cache/$file"_*.deb
done </root/Ubuntu_Helpers/RemoveFromCD.cfg

ExtraPacks="libcucul0 libcaca0 libmad0 libxine1-ffmpeg xine-ui"

for pack in $ExtraPacks ;do
	cp /var/ubuntu/cachecd1-cache/$pack*  /var/plutobuild/cache-cd/deb-cache
done

for pack in /var/moved-from-cache/* ;do
	cp $pack /var/plutobuild/cache-cd/deb-cache
done

pushd /var/plutobuild/cache-cd/deb-cache
	dpkg-scanpackages . /dev/null > Packages
	cat Packages | gzip -9c > Packages.gz
popd

pushd /var/plutobuild/
cp /var/www/mce-installer*.deb ./cache-cd/
#if [[ -z "$Version" ]]; then
#	sql_slave_db="pluto_main_build"
#	export Version=$(echo "select VersionName from Version" | mysql $sql_slave_db | tail -1);
#fi
#mkisofs -f -J -r -o "${iso_name}-packages.iso" ./cache-cd
mkisofs -f -joliet-long -r -o "${iso_name}-packages.iso" ./cache-cd
mv "${iso_name}-packages.iso" /var/www
popd
