#!/bin/bash

. /etc/lmce-build/builder.conf

## Video Wizard Videos
mkdir -p "${extras_dir}"
pushd "${extras_dir}"
	for file in \
	       	asterisk-perl_0.08-1_all.deb \
		libxml-parser-lite-tree-perl_1_all.deb \
		pluto-avwizard-sounds_1.0-1_all.deb \
		libflickr-api-perl_1_all.deb \
	       	msttcorefonts_2.2pluto1_all.deb \
		video-wizard-videos_1.1_all.deb \
	;do
		:
	wget -c "http://82.77.255.209/extra/$file"
	done
popd

## Some Skins
skins_dir=/home/samba/www_docs/graphics

mkdir -p /usr/pluto/orbiter/
rm -f /usr/pluto/orbiter/skins
ln -s "$skins_dir" /usr/pluto/orbiter/skins

rm -rf "$skins_dir"
mkdir -p "$skins_dir"

pushd /
	ssh -i /etc/lmce-build/builder.key pluto@82.77.255.209 tar -c "$skins_dir" | tar -x
popd

pushd /usr/pluto/orbiter/skins
        cp -r LinuxMCE/* Basic/
popd

pushd "${build_dir}"
        ln -s /home/samba
popd
	
mkdir -p '/home/samba/www_docs/sample media/'
pushd '/home/samba/www_docs/sample media/'
	scp -r -i /etc/lmce-build/builder.key pluto@82.77.255.209:'/home/samba/www_docs/sample\ media/' ./
popd
