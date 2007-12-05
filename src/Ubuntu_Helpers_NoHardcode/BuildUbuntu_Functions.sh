#!/bin/bash -x

. /usr/pluto/bin/Utils.sh
. /etc/lmce-build/builder.conf

export KDE_LMCE=""

## Log every message that the builder outputs in the logfile
exec 100>&1
#exec >/var/log/BuildUbuntu.log
#exec 2>&1

function DisplayMessage {
	echo "`date +%H:%M:%S`  $*" >&100
}

function Error {
	echo
	DisplayMessage "ERROR: $*"
	tail -20 /var/log/BuildUbuntu.log
	exit 1
}

function Checkout_Pluto_Svn {
	DisplayMessage "**** STEP : SVN CHECKOUT"
	local Branch="${1:-trunk}"
	local BranchParts="$2" # ex: "src web"; the rest will come from trunk

	local AllParts="src ubuntu web misc_utils installers config-pkgs"
	if [[ -z "$BranchParts" ]]; then
		BranchParts="$AllParts"
	fi

	DisplayMessage "Removing old svn checkout dir"
	[[ -d $svn_dir ]] && mkdir -p $svn_dir
	rm -rf ${svn_dir}/trunk
	
	for svn_module in ${BranchParts}; do
		mkdir -p ${svn_dir}/trunk/$svn_module
		DisplayMessage "Checking out {$svn_url}/pluto/$Branch/$svn_module"
		svn co  ${svn_url}/pluto/"$Branch"/$svn_module  ${svn_dir}/trunk/$svn_module || Error "Failed to checkout {$svn_url}/pluto/$Branch/$svn_module"
	done

	# get unmarked parts from trunk
	for svn_module in ${AllParts}; do
		if [[ " $BranchParts " == *" $svn_module "* ]]; then
			# this part was marked to be taken from the branch
			continue
		fi

		#get part from trunk
		mkdir -p ${svn_dir}/trunk/$svn_module
		DisplayMessage "Checking out {$svn_url}/pluto/trunk/$svn_module"
		svn co ${svn_url}/pluto/trunk/$svn_module  ${svn_dir}/trunk/$svn_module || Error "Failed to checkout {$svn_url}/pluto/trunk/$svn_module"
	done


	pushd ${svn_dir}/trunk/src
	DisplayMessage "Checking out {$svn_url}/pluto-private/$Branch/"
	svn co --username automagic --password "$(</etc/pluto/automagic.pwd)" ${svn_url}/pluto-private/"$Branch"/src/ZWave/ || Error "Failed to checkount {$svn_url}/pluto-private/$Branch/src/ZWave"
	svn co --username automagic --password "$(</etc/pluto/automagic.pwd)" ${svn_url}/pluto-private/"$Branch"/src/Fiire_Scripts/ || Error "Failed to checkount {$svn_url}/pluto-private/$Branch/src/Fiire_Scripts"
	svn co --username automagic --password "$(</etc/pluto/automagic.pwd)" ${svn_url}/pluto-private/"$Branch"/src/RFID_Interface/ || Error "Failed to checkount {$svn_url}/pluto-private/$Branch/src/RFID_Interface"
	svn co --username automagic --password "$(</etc/pluto/automagic.pwd)" ${svn_url}/pluto-private/"$Branch"/src/lmce_launch_manager/ || Error "Failed to checkount {$svn_url}/pluto-private/$Branch/src/lmce_launch_manager"
	popd
}


function Build_MakeRelease_Binary {
	DisplayMessage "**** STEP : PREPARING BUILD SYSTEM (MakeRelase)"
	DisplayMessage "Precompiling pluto_main"
	pushd ${svn_dir}/trunk/src/pluto_main
	make || Error "Failed to precompile pluto_main to use for MakeRelease"
	popd

	DisplayMessage "Precompiling PlutoUtils"
	pushd ${svn_dir}/trunk/src/PlutoUtils
	make || Error "Failed to precompile PlutoUtils"
	popd

	DisplayMessage "Precompiling SerializeClass"
	pushd ${svn_dir}/trunk/src/SerializeClass
	SNR_CPPFLAGS="" make || Error "Failed to precompile SerializeClass"
	popd

	DisplayMessage "Precompiling LibDCE"
	pushd ${svn_dir}/trunk/src/DCE
	SNR_CPPFLAGS="" make || Error "Failed to precompile LibDCE"
	popd

	DisplayMessage "Precompiling MakeRelease"
	pushd ${svn_dir}/trunk/src/MakeRelease
	make -f Makefile.MakeRelease || Error "Failed to precompile MakeRelease"
	popd

	DisplayMessage "Precompiling MakeRelease_PrepFiles"
	pushd ${svn_dir}/trunk/src/MakeRelease_PrepFiles
	make || Error "Failed to precompile MakeRelease_PrepFiles"
	popd

	DisplayMessage "Precompiling mysql_wrapper"
	pushd ${svn_dir}/trunk/src/mysql_wrapper
	make || Error "Failed to precompile mysql_wrapper"
	popd

	mkdir -p ${mkr_dir}
	mkdir -p /usr/pluto/bin
	cp ${svn_dir}/trunk/src/bin/MakeRelease ${mkr_dir}
	cp ${svn_dir}/trunk/src/bin/MakeRelease_PrepFiles ${mkr_dir}
	cp ${svn_dir}/trunk/src/bin/mysql_wrapper /usr/pluto/bin/
	cp ${svn_dir}/trunk/src/lib/*.so ${mkr_dir}
}

function Build_Pluto_Stuff {
	MakeReleaseExtraParams=("$@")

	
	touch /home/README.Devel.Dependencies
	mkdir -p /usr/pluto/dummy-packages/
	touch /usr/pluto/dummy-packages/Readme.PlutoConsoleUtilities.dummy

	export PATH=$PATH:${svn_dir}/trunk/src/bin
#	export PATH=/usr/lib/ccache:$PATH:${svn_dir}/trunk/src/bin

	export LD_LIBRARY_PATH="$mkr_dir:${svn_dir}/trunk/src/lib"
	export PKG_CONFIG_PATH=/opt/linphone-1.3.5/lib/pkgconfig

	SVNrevision=$(svn info "${svn_dir}"/trunk/src |grep ^Revision | cut -d" " -f2)

	MakeRelease="${mkr_dir}/MakeRelease"
	MakeRelease_PrepFiles="${mkr_dir}/MakeRelease_PrepFiles"
	$MakeRelease_PrepFiles -p ${svn_dir}/trunk -e "*.cpp,*.h,Makefile*,*.php,*.sh,*.pl,*.awk" -c ${build_dir}/${flavor}.conf

	LinphoneVersion="$(dpkg -s 'liblinphone1-lmce' | grep '^Version: ' | cut -d' ' -f2)"
	if [[ "$LinphoneVersion" == "1."[57]"."* ]]; then
		# Linphone 1.5.1 in Ubuntu Feisty makes SimplePhone to need this
		export LINPHONE_CONST=const
	fi

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

	$MakeRelease "${MakeReleaseExtraParams[@]}" -R "$SVNrevision" -h $sql_slave_host -u $sql_slave_user -O $out_dir -D $sql_slave_db -o 15 -r 21 -m 1 -K "543,542,462,607,432,431,427,426,430,429,336,337,589,590,515,516"  -s "${svn_dir}/trunk" -n / > >(tee -a $build_dir/Build.log)  -d || exit 1
	
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
	scp pluto@10.0.2.4:'/home/builds/Windows_Output_LinuxMCE/src/bin/*' ./
	popd

	pushd ${svn_dir}/trunk/src/lib
	scp pluto@10.0.2.4:'/home/builds/Windows_Output_LinuxMCE/src/lib/*' ./
	popd
	
}

function Create_Local_Repository {
	
	rm -f $local_mirror_dir/*.deb
	rm -f $local_mirror_dir/Packages*

	mkdir -p $local_mirror_dir

	cp ${out_dir}/tmp/*.deb $local_mirror_dir
	cp ${replacements_dir}/*.deb $local_mirror_dir
	pushd $local_mirror_dir
		dpkg-scanpackages . /dev/null > Packages
		cat Packages | gzip -9c > Packages.gz
#		ln -sf . $local_mirror_dir/ubuntu
	popd

#       cp -f /root/build-files/virus_free.php /var/www
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
	ssh -i /root/.ssh/uploads_plutohome_key uploads@plutohome.com "
                set -x;
                rm -f /tmp/main_sqlcvs.dump /tmp/myth_sqlcvs /home/uploads/sqlcvs_dumps.tar.gz;
                mysqldump --quote-names --allow-keywords --add-drop-table -u root -pmoscow70bogata main_sqlcvs > /tmp/main_sqlcvs.dump;
                mysqldump --quote-names --allow-keywords --add-drop-table -u root -pmoscow70bogata myth_sqlcvs > /tmp/myth_sqlcvs.dump;
                cd /tmp;
                tar zcvf /home/uploads/sqlcvs_dumps.tar.gz main_sqlcvs.dump myth_sqlcvs.dump"
        scp -i /root/.ssh/uploads_plutohome_key uploads@plutohome.com:/home/uploads/sqlcvs_dumps.tar.gz $temp_sqlcvsdir
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

	echo 'DELETE FROM `Package_Version`; DELETE FROM `Schema`;' | mysql -h $sql_slave_host -u $sql_slave_user $sql_slave_db_mainsqlcvs

	rm -rf $temp_file $temp_file_main $temp_file_myth $temp_file_media $temp_file_security $temp_file_telecom $temp_sqlcvsdir
}

function Import_Pluto_Skins {

	local skins_dir=/home/samba/www_docs/graphics

	mkdir -p /usr/pluto/orbiter/
	rm -f /usr/pluto/orbiter/skins
	ln -s $skins_dir /usr/pluto/orbiter/skins

	rm -rf $skins_dir
	mkdir -p $skins_dir

	pushd /
	ssh pluto@10.0.2.4 tar -c $skins_dir | tar -x
	popd
	
	pushd /usr/pluto/orbiter/skins
#		rm -f Basic
		cp -r LinuxMCE/* Basic/
#		ln -s LinuxMCE Basic
	popd

	pushd ${build_dir}
		ln -s /home/samba
	popd

	mkdir -p "/home/samba/www_docs/sample media"
	pushd /
		ssh pluto@10.0.2.4 'tar -c "/home/samba/www_docs/sample media"' | tar -x 
	popd
	
}


function Create_ISO {
	# Create the iso
	pushd $local_mirror_dir
	        /root/Ubuntu_Helpers/cd1-build.sh "linuxmce-1.1"
		/root/Ubuntu_Helpers/cd2-build.sh "linuxmce-1.1"
	popd
}
