#!/bin/bash -x

flavor="ubuntu"

if [[ $build_dir == "" || $local_mirror_dir == "" ]] ;then
	build_dir="/var/plutobuild"
	local_mirror_dir="/var/www"
fi

svn_dir="${build_dir}/svn"
svn_url="http://10.0.0.170/"

sql_master_host="10.0.0.150"
sql_master_db="pluto_main"
sql_master_db_media="pluto_media"
sql_master_db_security="pluto_security"
sql_master_db_telecom="pluto_telecom"
sql_master_user="root"

sql_slave_host="127.0.0.1"
sql_slave_db="pluto_main_build"
sql_slave_db_mainsqlcvs="main_sqlcvs_$flavor"
sql_slave_db_mythsqlcvs="myth_sqlcvs_$flavor"
sql_slave_db_media="pluto_media"
sql_slave_db_security="pluto_security"
sql_slave_db_telecom="pluto_telecom"
sql_slave_user="root"

replacements_dir="${build_dir}/replacements"
out_dir="${build_dir}/out"
mkr_dir="${build_dir}/MakeRelease"

export Version=$(echo "select VersionName from Version" | mysql $sql_slave_db | tail -1);


function Install_Build_Needed_Packages {
	local pkgs="subversion build-essential dh-make libmysqlclient12-dev libhttpfetcher-dev libattr1-dev libdbus-1-dev libdbus-glib-1-dev libhal-dev libdancer-xml0-dev libbluetooth2-dev libid3-3.8.3-dev libxine-dev x11proto-core-dev libx11-dev libx11-dev x11proto-core-dev x11proto-xext-dev x11proto-xf86vidmode-dev libx11-dev libjpeg62-dev libcdparanoia0-dev libsdl1.2-dev libsdl-gfx1.2-dev libxmu-headers x11proto-record-dev libhid-dev libusb-dev libsdl-image1.2-dev libsdl-ttf2.0-dev libsdl-sge-dev libxtst-dev libxrender-dev liblinphone1-dev libcddb-dev libdvdread-dev libcurl3-dev ruby1.8-dev swig libtcltk-ruby mysql-client mysql-server"
	local pkg
	for pkg in $pkgs ;do
		apt-get -y install $pkg
	done
}


function Checkout_Pluto_Svn {
	local Branch="${1:-trunk}"

	[[ -d $svn_dir ]] && mkdir -p $svn_dir
	rm -rf ${svn_dir}/trunk
	
	for svn_module in src ubuntu web misc_utils installers config-pkgs;do
		mkdir -p ${svn_dir}/trunk/$svn_module
		svn co ${svn_url}/pluto/"$Branch"/$svn_module  ${svn_dir}/trunk/$svn_module
	done

	cp -f /root/images-pluto-admin/*.jpg ${svn_dir}/trunk/web/pluto-admin/include/images/
	cp -f /root/images-pluto-admin/generic_xml_error_linuxmce.png ${svn_dir}/trunk/web/pluto-admin/security_images/generic_xml_error.png

	sed -i "s,\\\$wikiHost=.*\$,\$wikiHost='http://wiki.linuxmce.com/';,g" ${svn_dir}/trunk/web/plutohome-com/globalconfig/localconfig.inc.php

	#/bin/sql2cpp -h localhost -u root -D pluto_main
	pushd ${svn_dir}/trunk/src
	svn co --username automagic --password "$(</etc/pluto/automagic.pwd)" ${svn_url}/pluto-private/"$Branch"/src/ZWave/
	
	svn co --username automagic --password "$(</etc/pluto/automagic.pwd)" ${svn_url}/pluto-private/"$Branch"/src/RFID_Interface/
	popd
}


function Build_MakeRelease_Binary {
	pushd ${svn_dir}/trunk/src/pluto_main
	make
	popd

	pushd ${svn_dir}/trunk/src/PlutoUtils
	SNR_CPPFLAGS="" make
	popd

	pushd ${svn_dir}/trunk/src/SerializeClass
	SNR_CPPFLAGS="" make
	popd

	pushd ${svn_dir}/trunk/src/DCE
	SNR_CPPFLAGS="" make
	popd

	pushd ${svn_dir}/trunk/src/MakeRelease
	make -f Makefile.MakeRelease
	popd

	pushd ${svn_dir}/trunk/src/MakeRelease_PrepFiles
	make
	popd

	mkdir -p ${mkr_dir}
	cp ${svn_dir}/trunk/src/bin/MakeRelease ${mkr_dir}
	cp ${svn_dir}/trunk/src/bin/MakeRelease_PrepFiles ${mkr_dir}
	cp ${svn_dir}/trunk/src/lib/*.so ${mkr_dir}
}

function Build_Pluto_Replacements {
	temp_dir="${replacements_dir}"
	mkdir -p $temp_dir

	#Package: libsdl-pluto
	apt-get -y install quilt nasm libxv-dev libarts1-dev debhelper fakeroot
	apt-get -y install dbs libaa1-dev libslang2-dev xlibs-dev libsvga1-dev type-handling automake1.7
	pushd ${svn_dir}/trunk/ubuntu/libsdl1.2-1.2.7+1.2.8cvs20041007/
		dpkg-buildpackage -us -uc -rfakeroot -b
		dpkg -i ../libsdl1.2debian-pluto-all_1.2.7+1.2.8cvs20041007+pluto-2_i386.deb
		dpkg -i ../libsdl1.2debian-pluto_1.2.7+1.2.8cvs20041007+pluto-2_i386.deb
		dpkg -i ../libsdl1.2-pluto-dev_1.2.7+1.2.8cvs20041007+pluto-2_i386.deb
		cp ../libsdl1.2*-pluto-*.deb ${temp_dir}
	popd

	#Package: libxine
	apt-get -y install libcaca-dev liblircclient-dev libtheora-dev libflac-dev libmodplug-dev libgnomevfs2-dev libsmbclient-dev libspeex-dev libmad0-dev libxvmc-dev automake1.9 autoconf libtool libcdio-dev sgmltools-lite dpatch transfig libavformat-dev libpostproc-dev libavcodec-dev libraw1394-dev libdc1394-13-dev
	pushd ${svn_dir}/trunk/ubuntu/xine-lib-1.1.3
		dpkg-buildpackage -rfakeroot -us -uc -b
		dpkg -i ../libxine1-pluto_1.1.3*_i386.deb
		dpkg -i ../libxine-pluto-dev_1.1.3*_i386.deb
		cp ../libxine*.deb ${temp_dir}
	popd

	#Package: pluto-asterisk
	apt-get -y install linux-headers-`uname -r`
	pushd ${svn_dir}/trunk/ubuntu/asterisk
		./make_package_ubuntu.sh `uname -r`
		cp -r asterisk-pluto_*.deb ${temp_dir}
	popd

	#Package: lshwd
	pushd ${svn_dir}/trunk/ubuntu/lshwd-2.0-rc4
		dpkg-buildpackage -rfakeroot -us -uc -b
		cp ../lshwd_2.0*.deb ${temp_dir}
	popd

	#Package: lirc-modules
	apt-get -y install linux-source linux-headers module-assistant lirc-modules-source
	pushd .
                cd "${svn_dir}"/trunk/src/Ubuntu_Helpers
                ./Preseed_lirc-modules-source.sh
                rm -f /etc/lirc/lirc-modules-source.conf
                dpkg-reconfigure -fnoninteractive lirc-modules-source

                mkdir -p /usr/src/modules/lirc/drivers/media/video/bt8xx/
                cp -a /usr/src/linux-source-`uname -r | cut -d '-' -f1`/drivers/media/video/bt8xx/* /lib/modules/`uname -r`/build/drivers/media/video
                cp -a /usr/src/linux-source-`uname -r | cut -d '-' -f1`/drivers/media/video/* /lib/modules/`uname -r`/build/drivers/media/video

                cd /usr/src/linux-source-`uname -r | cut -d '-' -f1`/drivers/media/video
                cp -a btcx-risc.h /usr/src/modules/lirc/drivers/media/video
                cd /usr/src/modules/lirc/drivers
                ln -s ../../../linux-source-2.6.17/drivers/video/ ./  || :
                m-a -ft a-b lirc-modules
                cp /usr/src/lirc-modules*.deb "${temp_dir}"
	popd

	#Package: ivtv-modules
	apt-get -y install ivtv-source
	pushd .
		m-a -ft a-b ivtv
		cp /usr/src/ivtv-modules*.deb "${temp_dir}"
		Src="deb http://dl.ivtvdriver.org/ubuntu edgy firmware"
		if ! grep -qF "$Src" /etc/apt/sources.list; then
			echo "$Src" >> /etc/apt/sources.list
			apt-get update
		fi
		aptitude download ivtv-firmware
		cp ivtv-firmware_*.deb "${temp_dir}"
		rm ivtv-firmware_*.deb
	popd

	#Package: lirc-pluto
	pushd "${svn_dir}"/trunk/ubuntu/lirc-pluto-0.1
		dpkg-buildpackage -rfakeroot -us -uc -b
		cp ../lirc-pluto_*.deb "${temp_dir}"
	popd

	#Package: mce-launcher
	pushd "${svn_dir}"/trunk/src/mce-launcher
		dpkg-buildpackage -rfakeroot -us -uc -b
		cp ../mce-launcher_*.deb "${temp_dir}"
	popd

	#Package: mce-installer
	pushd "${svn_dir}"/trunk/src/mce-installer
		dpkg-buildpackage -rfakeroot -us -uc -b
		cp ../mce-installer_*.deb "${temp_dir}"
	popd

	#Package: mtx-pluto
	pushd "${svn_dir}"/trunk/ubuntu/mtx-1.3.10
		dpkg-buildpackage -rfakeroot -us -uc -b
		cp ../mtx-pluto_*.deb "${temp_dir}"
	popd

	#Download arch independent packages from 150
	pushd $temp_dir
		wget -c http://10.0.0.163/debian/dists/replacements/main/binary-i386/replacements-common/libflickr-api-perl_1_all.deb
		wget -c http://10.0.0.163/debian/dists/replacements/main/binary-i386/replacements-common/libxml-parser-lite-tree-perl_1_all.deb
		wget -c http://10.0.0.163/debian/dists/replacements/main/binary-i386/replacements-common/pluto-sample-media_3_i386.deb
		wget -c http://10.0.0.163/debian/dists/replacements/main/binary-i386/replacements-common/asterisk-perl_0.08-1_i386.deb
#		wget -c http://10.0.0.163/debian/dists/replacements/main/binary-i386/replacements-common/video-wizard-videos-pluto_1.1_i386.deb
		wget -c http://10.0.0.163/debian/dists/replacements/main/binary-i386/replacements-common/pluto-avwizard-sounds_1.0-1_i386.deb
		wget -c http://10.0.0.163/debian/dists/replacements/main/binary-i386/replacements-common/Pluto/tee-pluto_1.0_i386.deb
		wget -c http://10.0.0.163/debian/dists/replacements/main/binary-i386/replacements-common/slimdevices-slim-server_6.2.2-2_i386.deb
		wget -c http://10.0.0.163/debian/dists/replacements/main/binary-i386/replacements-common/msttcorefonts_1.2.pluto.4_all.deb
	popd 

	#Put the extrapackages on the cd and repo
	cp /var/plutobuild/extra/*.deb $temp_dir
}

function Build_Pluto_Stuff_Compile {
	:
}

function Build_Pluto_Stuff_Debs {
	:
}

function Build_Pluto_Stuff {
	touch /home/README.Devel.Dependencies

	export PATH=/usr/lib/ccache:$PATH:${svn_dir}/trunk/src/bin
	export LD_LIBRARY_PATH="$mkr_dir:${svn_dir}/trunk/src/lib"

	SVNrevision=$(svn info "${svn_dir}"/trunk |grep ^Revision | cut -d" " -f2)

	MakeRelease="${mkr_dir}/MakeRelease"
	MakeRelease_PrepFiles="${mkr_dir}/MakeRelease_PrepFiles"
	$MakeRelease_PrepFiles -p ${svn_dir}/trunk -e "*.cpp,*.h,Makefile*,*.php,*.sh,*.pl,*.awk" -c ${build_dir}/${flavor}.conf

	rm -rf $out_dir
	mkdir -p $out_dir
#	336,337 - IVTV Video Drivers
#	589,590 - Alsa Sound Drivers
# 	515,516 - PWC Camera Drivers
#	432  	Pluto VideoLan Client
#	431 	Pluto VideoLan Client Source
#	427 	Pluto VideoLan Plugin
#	426 	Pluto VideoLan Plugin Source
#	430 	Pluto VideoLan Server
#	429 	Pluto VideoLan Server Source
#	462  	Pluto ATI Video Drivers
#	607 	Pluto ATI Video Drivers Source
#	543  	Pluto vloopback Kernel Module
#	542 	Pluto vloopback Kernel Module Source
# 	-b

	$MakeRelease -R "$SVNrevision" -h $sql_slave_host -u $sql_slave_user -O $out_dir -D $sql_slave_db -o 1 -r 21 -m 1 -K "543,542,462,607,432,431,427,426,430,429,336,337,589,590,515,516"  -s "${svn_dir}/trunk" -n / > >(tee -a $build_dir/Build.log)  -d
}

function Create_Fake_Windows_Binaries {
	touch ${svn_dir}/trunk/src/bin/Pluto_S60.sis
	touch ${svn_dir}/trunk/src/bin/Orbiter.CAB
	touch ${svn_dir}/trunk/src/bin/Orbiter_Treo.CAB

	touch ${svn_dir}/trunk/src/bin/UpdateBinary.exe
	touch ${svn_dir}/trunk/src/bin/UpdateBinaryCE.exe 
	touch ${svn_dir}/trunk/src/bin/UpdateBinaryCE_x86.exe
	touch ${svn_dir}/trunk/src/bin/{Orbiter_Win32.dat,Orbiter_CeNet4_XScale.dat}
	touch "${svn_dir}/trunk/src/bin/Symbian Series 60 mobile.vmc"
	touch "${svn_dir}/trunk/src/bin/Windows Mobile Smartphone.vmc"
	touch ${svn_dir}/trunk/src/bin/Orbiter_CeNet4_x86.dat

	touch ${svn_dir}/trunk/src/bin/OrbiterInstaller.msi
	touch ${svn_dir}/trunk/src/bin/OrbiterCE_SDL.CAB
	touch ${svn_dir}/trunk/src/bin/Orbiter_CeNet4_XScale.CAB
	touch ${svn_dir}/trunk/src/bin/Orbiter_CeNet4_x86.CAB
	touch ${svn_dir}/trunk/src/bin/ImportContacts.zip

	touch ${svn_dir}/trunk/src/bin/Orbiter.exe
	touch ${svn_dir}/trunk/src/bin/Orbiter_CeNet4_XScale.exe
	touch ${svn_dir}/trunk/src/bin/OrbiterSmartphone.exe
	touch ${svn_dir}/trunk/src/bin/OrbiterCE_SDL.exe
	touch ${svn_dir}/trunk/src/bin/Orbiter_CeNet4_x86.exe

	touch ${svn_dir}/trunk/src/bin/AYGSHELL.DLL
	touch ${svn_dir}/trunk/src/bin/PthreadsCE.dll
	touch ${svn_dir}/trunk/src/bin/Orbiter.MD5
	touch ${svn_dir}/trunk/src/bin/logo.gif

	touch ${svn_dir}/trunk/src/bin/PlutoBaSInstaller.msi
	touch ${svn_dir}/trunk/src/bin/PlutoRebootSetup.msi


	pushd ${svn_dir}/trunk/src/bin
	scp root@10.0.0.150:/home/builds/Windows_Output_LinuxMCE/src/bin/* ./
	popd

	pushd ${svn_dir}/trunk/src/lib
	scp root@10.0.0.150:/home/builds/Windows_Output_LinuxMCE/src/lib/* ./
	popd
}

function Create_Local_Repository {
	rm -rf $local_mirror_dir
	mkdir -p $local_mirror_dir

	cp ${out_dir}/tmp/*.deb $local_mirror_dir
	cp ${replacements_dir}/*.deb $local_mirror_dir
	pushd $local_mirror_dir
		dpkg-scanpackages . /dev/null > Packages
		cat Packages | gzip -9c > Packages.gz
	popd
	ln -s . $local_mirror_dir/ubuntu
}

function Import_Build_Database {
	local temp_sqlcvsdir=$(mktemp -d)
	local temp_file=$(mktemp)
	local temp_file_main=$(mktemp)
	local temp_file_myth=$(mktemp)
	local temp_file_media=$(mktemp)
	local temp_file_security=$(mktemp)
	local temp_file_telecom=$(mktemp)

	## Import sqlcvs repositories from plutohome.com
	ssh uploads@plutohome.com "
                set -x;
                rm -f /tmp/main_sqlcvs.dump /tmp/myth_sqlcvs /home/uploads/sqlcvs_dumps.tar.gz;
                mysqldump --quote-names --allow-keywords --add-drop-table -u root -pmoscow70bogata main_sqlcvs > /tmp/main_sqlcvs.dump;
                mysqldump --quote-names --allow-keywords --add-drop-table -u root -pmoscow70bogata myth_sqlcvs > /tmp/myth_sqlcvs.dump;
                cd /tmp;
                tar zcvf /home/uploads/sqlcvs_dumps.tar.gz main_sqlcvs.dump myth_sqlcvs.dump"
        scp uploads@plutohome.com:/home/uploads/sqlcvs_dumps.tar.gz $temp_sqlcvsdir
        pushd $temp_sqlcvsdir
	        tar zxvf sqlcvs_dumps.tar.gz
		mv main_sqlcvs.dump $temp_file_main
		mv myth_sqlcvs.dump $temp_file_myth 
	popd
	## Import other databases from 150
	mysqldump -h $sql_master_host -u $sql_master_user $sql_master_db > $temp_file        
	mysqldump -h $sql_master_host -u $sql_master_user $sql_master_db_media > $temp_file_media
	mysqldump -h $sql_master_host -u $sql_master_user $sql_master_db_security > $temp_file_security
	mysqldump -h $sql_master_host -u $sql_master_user $sql_master_db_telecom > $temp_file_telecom

	export LD_LIBRARY_PATH="$mkr_dir:${svn_dir}/trunk/src/lib"
	MakeRelease_PrepFiles="${mkr_dir}/MakeRelease_PrepFiles"
	$MakeRelease_PrepFiles -p $(dirname $temp_file) -e "$(basename $temp_file),$(basename $temp_file_main),$(basename $temp_file_myth),$(basename $temp_file_media),$(basename $temp_file_security),$(basename $temp_file_telecom)" -c ${build_dir}/${flavor}.conf

	echo "DROP DATABASE $sql_slave_db;
		DROP DATABASE $sql_slave_db_mainsqlcvs;
		DROP DATABASE $sql_slave_db_media;
		DROP DATABASE $sql_slave_db_security;
		DROP DATABASE $sql_slave_db_telecom;
		DROP DATABASE $sql_slave_db_mythsqlcvs;
		DROP DATABASE pluto_main;
	" | mysql -f -h $sql_slave_host -u $sql_slave_user
	echo "CREATE DATABASE $sql_slave_db;
		CREATE DATABASE $sql_slave_db_mainsqlcvs;
		CREATE DATABASE $sql_slave_db_mythsqlcvs;
		CREATE DATABASE $sql_slave_db_media;
		CREATE DATABASE $sql_slave_db_security;
		CREATE DATABASE $sql_slave_db_telecom;
		CREATE DATABASE pluto_main;
	" | mysql -f -h $sql_slave_host -u $sql_slave_user
	cat $temp_file | mysql -h $sql_slave_host -u $sql_slave_user $sql_slave_db
	cat $temp_file | mysql -h $sql_slave_host -u $sql_slave_user "pluto_main"
	cat $temp_file_main | mysql -h $sql_slave_host -u $sql_slave_user $sql_slave_db_mainsqlcvs
	cat $temp_file_myth | mysql -h $sql_slave_host -u $sql_slave_user $sql_slave_db_mythsqlcvs
	cat $temp_file_media | mysql -h $sql_slave_host -u $sql_slave_user $sql_slave_db_media
	cat $temp_file_security | mysql -h $sql_slave_host -u $sql_slave_user $sql_slave_db_security
	cat $temp_file_telecom | mysql -h $sql_slave_host -u $sql_slave_user $sql_slave_db_telecom

	mysql -h $sql_slave_host -u $sql_slave_user $sql_slave_db_mainsqlcvs < /root/Ubuntu_Helpers/Version.dump
	mysql -h $sql_slave_host -u $sql_slave_user $sql_slave_db < /root/Ubuntu_Helpers/Version.dump
	echo 'DELETE FROM `Package_Version`; DELETE FROM `Schema`;' | mysql -h $sql_slave_host -u $sql_slave_user $sql_slave_db_mainsqlcvs
	echo 'update Package_Directory SET FK_Distro = NULL WHERE PK_Package_Directory = 675' | mysql -h $sql_slave_host -u $sql_slave_user $sql_slave_db

	rm -rf $temp_file $temp_file_main $temp_file_myth $temp_file_media $temp_file_security $temp_file_telecom $temp_sqlcvsdir

	export Version=$("select VersionName from Version" | mysql $sql_slave_db | tail -1);
}

function Import_Pluto_Skins {
	local skins_dir=/home/samba/www_docs/graphics

	mkdir -p /usr/pluto/orbiter/
	rm -f /usr/pluto/orbiter/skins
	ln -s $skins_dir /usr/pluto/orbiter/skins

	rm -rf $skins_dir
	mkdir -p $skins_dir

	pushd /
	ssh pluto@10.0.0.150 tar -c $skins_dir | tar -x
	popd
	
	pushd /usr/pluto/orbiter/skins
		rm -f Basic
		ln -s LinuxMCE Basic
	popd
}

function Create_Diskless_Archive {
	touch /home/DisklessFS/ramdisk.tar.bz2

	apt-get -y install debootstrap

	local temp_dir=$(mktemp -d)
	debootstrap edgy $temp_dir http://ro.archive.ubuntu.com/ubuntu/

	mkdir -p /home/DisklessFS
	pushd $temp_dir
		tar -jcf /home/DisklessFS/PlutoMD_Debootstraped.tar.bz2 *
	popd
	rm -rf $temp_dir
}

function Create_ISO {
	# Create the iso
	pushd $local_mirror_dir
	        /root/Ubuntu_Helpers/get-packages.sh
	popd
}

function Upload_Build_Archive {
	pushd $local_mirror_dir
		tar -zcf /var/plutobuild/linuxmce-uploads.tar.gz *
		scp /var/plutobuild/linuxmce-uploads.tar.gz uploads@deb.plutohome.com:
	popd
}

