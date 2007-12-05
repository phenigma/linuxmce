#!/bin/bash -x

. /etc/lmce-build/builder.conf

function Changed_Since_Last_Build {
	local fs_path="$1"
	local cache_file="${replacements_dir}/.cache"
	local url_id=$(svn info "$fs_path" | grep '^URL: ' | cut -d' ' -f2 | md5sum | cut -d' ' -f1)
	local revision_new=$(svn info $fs_path | grep '^Revision: ' | cut -d' ' -f2)
	local revision_old=$(cat "$cache_file" | grep "^${url_id}" | cut -d'|' -f2)
	
	if [[ "$revision_new" == "$revision_old" ]] ;then
		return $(/bin/false)
	fi

	grep -v "$url_id" "$cache_file" > "${cache_file}.tmp"
	echo "$url_id|$revision_new" >> "${cache_file}.tmp"
	mv "${cache_file}.tmp" "$cache_file"

	return $(/bin/true)
}

function Build_Replacements { 
	mkdir -p "$replacements_dir"

	#Package: libsdl
	dir_="${svn_dir}/trunk/ubuntu/libsdl1.2-1.2.12"
	if Changed_Since_Last_Build "$dir_" ;then
		pushd "$dir_"
		dpkg-buildpackage -rfakeroot -us -uc -b
		cp -r ../libsdl1.2debian-pluto*.deb "${replacements_dir}"
		popd
	fi

	#Package: libxine
	dir_="${svn_dir}/trunk/ubuntu/xine-lib-1.1.7"
	if Changed_Since_Last_Build "$dir_" ;then
		pushd "$dir_"
		dpkg-buildpackage -rfakeroot -us -uc -b
		cp -r ../libxine*.deb ${replacements_dir}
		popd
	fi

	#Package: video-wizard-videos
	local vvv_replacements_dir=$(mktemp -d)
	pushd "$vvv_replacements_dir"
		scp -r pluto@10.0.2.4:"/home/samba/www_docs/video\ wizard/video-wizard-videos" ./
		cd "video-wizard-videos"
		dpkg-deb -b . ..
		cp -r ../video-wizard-videos_*.deb ${replacements_dir}
	popd
	rm -rf "$vvv_replacements_dir"

	#Package: tee-pluto
	dir_="${svn_dir}/trunk/misc_utils/tee-pluto"
	if Changed_Since_Last_Build "$dir_" ;then
		pushd "$dir_"
		dpkg-buildpackage -rfakeroot -us -uc -b
		cp ../tee-pluto_*.deb ${replacements_dir}
		popd
	fi
	
	#Package: liblinphone
	dir_="${svn_dir}/trunk/ubuntu/linphone-1.3.5"
	if Changed_Since_Last_Build "$dir_" ;then
		pushd "$dir_"
		dpkg-buildpackage -rfakeroot -us -uc -b
		cp ../liblinphone1-lmce_*.deb ${replacements_dir}
		cp ../libortp4-lmce_*.deb ${replacements_dir}
		popd
	fi

	#Package: pluto-mplayer
	dir_="${svn_dir}/trunk/ubuntu/mplayer"
	if Changed_Since_Last_Build "$dir_" ;then
		pushd "$dir_"
		dpkg-buildpackage -rfakeroot -us -uc -b
		cp -r ../pluto-mplayer_*.deb ${replacements_dir}
		popd
	fi
	
	#Package: pluto-ffmpeg
	dir_="${svn_dir}/trunk/ubuntu/ffmpeg"
	if Changed_Since_Last_Build "$dir_" ;then
		pushd "$dir_"
		dpkg-buildpackage -rfakeroot -us -uc -b
		cp -r ../pluto-ffmpeg_*.deb ${replacements_dir}
		popd
	fi
	
	#Package: ushare
	dir_="${svn_dir}/trunk/ubuntu/ushare-0.9.6"
	if Changed_Since_Last_Build "$dir_" ;then
		pushd "$dir_"
		dpkg-buildpackage -rfakeroot -us -uc -b
		cp -r ../ushare_*.deb ${replacements_dir}
		popd
	fi
	
	#Package: djmount
	dir_="${svn_dir}/trunk/ubuntu/djmount-0.71"
	if Changed_Since_Last_Build "$dir_" ;then
		pushd "$dir_"
		dpkg-buildpackage -rfakeroot -us -uc -b
		cp -r ../djmount_*.deb ${replacements_dir}
		popd
	fi

	#Package: freepbx
	dir_="${svn_dir}/trunk/ubuntu/asterisk/freepbx"
	if Changed_Since_Last_Build "$dir_" ;then
		pushd "$dir_"
		dpkg-buildpackage -rfakeroot -us -uc -b
		cp -r ../freepbx_*.deb ${replacements_dir}
		popd
	fi

	#Package: pluto-asterisk
	dir_="${svn_dir}/trunk/ubuntu/asterisk"
	if Changed_Since_Last_Build "$dir_" ;then
		pushd "$dir_"
		./make_package_ubuntu.sh `uname -r`
		cp -r asterisk-pluto_*.deb ${replacements_dir}
		popd
	fi

	#Package: lshwd
	dir_="${svn_dir}/trunk/ubuntu/lshwd-2.0-rc4"
	if Changed_Since_Last_Build "$dir_" ;then
		pushd "$dir_"
		dpkg-buildpackage -rfakeroot -us -uc -b
		cp ../lshwd_2.0*.deb ${replacements_dir}
		popd
	fi

	#Package: lirc-modules
	pushd .
                cd "${svn_dir}"/trunk/src/Ubuntu_Helpers
                rm -f /etc/lirc/lirc-modules-source.conf
                dpkg-reconfigure -fnoninteractive lirc-modules-source

		pushd /usr/src
			tar jxvf linux-source-`uname -r | cut -d '-' -f1`.tar.bz2
		popd

                mkdir -p /usr/src/modules/lirc/drivers/media/video/bt8xx/
                cp -a /usr/src/linux-source-`uname -r | cut -d '-' -f1`/drivers/media/video/bt8xx/* /lib/modules/`uname -r`/build/drivers/media/video
                cp -a /usr/src/linux-source-`uname -r | cut -d '-' -f1`/drivers/media/video/* /lib/modules/`uname -r`/build/drivers/media/video

                cd /usr/src/linux-source-`uname -r | cut -d '-' -f1`/drivers/media/video
                cp -a btcx-risc.h /usr/src/modules/lirc/drivers/media/video
                cd /usr/src/modules/lirc/drivers
                ln -s ../../../linux-source-2.6.17/drivers/video/ ./  || :
                m-a -ft a-b lirc-modules
                cp /usr/src/lirc-modules*.deb "${replacements_dir}"
	popd
	
	#Package: zaptel-modules
	m-a -ft -l `unamr -r` a-b zaptel
	cp /usr/src/zaptel-modules*.deb "${replacements_dir}"

	#Package: ivtv-modules
	pushd .
		m-a -ft a-b ivtv
		cp /usr/src/ivtv-modules*.deb "${replacements_dir}"
	popd
	
	pushd "${svn_dir}"/trunk/ubuntu/
		Src="deb http://dl.ivtvdriver.org/ubuntu feisty firmware"
		if [ ! -e /etc/apt/sources.list.pbackup ] ;then
			cp /etc/apt/sources.list /etc/apt/sources.list.pbackup
		fi
		if ! grep -qF "$Src" /etc/apt/sources.list; then
			echo "$Src" >> /etc/apt/sources.list
			apt-get update
		fi

		rm -f ivtv-firmware_*.deb
		aptitude download ivtv-firmware
		mkdir ivtv-firmware
		cd ivtv-firmware
		dpkg -x ../ivtv-firmware_*.deb .
		dpkg -e ../ivtv-firmware_*.deb
		rm -f DEBIAN/preinst
		rm -f ../ivtv-firmware_*.deb
		dpkg-deb -b . ..
		cd ..
		rm -rf ivtv-firmware

		cp ivtv-firmware_*.deb "${replacements_dir}"
		rm ivtv-firmware_*.deb
	popd

	#Package: lirc-pluto
	dir_="${svn_dir}"/trunk/ubuntu/lirc-pluto-0.1
	if Changed_Since_Last_Build "$dir_" ;then
		pushd "$dir_"
		dpkg-buildpackage -rfakeroot -us -uc -b
		cp ../lirc-pluto_*.deb "${replacements_dir}"
		popd
	fi
	
	#Package: mce-launcher
	dir_="${svn_dir}"/trunk/src/mce-launcher
	if Changed_Since_Last_Build "$dir_" ;then
		pushd "$dir_"
		dpkg-buildpackage -rfakeroot -us -uc -b
		cp ../mce-launcher_*.deb "${replacements_dir}"
		popd
	fi

	#Package: mce-installer
	dir_="${svn_dir}"/trunk/src/mce-installer
	if Changed_Since_Last_Build "$dir_" ;then
		pushd "$dir_"
		dpkg-buildpackage -rfakeroot -us -uc -b
		cp ../mce-installer_*.deb "${replacements_dir}"
		popd
	fi

	#Package: mtx-pluto
	dir_="${svn_dir}"/trunk/ubuntu/mtx-1.3.11
	if Changed_Since_Last_Build "$dir_" ;then
		pushd "$dir_"
		dpkg-buildpackage -rfakeroot -us -uc -b
		cp ../mtx-pluto_*.deb "${replacements_dir}"
		popd
	fi

	#Package: linux-image-diskless
	dir_="${svn_dir}"/trunk/ubuntu/linux-image-dummy
	if Changed_Since_Last_Build "$dir_" ;then
		pushd "$dir_"
		./makepackage.sh	
		cp linux-image-diskless_*.deb "${replacements_dir}"		
		popd
	fi

#TODO: Figure how we'll do this on that server
#	#Download arch independent packages from 150
#	pushd "$replacements_dir"
#		scp  pluto@10.0.2.4:/home/samba/repositories/replacements-common/libflickr-api-perl_1_all.deb ./
#		scp  pluto@10.0.2.4:/home/samba/repositories/replacements-common/libxml-parser-lite-tree-perl_1_all.deb ./
#		scp  pluto@10.0.2.4:/home/samba/repositories/replacements-common/asterisk-perl_0.08-1_all.deb ./
#		scp  pluto@10.0.2.4:/home/samba/repositories/replacements-common/pluto-avwizard-sounds_1.0-1_all.deb ./
#		scp  pluto@10.0.2.4:/home/samba/repositories/replacements-common/msttcorefonts_2.2pluto1_all.deb ./
#	popd 

}


Build_Replacements
