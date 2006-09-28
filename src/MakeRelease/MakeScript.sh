#!/bin/bash

nobuild="-b"

branch="trunk"
#branch="2.0.0.40"

flavor=pluto_release

echo "Marker: starting `date`"
# if we receive a "force-build" parameter, ignore this setting
for ((i = 1; i <= "$#"; i++)); do
	case "${!i}" in
		force-build)
			monster=
			nobuild=
			linuxmce=
		;;
		monster-build)
			monster=y
			#nobuild=
			flavor=monster
		;;
		linuxmce-build)
			linuxmce=y
			flavor=linuxmce
		;;
		via-build)
			monster=
			nobuild=
			flavor=via
		;;
		debug-build)
			monster=
			nobuild=
			flavor=pluto_debug
		;;
		nocheckout) nocheckout=y ;;
		nosqlcvs) nosqlcvs=y ;;
		dont-compile-existing) dont_compile_existing="-X" ;;
	esac
done

. /home/WorkNew/src/MakeRelease/MR_Conf.sh

export MakeRelease_Flavor="$flavor"
ConfEval "$flavor"

fastrun=""
#fastrun="-f -DERROR_LOGGING_ONLY"

BASE_OUT_FOLDER=/home/builds/

BASE_INSTALLATION_CD_FOLDER=/home/installation-cd/
BASE_INSTALLATION_2_6_10_CD_FOLDER=/home/installation-cd-kernel-2.6.10/
BASE_INSTALLATION_2_6_12_CD_FOLDER=/home/installation-cd-kernel-2.6.12/

#
# Uncomment the following function name to allow running of the MakeScript without actually calling 
# the MakeRelease program/
#function MakeRelease { true; }


echo "--" >> $BASE_OUT_FOLDER/MakeReleaseCalls.log
echo "--" >> $BASE_OUT_FOLDER/MakeReleaseCalls.log
echo "Called at time `date`" >> $BASE_OUT_FOLDER/MakeReleaseCalls.log
echo "--" >> $BASE_OUT_FOLDER/MakeReleaseCalls.log
ps auxfww >> $BASE_OUT_FOLDER/MakeReleaseCalls.log
echo "--" >> $BASE_OUT_FOLDER/MakeReleaseCalls.log

function changeSvnPermissions
{
	newPerms=$1;

	if [ "x$1" != "" ]; then
		return;
	fi;

	sudo -u www-date sed "s/@restricted = .*/@restricted = $newPerms/" /home/sources/svn-repositories/svn-users-permissions > /home/sources/svn-repositories/svn-users-permissions-updated
	sudo -u www-data cp /home/sources/svn-repositories/svn-users-permissions /home/sources/svn-repositories/svn-users-permissions-bak
	sudo -u www-data cp /home/sources/svn-repositories/svn-users-permissions-updated /home/sources/svn-repositories/svn-users-permissions
}

ReplacePluto()
{
	File="$1"
	ProperName="$2"
	WebSite="$3"

	# What the?!  sed only replaces one instance per line so do it multipe times??
	# Answer to that one: add the "g" modifier and it replaces all instances, like I did (Radu)
	sed -i "s/Pluto/$ProperName/g" $File
	sed -i "s/plutohome.com/$WebSite/g" $File
}

Q="select PK_Version from Version ORDER BY date desc, PK_Version limit 1"
version=$(echo "$Q;" | mysql -N pluto_main)
Q="update DeviceTemplate set FK_Package=307 where CommandLine='Generic_Serial_Device'" #  Be sure all GSD Devices use that package
echo "$Q;" | mysql -N pluto_main

echo Using version with id: "$version"

if [ "$nobuild" = "" ]; then
	if [[ -z "$nosqlcvs" ]]; then
		rm /tmp/main_sqlcvs.dump
		rm /tmp/myth_sqlcvs.dump
		#This is a release build, so we want to get a real sqlCVS
		bash -x /home/database-dumps/sync-sqlcvs.sh
		rm /tmp/sqlcvs_dumps.tar.gz
		ssh uploads@plutohome.com "
			rm -f /tmp/main_sqlcvs.dump /tmp/myth_sqlcvs /home/uploads/sqlcvs_dumps.tar.gz;
			mysqldump -e --quote-names --allow-keywords --add-drop-table -u root -pmoscow70bogata main_sqlcvs > /tmp/main_sqlcvs.dump;
			mysqldump -e --quote-names --allow-keywords --add-drop-table -u root -pmoscow70bogata myth_sqlcvs > /tmp/myth_sqlcvs.dump;
			cd /tmp;
			tar zcvf /home/uploads/sqlcvs_dumps.tar.gz main_sqlcvs.dump myth_sqlcvs.dump"
		scp uploads@plutohome.com:/home/uploads/sqlcvs_dumps.tar.gz /tmp/
		cd /tmp
		tar zxvf sqlcvs_dumps.tar.gz

		if [ ! -f /tmp/main_sqlcvs.dump ]; then
			echo "main_sqlcvs.dump not found.  aborting"
			read
			exit
		fi

		if [ ! -f /tmp/myth_sqlcvs.dump ]; then
			echo "myth_sqlcvs.dump not found.  aborting"
			read
			exit
		fi

		MakeRelease_PrepFiles -p /tmp -e main_sqlcvs.dump,myth_sqlcvs.dump -c /etc/MakeRelease/$flavor.conf
		mysql main_sqlcvs < /tmp/main_sqlcvs.dump
		mysql myth_sqlcvs < /tmp/myth_sqlcvs.dump
		
		if [ $version -eq 1 ]; then
			sqlCVS -h localhost -D main_sqlcvs update-psc
		fi
		sqlCVS -h localhost -D pluto_security update-psc
		sqlCVS -h localhost -D pluto_media update-psc
	fi

	if [[ -z "$nocheckout" ]]; then
		echo "Marker: svn co $flavor `date`"
		# Prepare build directory
		rm -rf $build_dir
		mkdir -p $build_dir/private
		
		# Check out private repository
		cd $build_dir/private
		if [[ "$branch" == trunk ]]; then
			svn co http://10.0.0.170/pluto-private/trunk/. | tee $build_dir/svn.log
		else
			svn co http://10.0.0.170/pluto-private/branches/"$branch" | tee $build_dir/svn.log
			rm -f trunk
			ln -s "$branch" trunk # workaround as to not change all of the script
		fi

		# Check out public repository
		cd $build_dir
		if [[ "$branch" == trunk ]]; then
			svn co http://10.0.0.170/pluto/trunk/. | tee -a $build_dir/svn.log
		else
			svn co http://10.0.0.170/pluto/branches/"$branch" | tee -a $build_dir/svn.log
			rm -f trunk
			ln -s "$branch" trunk # workaround as to not change all of the script
		fi
		
		echo "Marker: Prepping files"
		MakeRelease_PrepFiles -p $build_dir/trunk -e "*.cpp,*.h,Makefile*,*.php,*.sh,*.pl" -c /etc/MakeRelease/$flavor.conf

		# Clone Video4Linux Mercurial repository
		cd $build_dir/trunk/src/drivers
		hg clone /home/sources/mercurial-repositories/v4l-dvb/ | tee $build_dir/mercurial-v4l.log
		
		# Make symlinks from private copy to public copy
		for Dir1 in $build_dir/private/trunk/*; do
			BaseDir1=$(basename "$Dir1")
			[[ -L "$Dir1" || ! -d $build_dir/trunk/"$BaseDir1" ]] && continue
			for Dir2 in "$Dir1"/*; do
				[[ -L "$Dir2" ]] && continue
				BaseDir2=$(basename "$Dir2")
				rm -f $build_dir/trunk/"$BaseDir1"/"$BaseDir2"
				ln -s $build_dir{/private,}/trunk/"$BaseDir1"/"$BaseDir2"
				echo ln -s $build_dir{/private,}/trunk/"$BaseDir1"/"$BaseDir2"
			done
		done
		
		# Make symlinks from public copy to private copy
		for Dir1 in $build_dir/trunk/*; do
			BaseDir1=$(basename "$Dir1")
			[[ -L "$Dir1" || ! -d $build_dir/private/trunk/"$BaseDir1" ]] && continue
			for Dir2 in "$Dir1"/*; do
				[[ -L "$Dir2" ]] && continue
				BaseDir2=$(basename "$Dir2")
				rm -f $build_dir/private/trunk/"$BaseDir1"/"$BaseDir2"
				ln -s $build_dir{,/private}/trunk/"$BaseDir1"/"$BaseDir2"
				echo ln -s $build_dir{,/private}/trunk/"$BaseDir1"/"$BaseDir2"
			done
		done
	fi
		
	mkdir -p $build_dir/trunk/src/bin
	cd $build_dir/trunk/src/bin
	rm ../pluto_main/*
	# We have to use pluto_main so the class is named correctly, but that means we need to be sure  the local pluto_main is up to date
	sql2cpp -D pluto_main -h localhost
	cd ../pluto_main

	## temporary
	svn revert Table_Device.cpp  Table_Device_DeviceData.cpp Table_Orbiter.cpp Table_CommandGroup_Command_CommandParameter.cpp Table_CommandGroup.cpp Table_CommandGroup_Command.cpp
	svn -m "Automatic Regen" --username aaron --password aaron --non-interactive commit
    cd $build_dir/trunk
    svn info > svn.info
else
    cd $build_dir/trunk
fi

#Do some database maintenance to correct any errors
# Be sure all debian packages are marked as being compatible with debian distro
MQ1="UPDATE Package_Source_Compat   JOIN Package_Source on FK_Package_Source=PK_Package_Source  SET FK_OperatingSystem=NULL,FK_Distro=1  WHERE FK_RepositorySource=2";
echo $MQ1 | mysql pluto_main

MQ1="DELETE FROM CachedScreens; DELETE FROM Device_DeviceData;"
echo $MQ1 | mysql main_sqlcvs

svninfo=$(svn info . |grep ^Revision | cut -d" " -f2)
O2="UPDATE Version SET SvnRevision=$svninfo WHERE PK_Version=$version;"
echo $O2 | mysql pluto_main
echo $O2 > $build_dir/query2

Q3="select VersionName from Version WHERE PK_Version=$version"
version_name=$(echo "$Q3;" | mysql -N pluto_main)

DEST="aaron@plutohome.com -c radu.c@plutohome.com -c chris.m@plutohome.com"

function reportError
{
    echo "MakeRelease failed.";

    [ -e "$build_dir/MakeRelease.log" ] && (echo -e "Make Release failed. Attached are the last 50 lines of the relevant log file\n\n"; tail -n 50 $build_dir/MakeRelease.log) | mail -s "MakeRelease failed" $DEST
#   [ -e "$3/svn-checkout.log" ] && (echo -e "Build failed $1. Attached are the last 50 lines of the relevant log file\n\n"; tail -n 50 $3/svn-checkout.log) | mail -s "Build failure for revision $2" $DEST

	cp $build_dir/MakeRelease*.log "$BASE_OUT_FOLDER"/"$version_name"
}

echo Building version $version_name 


if [ -d "$BASE_OUT_FOLDER/$version_name" ]; then
	[ -d "$BASE_OUT_FOLDER/$version_name.auto_backup" ] && rm -rf "$BASE_OUT_FOLDER/$version_name.auto_backup";
	echo It seems like the same build was created before. Backing up the previous version.
	echo Warning !! I will only keep 1 backup of the build. You should save the backup while
	echo this build is running if you needs it earlier. 
	echo     This is the backed up folder: $BASE_OUT_FOLDER/$version_name.auto_backup
	echo 
	mv "$BASE_OUT_FOLDER/$version_name" "$BASE_OUT_FOLDER/$version_name.auto_backup"
fi;

# Creating target folder.
mkdir -p "$BASE_OUT_FOLDER/$version_name";
echo "Marker: starting compilation `date`"
if ! MakeRelease $fastrun $nobuild $dont_compile_existing -D main_sqlcvs -c -a -o 1 -r 2,9,11 -m 1 -s $build_dir/trunk -n / -R $svninfo -v $version > >(tee $build_dir/MakeRelease1.log); then
	echo "MakeRelease Failed.  Press any key"
	reportError
	read
	exit
fi

# We did a 'don't make package' above with -c so the windows builder may continue building/outputting the latest bins
cp /home/builds/Windows_Output/src/bin/* $build_dir/trunk/src/bin

echo "Marker: starting package building `date`"
if ! MakeRelease $fastrun -D main_sqlcvs -b -a -o 1 -r 2,9,11 -m 1 -s $build_dir/trunk -n / -R $svninfo -v $version > >(tee $build_dir/MakeRelease1.log); then
	echo "MakeRelease Failed.  Press any key"
	reportError
	read
	exit
fi
	
BuildScript="$build_dir/trunk/src/BUILD.sh"
(echo '#!/bin/bash'; sed 's#cd $build_dir/trunk//src/#popd 2>/dev/null\npushd #g' Compile.script) >"$BuildScript"

if [[ "$monster" == y ]]; then
	`dirname $0`/scripts/propagate-monster.sh "$BASE_OUT_FOLDER/$version_name/"

elif [[ "$linuxmce" == y ]]; then
# build linuxmce 
#	`dirname $0`/scripts/propagate-linuxmce.sh "$BASE_OUT_FOLDER/$version_name/"
:
else
	`dirname $0`/scripts/propagate.sh "$BASE_OUT_FOLDER/$version_name/"
fi

echo Setting this version as the current one.
rm $BASE_OUT_FOLDER/current
ln -s $BASE_OUT_FOLDER/$version_name $BASE_OUT_FOLDER/current

#pushd "$BASE_INSTALLATION_CD_FOLDER"
#"$BASE_INSTALLATION_CD_FOLDER/go-netinst.pl" "$BASE_OUT_FOLDER/$version_name" cache
#popd
#pushd "$BASE_INSTALLATION_2_6_10_CD_FOLDER"
#"$BASE_INSTALLATION_2_6_10_CD_FOLDER/go-netinst.pl" "$BASE_OUT_FOLDER/$version_name" cache
#popd

if [ "$monster" = "y" ]; then
	pushd "$BASE_INSTALLATION_2_6_12_CD_FOLDER"
	"$BASE_INSTALLATION_2_6_12_CD_FOLDER/go-netinst-monster.pl" "$BASE_OUT_FOLDER/$version_name" cache
	popd
elif [ "$linuxmce" = "y" ]; then
	pushd "$BASE_INSTALLATION_2_6_12_CD_FOLDER"
	"$BASE_INSTALLATION_2_6_12_CD_FOLDER/go-netinst-linuxmce.pl" "$BASE_OUT_FOLDER/$version_name" cache
	popd
else 
	pushd "$BASE_INSTALLATION_2_6_12_CD_FOLDER"
	"$BASE_INSTALLATION_2_6_12_CD_FOLDER/go-netinst.pl" "$BASE_OUT_FOLDER/$version_name" cache
	popd
fi 
#mv /home/builds/$version_name/debian-packages.tmp /home/builds/$version_name/debian-packages.list

if ! MakeRelease -D main_sqlcvs -a -o 7 -n / -s /home/samba/builds/Windows_Output/ -r 10 -v $version -b -k 116,119,124,126,154,159,193,203,213,226,237,242,255,277,204,118,303,128,162,191,195,280,272,363,364,341 > $build_dir/MakeRelease2.log ; then
	echo "MakeRelease Failed.  Press any key"
	reportError
	read
	exit
fi

if ! MakeRelease -D main_sqlcvs -a -o 7 -n / -s $build_dir/trunk -r 10 -v $version -b -k 211,214,233,256,219,220 > $build_dir/MakeRelease3.log ; then
	echo "MakeRelease Failed.  Press any key"
	reportError
	read
	exit
fi

if ! MakeRelease -D main_sqlcvs -a -o 12 -n / -s /home/samba/builds/Windows_Output/ -r 15 -v $version -b -k 119 > $build_dir/MakeRelease4.log ; then
	echo "MakeRelease Failed.  Press any key"
	reportError
	read
	exit
fi

if ! MakeRelease -D main_sqlcvs -a -o 8 -n / -s /home/samba/builds/Windows_Output/ -r 16 -v $version -b -k 119 > $build_dir/MakeRelease5.log ; then
	echo "MakeRelease Failed.  Press any key"
	reportError
	read
	exit
fi

cp -r /home/samba/builds/Windows_Output/winlib $BASE_OUT_FOLDER/$version_name
cp -r /home/samba/builds/Windows_Output/winnetdlls $BASE_OUT_FOLDER/$version_name
#Moved up ..
#dcd /home/tmp/pluto-build/
#./propagate.sh

/home/WorkNew/MakeRelease/SelfPackagingModules.sh
pushd /home/samba/repositories/pluto/replacements/main/binary-i386/
./update-repository
popd

mkdir -p /home/builds/upload
pushd /home/builds
rm upload/download.tar.gz
cd $version_name
if [ "$monster" = "y" ]; then
	md5sum installation-cd.iso > installation-cd-1-$version_name.monster.md5
	mv installation-cd.iso installation-cd-1-$version_name.monster.iso
	echo $version_name > current_version

elif [ "$linuxmce" = "y" ]; then

	md5sum installation-cd.iso > installation-cd-1-$version_name.linuxmce.md5
	mv installation-cd.iso installation-cd-1-$version_name.linuxmce.iso
	echo $version_name > current_version
else
	md5sum installation-cd.iso > installation-cd-1-$version_name.pluto.md5
	mv installation-cd.iso installation-cd-1-$version_name.pluto.iso
	echo $version_name > current_version
fi
	
if [[ $version -ne 1 || $upload == y ]]; then
	echo "Marker: uploading download.tar.gz `date`"
	if [ "$monster" = "y" ]; then
		tar zcvf ../upload/download.monster.tar.gz *
		scp ../upload/download.monster.tar.gz uploads@plutohome.com:~/
	else
		ssh uploads@plutohome.com "rm ~/*download* ~/*replace*"
		tar zcvf ../upload/download.tar.gz *
		scp ../upload/download.tar.gz uploads@plutohome.com:~/
		cd ../upload
	    sh -x `dirname $0`/scripts/DumpVersionPackage.sh
		scp dumpvp.tar.gz uploads@plutohome.com:~/

		echo "Marker: uploading replacements `date`"

		cd /home/WorkNew/src/MakeRelease
		bash -x DirPatch.sh
		scp replacements.tar.gz uploads@plutohome.com:~/
		scp replacements.patch.sh uploads@plutohome.com:~/
	    popd
	fi

	if [ $version -eq 1 ]; then
		ssh uploads@plutohome.com "/home/uploads/SetupTemp.sh"
	fi
	
	echo "Marker: SourceForge `date`"
	
	# SourceForge CVS
	if ! MakeRelease -D main_sqlcvs -a -o 1 -r 12 -m 1 -s $build_dir/trunk -n / -b -v $version  > $build_dir/MakeRelease6.log ; then
		reportError
		echo "MakeRelease to source forge CVS Failed.  Press any key"
	    read
	fi
	
	# SourceForge Debian Sarge
	if ! MakeRelease -D main_sqlcvs -a -o 1 -r 13 -m 1 -s $build_dir/trunk -n / -b -v $version  > $build_dir/MakeRelease7.log ; then
		reportError
		echo "MakeRelease to source forge CVS Failed.  Press any key"
		read
	fi

	# SourceForge Windows Archives
	if ! MakeRelease -D main_sqlcvs -a -o 1 -r 17 -m 1 -s $build_dir/trunk -n / -b -v $version  > $build_dir/MakeRelease8.log ; then
		reportError
		echo "MakeRelease to source forge CVS Failed.  Press any key"
		read
	fi
		
	# SourceForge Source Archives
	if ! MakeRelease -D main_sqlcvs -a -o 1 -r 18 -m 1 -s $build_dir/trunk -n / -b -v $version  > $build_dir/MakeRelease9.log ; then
		reportError
		echo "MakeRelease to source forge CVS Failed.  Press any key"
		read
	fi

	(echo -e "Need to propagate new SourceForge\n\n") | mail -s "SourceForge" dan.h@plutohome.com -c aaron@plutohome.com
	
	echo "Sent to server."
fi

cp $build_dir/MakeRelease*.log "$BASE_OUT_FOLDER"/"$version_name"

echo "Marker: done `date`"
echo "Everything okay.  Press any key"

if [ "x$nobuild" = "x" ]; then
	(echo -e "MakeRelease $version ok\n\n") | mail -s "MakeRelease $version ok" mihai.t@plutohome.com -c aaron@plutohome.com
else
	(echo -e "MakeRelease $version ok\n\nNeed to reset nobuild flag") | mail -s "**reset nobuild flag** MakeRelease $version ok" mihai.t@plutohome.com -c aaron@plutohome.com
fi

if [ $version -ne 1 ]; then
	(echo -e "Change version back to 1\n\n") | mail -s "**change version back to 1**" mihai.t@plutohome.com -c aaron@plutohome.com
fi

if [ "x$fastrun" = "x" ]; then
	echo "Normal debug build"
else
	(echo -e "Remove fastrun flag\n\n") | mail -s "**remove fastrun flag**" mihai.t@plutohome.com -c aaron@plutohome.com
fi
sh -x /home/SendToSwiss.sh
read

#if [[ "$monster" == y ]]; then
#	mv /home/builds/$version_name{,.monster}
#	rm -f /home/builds/current /home/builds/current-monster
#	ln -s /home/builds/$version_name.monster /home/builds/current-monster
#	echo "Last build used the monster flag. Reset it." | mail -s "** Reset Monster Flag **" radu.c@plutohome.com
#else
#	mv /home/builds/$version_name{,.pluto}
#	rm -f /home/builds/current /home/builds/current-pluto
#	ln -s /home/builds/$version_name.pluto /home/builds/current
#	ln -s /home/builds/$version_name.pluto /home/builds/current-pluto
#fi
