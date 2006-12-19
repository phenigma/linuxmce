#!/bin/bash -x

flavor="pluto_debug"
build_dir="/var/plutobuild"
svn_dir="${build_dir}/svn"
svn_url="http://10.0.0.170/"

sql_master_host="10.0.0.150"
sql_master_db="pluto_main"
sql_master_db_mainsqlcvs="main_sqlcvs_$flavor"
sql_master_db_mythsqlcvs="myth_sqlcvs_$flavor"
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

out_dir="${build_dir}/out"
mkr_dir="${build_dir}/MakeRelease"

function Install_Build_Needed_Packages {
	local pkgs="subversion build-essential dh-make libmysqlclient12-dev libhttpfetcher-dev libattr1-dev libdbus-1-dev libdbus-glib-1-dev libhal-dev libdancer-xml0-dev libbluetooth2-dev libid3-3.8.3-dev libxine-dev x11proto-core-dev libx11-dev libx11-dev x11proto-core-dev x11proto-xext-dev x11proto-xf86vidmode-dev libx11-dev libjpeg62-dev libcdparanoia0-dev libsdl1.2-dev libsdl-gfx1.2-dev libxmu-headers x11proto-record-dev libhid-dev libusb-dev libsdl-image1.2-dev libsdl-ttf1.2-dev libsdl-sge-dev libxtst-dev libxrender-dev liblinphone1-dev libcddb-dev libdvdread-dev libcurl3-dev ruby1.8-dev swig libtcltk-ruby mysql-client"
	local pkg
	for pkg in $pkgs ;do
		apt-get -y install $pkg
	done
}


function Checkout_Pluto_Svn {
	[[ -d $svn_dir ]] && mkdir -p $svn_dir
	rm -rf ${svn_dir}/trunk
	mkdir -p ${svn_dir}/trunk
	svn co ${svn_url}/pluto/trunk  ${svn_dir}/trunk

	pushd ${svn_dir}/trunk/src
	svn co ${svn_url}/pluto-private/trunk/src/ZWave/
	svn co ${svn_url}/pluto-private/trunk/src/RFID_Interface/
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
	apt-get -y install quilt nasm libxv-dev libarts1-dev debhelper fakeroot

	pushd ${svn_dir}/trunk/ubuntu/libsdl1.2-1.2.10
		dpkg-buildpackage -us -uc -rfakeroot -b
		dpkg -i ../libsdl1.2debian-all_1.2.10-3ubuntu2pluto1_i386.deb
		dpkg -i ../libsdl1.2debian_1.2.10-3ubuntu2pluto1_i386.deb
		dpkg -i ../libsdl1.2-dev_1.2.10-3ubuntu2pluto1_i386.deb
	popd

	apt-get -y install libcaca-dev liblircclient-dev libtheora-dev libflac-dev libmodplug-dev libgnomevfs2-dev libsmbclient-dev libspeex-dev libmad0-dev libxvmc-dev automake1.9 autoconf libtool libcdio-dev sgmltools-lite dpatch
	pushd ${svn_dir}/trunk/ubuntu/xine-lib-1.1.2+repacked1
		dpkg-buildpackage -rfakeroot -us -uc -b
		dpkg -i ../libxine1_1.1.2+repacked1-0ubuntu3pluto1_i386.deb
		dpkg -i ../libxine-dev_1.1.2+repacked1-0ubuntu3pluto1_i386.deb
	popd
}

function Build_Pluto_Stuff {
	export PATH=/usr/lib/ccache:$PATH:${svn_dir}/trunk/src/bin
	export LD_LIBRARY_PATH="$mkr_dir:${svn_dir}/trunk/src/lib"

	MakeRelease="${mkr_dir}/MakeRelease"
	MakeRelease_PrepFiles="${mkr_dir}/MakeRelease_PrepFiles"
	#$MakeRelease_PrepFiles -p ${svn_dir}/trunk -e "*.cpp,*.h,Makefile*,*.php,*.sh,*.pl,*.awk" -c ${build_dir}/${flavor}.conf

	rm -rf $out_dir
	mkdir -p $out_dir
#	449,498 - SimplePhone
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
	$MakeRelease -b -h $sql_slave_host -u $sql_slave_user -O $out_dir -D $sql_slave_db -a -o 1 -r 21 -m 1 -K "543,542,462,607,432,431,427,426,430,429,499,498,336,337,589,590,515,516"  -s "${svn_dir}/trunk" -n / > >(tee -a $build_dir/Build.log)  -d
#-b
	#$MakeRelease -h 10.0.0.150 -u root -O $out_dir -D main_sqlcvs_$flavor -c -a -o 1 -r 20 -m 1 -s "${svn_dir}/trunk" -n / > >(tee -a $build_dir/Build.log) -d 
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
}

function Create_Local_Repository {
	local_mirror_dir="/usr/pluto/deb-cache"
	rm -rf $local_mirror_dir
	mkdir -p $local_mirror_dir

	cp ${out_dir}/tmp/*.deb $local_mirror_dir
	pushd $local_mirror_dir
		#dpkg-scanpackages . /dev/null | sed 's,\./,dists/replacements/main/binary-i386/,g' | gzip -9c > Packages.gz
		dpkg-scanpackages . /dev/null > Packages
		cat Packages | gzip -9c > Packages.gz
	popd
}

function Import_Build_Database {
	local temp_file=$(mktemp)
	local temp_file_main=$(mktemp)
	local temp_file_myth=$(mktemp)
	local temp_file_media=$(mktemp)
	local temp_file_security=$(mktemp)
	local temp_file_telecom=$(mktemp)

	mysqldump -h $sql_master_host -u $sql_master_user $sql_master_db > $temp_file
	mysqldump -h $sql_master_host -u $sql_master_user $sql_master_db_mainsqlcvs > $temp_file_main
	mysqldump -h $sql_master_host -u $sql_master_user $sql_master_db_mythsqlcvs > $temp_file_myth
	mysqldump -h $sql_master_host -u $sql_master_user $sql_master_db_media > $temp_file_media
	mysqldump -h $sql_master_host -u $sql_master_user $sql_master_db_security > $temp_file_security
	mysqldump -h $sql_master_host -u $sql_master_user $sql_master_db_telecom > $temp_file_telecom

	export LD_LIBRARY_PATH="$mkr_dir:${svn_dir}/trunk/src/lib"
	MakeRelease_PrepFiles="${mkr_dir}/MakeRelease_PrepFiles"
	$MakeRelease_PrepFiles -p $(dirname $temp_file) -e "$(basename $temp_file),$(basename $temp_file_main),$(basename $temp_file_myth),$(basename $temp_file_media),$(basename $temp_file_security),$(basename $temp_file_telecom)" -c ${mkr_dir}/${flavor}.conf

	echo "DROP DATABASE $sql_slave_db;
		DROP DATABASE $sql_slave_db_mainsqlcvs;
		DROP DATABASE $sql_slave_db_mythsqlcvs;
		DROP DATABASE $sql_slave_db_media;
		DROP DATABASE $sql_slave_db_security;
		DROP DATABASE $sql_slave_db_telecom;
	" | mysql -f -h $sql_slave_host -u $sql_slave_user
	echo "CREATE DATABASE $sql_slave_db;
		CREATE DATABASE $sql_slave_db_mainsqlcvs;
		CREATE DATABASE $sql_slave_db_mythsqlcvs;
		CREATE DATABASE $sql_slave_db_media;
		CREATE DATABASE $sql_slave_db_security;
		CREATE DATABASE $sql_slave_db_telecom;
	" | mysql -f -h $sql_slave_host -u $sql_slave_user
	cat $temp_file | mysql -h $sql_slave_host -u $sql_slave_user $sql_slave_db
	cat $temp_file_main | mysql -h $sql_slave_host -u $sql_slave_user $sql_slave_db_mainsqlcvs
	cat $temp_file_myth | mysql -h $sql_slave_host -u $sql_slave_user $sql_slave_db_mythsqlcvs
	cat $temp_file_media | mysql -h $sql_slave_host -u $sql_slave_user $sql_slave_db_media
	cat $temp_file_security | mysql -h $sql_slave_host -u $sql_slave_user $sql_slave_db_security
	cat $temp_file_telecom | mysql -h $sql_slave_host -u $sql_slave_user $sql_slave_db_telecom

	rm -rf $temp_file $temp_file_main $temp_file_myth $temp_file_media $temp_file_security $temp_file_telecom
}

function Import_Pluto_Skins {
	local skins_dir=/home/samba/www_docs/graphics
	
	mkdir -p /usr/pluto/orbiter/
	rm -f /usr/pluto/orbiter/skins
	ln -s $skins_dir /usr/pluto/orbiter/skins

	rm -rf $skins_dir
	mkdir -p $skins_dir

#pushd $skins_dir
	pushd /
	ssh root@10.0.0.150 tar -c $skins_dir | tar -x
	popd
}

#Import_Build_Database
#Import_Pluto_Skins
#Install_Build_Needed_Packages
#Build_Pluto_Replacements
#Checkout_Pluto_Svn
#Build_MakeRelease_Binary
#Create_Fake_Windows_Binaries
#Build_Pluto_Stuff
Create_Local_Repository


