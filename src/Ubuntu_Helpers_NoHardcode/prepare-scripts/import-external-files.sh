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
		wget -c "http://82.77.255.209/extra/$file"
	done
popd
