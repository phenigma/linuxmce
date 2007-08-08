#!/bin/bash

set -x

if [[ "$(id -u)" == 0 ]]; then
	echo "ERROR: This script can't be run as root."
	exit 1
fi

nobuild=""

branch="trunk"
#branch="2.0.0.43"

flavor=pluto
upload="y"

rev_pub=HEAD
rev_prv=HEAD

echo "Marker: starting `date`"
# if we receive a "force-build" parameter, ignore this setting
for ((i = 1; i <= "$#"; i++)); do
	case "${!i}" in
		force-build)
			nobuild=
		;;
		monster-build)
			flavor=monster
		;;
		via-build)
			flavor=via
		;;
		debug-build)
			flavor=pluto_debug
		;;
		nocheckout) 
			nocheckout=y 
		;;
		nosqlcvs) 
			nosqlcvs=y 
		;;
		nosqlcvs-sync)
			nosqlcvs_sync=y
		;;
		dont-compile-existing)
			dont_compile_existing="-X" 
		;;
		no-build)
			nobuild="-b" 
		;;
		no-upload)
			upload=
		;;
		rev=*,*)
			rev_pub=${!i%,*}
			rev_prv=${!i#,*}
		;;
		branch=*)
			branch=${!i#branch=}
		;;
		*)
			echo "ERROR: Unknown parameter '${!i}'"
			exit 1;
		;;
	esac
done

exec 1> >(tee /tmp/MakeScript-$flavor.log) 2>&1
echo "Building: branch=$branch, rev=$rev_pub,$rev_prv, flavor=$flavor,nobuild=$nobuild,nocheckout=$nocheckout,nosqlcvs=$nosqlcvs,nosqlcvs_sync=$nosqlcvs_sync,dont_compile_existing=$dont_compile_existing,upload=$upload"

if [[ "$flavor" == pluto_debug ]]; then
	. /home/WorkNew/src/BootScripts/LockUtils.sh
	WaitLock "DebugBuild" "DailyBuilder"
	trap 'Unlock "DebugBuild" "DailyBuilder"' EXIT
	export PATH=/usr/lib/ccache:"$PATH"
fi

## Read and export the configuration options
. /home/WorkNew/src/MakeRelease/MR_Conf.sh
export MakeRelease_Flavor="$flavor"
MR_ConfEval "$flavor"

fastrun=""
#fastrun="-f -DERROR_LOGGING_ONLY"

BASE_OUT_FOLDER=/home/builds/
CDHB_OUT_FOLDER="$BASE_OUT_FOLDER"/CDHB

BASE_INSTALLATION_CD_FOLDER=/home/installation-cd/
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

Q="select PK_Version from Version ORDER BY date desc, PK_Version limit 1"
version=$(echo "$Q;" | mysql -N pluto_main)
Q="update DeviceTemplate set FK_Package=307 where CommandLine='Generic_Serial_Device'" #  Be sure all GSD Devices use that package
echo "$Q;" | mysql -N pluto_main

echo Using version with id: "$version"

	if [[ -z "$nosqlcvs" ]]; then
		rm -f /tmp/main_sqlcvs_"$flavor".dump
		rm -f /tmp/myth_sqlcvs_"$flavor".dump
		#This is a release build, so we want to get a real sqlCVS
		if [[ -z "$nosqlcvs_sync" ]]; then
			bash -x /home/database-dumps/sync-sqlcvs.sh
		fi
		rm -f /tmp/sqlcvs_dumps_"$flavor".tar.gz
		ssh -i ~/.ssh/uploads_plutohome_build_150_key uploads@plutohome.com "
			rm -f /tmp/main_sqlcvs_\"$flavor\".dump /tmp/myth_sqlcvs_\"$flavor\" /home/uploads/sqlcvs_dumps_\"$flavor\".tar.gz;
			mysqldump -e --quote-names --allow-keywords --add-drop-table -u root -pmoscow70bogata main_sqlcvs > /tmp/main_sqlcvs_\"$flavor\".dump;
			mysqldump -e --quote-names --allow-keywords --add-drop-table -u root -pmoscow70bogata myth_sqlcvs > /tmp/myth_sqlcvs_\"$flavor\".dump;
			cd /tmp;
			tar zcvf /home/uploads/sqlcvs_dumps_\"$flavor\".tar.gz main_sqlcvs_\"$flavor\".dump myth_sqlcvs_\"$flavor\".dump"
		scp -i ~/.ssh/uploads_plutohome_build_150_key uploads@plutohome.com:/home/uploads/sqlcvs_dumps_"$flavor".tar.gz /tmp/
		cd /tmp
		tar zxvf sqlcvs_dumps_"$flavor".tar.gz

		if [ ! -f /tmp/main_sqlcvs_"$flavor".dump ]; then
			echo "main_sqlcvs.dump not found.  aborting"
			read
			exit
		fi

		if [ ! -f /tmp/myth_sqlcvs_"$flavor".dump ]; then
			echo "myth_sqlcvs.dump not found.  aborting"
			read
			exit
		fi

		MakeRelease_PrepFiles -p /tmp -e main_sqlcvs_"$flavor".dump,myth_sqlcvs_"$flavor".dump -c /etc/MakeRelease/$flavor.conf
		mysql main_sqlcvs_"$flavor" < /tmp/main_sqlcvs_"$flavor".dump
		mysql myth_sqlcvs_"$flavor" < /tmp/myth_sqlcvs_"$flavor".dump
		
		if [ $version -eq 1 ]; then
			sqlCVS -h localhost -D main_sqlcvs_"$flavor" update-psc
		fi
		sqlCVS -h localhost -D pluto_security update-psc
		sqlCVS -h localhost -D pluto_media update-psc
	fi

if [[ "$nobuild" == "" ]]; then
	if [[ -z "$nocheckout" ]]; then
		echo "Marker: svn co (r $rev_pub,$rev_prv) $flavor `date`"
		# Prepare build directory
		rm -rf $build_dir
		mkdir -p $build_dir/private
		
		# Check out private repository
		cd $build_dir/private
		if [[ "$branch" == trunk ]]; then
			svn co -r$rev_prv --username automagic --password "$(</etc/pluto/automagic.pwd)" http://10.0.0.170/pluto-private/trunk/. | tee $build_dir/svn.log
		else
			svn co -r$rev_prv --username automagic --password "$(</etc/pluto/automagic.pwd)" http://10.0.0.170/pluto-private/branches/"$branch" | tee $build_dir/svn.log
			rm -f trunk
			ln -s "$branch" trunk # workaround as to not change all of the script
		fi

		# Check out public repository
		cd $build_dir
		if [[ "$branch" == trunk ]]; then
			svn co -r$rev_pub http://10.0.0.170/pluto/trunk/. | tee -a $build_dir/svn.log
		else
			svn co -r$rev_pub http://10.0.0.170/pluto/branches/"$branch" | tee -a $build_dir/svn.log
			rm -f trunk
			ln -s "$branch" trunk # workaround as to not change all of the script
		fi
	
		echo "Marker: svn co done `date`"
	
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

		cp $build_dir/trunk/src/Fiire_Scripts/UpdateCodec.sh $build_dir/trunk/src/Xine_Player
	fi
		
	mkdir -p $build_dir/trunk/src/bin
	cd $build_dir/trunk/src/bin
	if [[ "$branch" == trunk ]]; then
		rm -f ../pluto_main/*
		# We have to use pluto_main so the class is named correctly, but that means we need to be sure  the local pluto_main is up to date
		sql2cpp -D pluto_main -h localhost
		cd ../pluto_main
	fi
    cd $build_dir/trunk
    svn info > svn.info
else
    cd $build_dir/trunk
fi

echo "Marker: Prepping files"
MakeRelease_PrepFiles -p $build_dir/trunk -e "*.cpp,*.h,Makefile*,*.php,*.sh,*.pl,*.awk" -c /etc/MakeRelease/$flavor.conf

#Do some database maintenance to correct any errors
# Be sure all debian packages are marked as being compatible with debian distro
MQ1="UPDATE Package_Source_Compat   JOIN Package_Source on FK_Package_Source=PK_Package_Source  SET FK_OperatingSystem=NULL,FK_Distro=1  WHERE FK_RepositorySource=2";
echo $MQ1 | mysql pluto_main

MQ1="DELETE FROM CachedScreens; DELETE FROM Device_DeviceData;"
echo $MQ1 | mysql main_sqlcvs_"$flavor"

svninfo=$(svn info . |grep ^Revision | cut -d" " -f2)
O2="UPDATE Version SET SvnRevision=$svninfo WHERE PK_Version=$version;"
echo $O2 | mysql pluto_main
echo $O2 > $build_dir/query2

Q3="select VersionName from Version WHERE PK_Version=$version"
version_name=`echo $(echo "$Q3;" | mysql -N pluto_main)_$flavor`

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

	trap - EXIT
	Unlock "DebugBuild" "DailyBuilder"
fi;

# Creating target folder.
mkdir -p "$BASE_OUT_FOLDER/$version_name";

if [[ "$MakeRelease_Flavor" == pluto_debug ]]; then
	. /home/WorkNew/src/BootScripts/SQL_Ops.sh
	UseDB pluto_builder
	Q="
		DELETE
		FROM File
	"
	RunSQL "$Q"
	rm -rf "$CDHB_OUT_FOLDER"
	
	MakeRelease_Gparm=-g
fi

echo "Marker: starting compilation `date`"
if ! MakeRelease $MakeRelease_Gparm $fastrun $nobuild $dont_compile_existing -O "$BASE_OUT_FOLDER/$version_name" -D main_sqlcvs_"$flavor" -c -o 1 -r 2,9,11 -m 1 -s $build_dir/trunk -n / -R $svninfo -v $version > >(tee $build_dir/MakeRelease1.log); then
	echo "MakeRelease Failed.  Press any key"
	reportError
	read
	exit
fi

# We did a 'don't make package' above with -c so the windows builder may continue building/outputting the latest bins
cp /home/builds/Windows_Output/src/bin/* $build_dir/trunk/src/bin

echo "Marker: starting package building `date`"
if ! MakeRelease $MakeRelease_Gparm $fastrun -D main_sqlcvs_"$flavor" -O "$BASE_OUT_FOLDER/$version_name" -b -o 1 -r 2,9,11 -m 1 -s $build_dir/trunk -n / -R $svninfo -v $version > >(tee $build_dir/MakeRelease1.log); then
	echo "MakeRelease Failed.  Press any key"
	reportError
	read
	exit
fi

# Compute MD5s
echo "Marker: calculating MD5 sums `date`"
if [[ "$MakeRelease_Flavor" == pluto_debug ]]; then
	/home/WorkNew/src/MakeRelease/MR_UpdateMD5s.sh
	mkdir -p "$CDHB_OUT_FOLDER"/tmp/
	cp "$BASE_OUT_FOLDER/$version_name"/tmp/*.deb "$CDHB_OUT_FOLDER"/tmp/
fi

#TODO: What the bleep is BUILD.sh ? (razvan)
BuildScript="$build_dir/trunk/src/BUILD.sh"
(echo '#!/bin/bash'; sed 's#cd $build_dir/trunk//src/#popd 2>/dev/null\npushd #g' Compile.script) >"$BuildScript"

`dirname $0`/scripts/propagate.sh "$BASE_OUT_FOLDER/$version_name/"

echo Setting this version as the current one.
rm -f $BASE_OUT_FOLDER/current-"$flavor"
ln -s $BASE_OUT_FOLDER/$version_name $BASE_OUT_FOLDER/current-"$flavor"

if [[ -z "$nobuild" ]]; then
	/home/WorkNew/src/MakeRelease/SelfPackagingModules.sh
	pushd /home/samba/repositories/$flavor/$replacementsdeb/main/binary-i386/
	./update-repository
	popd
fi

ln -s "/home/samba/repositories/$flavor/$replacementsdeb" /home/mirrors/Debian.ro.Sarge/debian/dists
ln -s "/home/samba/repositories/$flavor/$maindeb" /home/mirrors/Debian.ro.Sarge/debian/dists

pushd "$BASE_INSTALLATION_2_6_12_CD_FOLDER"
"$BASE_INSTALLATION_2_6_12_CD_FOLDER/build-cd1.sh" --iso-dir "$BASE_OUT_FOLDER/$version_name" --cache
"$BASE_INSTALLATION_2_6_12_CD_FOLDER/build-cd2.sh" --iso-dir "$BASE_OUT_FOLDER/$version_name"
popd

if ! MakeRelease -D main_sqlcvs_"$flavor" -O "$BASE_OUT_FOLDER/$version_name" -a -o 7 -n / -s /home/samba/builds/Windows_Output/ -r 10 -O "$BASE_OUT_FOLDER/$version_name" -v $version -b -k 116,119,124,126,154,159,193,203,213,226,237,242,255,277,204,118,303,128,162,191,195,280,272,363,364,341 > $build_dir/MakeRelease2.log ; then
	echo "MakeRelease Failed.  Press any key"
	reportError
	read
	exit
fi

if ! MakeRelease -D main_sqlcvs_"$flavor" -a -O "$BASE_OUT_FOLDER/$version_name" -o 7 -n / -s $build_dir/trunk -r 10 -v $version -b -k 211,214,233,256,219,220 > $build_dir/MakeRelease3.log ; then
	echo "MakeRelease Failed.  Press any key"
	reportError
	read
	exit
fi

if ! MakeRelease -D main_sqlcvs_"$flavor" -a -O "$BASE_OUT_FOLDER/$version_name" -o 12 -n / -s /home/samba/builds/Windows_Output/ -r 15 -v $version -b -k 119 > $build_dir/MakeRelease4.log ; then
	echo "MakeRelease Failed.  Press any key"
	reportError
	read
	exit
fi

if ! MakeRelease -D main_sqlcvs_"$flavor" -a -O "$BASE_OUT_FOLDER/$version_name" -o 8 -n / -s /home/samba/builds/Windows_Output/ -r 16 -v $version -b -k 119 > $build_dir/MakeRelease5.log ; then
	echo "MakeRelease Failed.  Press any key"
	reportError
	read
	exit
fi

cp -r /home/samba/builds/Windows_Output/winlib $BASE_OUT_FOLDER/$version_name
cp -r /home/samba/builds/Windows_Output/winnetdlls $BASE_OUT_FOLDER/$version_name

mkdir -p /home/builds/upload
pushd /home/builds

cd $version_name
#md5sum installation-cd.iso > installation-cd-1-$version_name.$flavor.md5
#mv installation-cd.iso installation-cd-1-$version_name.$flavor.iso
echo $version_name > current_version


if [[ "$version" !=  "1" || "$upload" == "y" ]]; then
	echo "Marker: uploading download.tar.gz `date`"
	
	## Create tarball containing /home/builds/build directory and upload it
	rm -f ../upload/download.$flavor.tar.gz; ssh -i ~/.ssh/uploads_linuxmce_build_150_key uploads@deb.plutohome.com "rm -f download.$flavor.tar.gz; rm -f replacements.$flavor.tar.gz"
	tar zcvf ../upload/download.$flavor.tar.gz *
	scp -i ~/.ssh/uploads_linuxmce_build_150_key ../upload/download.$flavor.tar.gz uploads@deb.plutohome.com:~/

	## Create replacements repo tarball and upload it
	rm -f ../upload/replacements.$flavor.tar.gz
	pushd /home/samba/repositories/$flavor/$replacementsdeb
	tar -hzcvf /home/builds/upload/replacements.$flavor.tar.gz  *
	popd
	scp -i ~/.ssh/uploads_linuxmce_build_150_key ../upload/replacements.$flavor.tar.gz uploads@deb.plutohome.com:~/

	if [ "$flavor" != "pluto" ]; then
		## Extract the files on plutohome.com
		echo "Marker: setting up `date`"
		ssh -i ~/.ssh/uploads_linuxmce_build_150_key uploads@deb.plutohome.com "/home/uploads/SetupUploads.sh \"$flavor\" \"$replacementsdeb\"  \"$maindeb\""
	else
#		ssh -i ~/.ssh/uploads_plutohome_build_150_key uploads@plutohome.com "rm -f ~/*download* ~/*replace*"
#		tar zcvf ../upload/download.tar.gz *
#		scp -i ~/.ssh/uploads_plutohome_build_150_key ../upload/download.tar.gz uploads@plutohome.com:~/
#		cd ../upload
#	    sh -x `dirname $0`/scripts/DumpVersionPackage.sh
#		scp -i ~/.ssh/uploads_plutohome_build_150_key dumpvp.tar.gz uploads@plutohome.com:~/
#
#		echo "Marker: uploading replacements `date`"
#
#		cd /home/WorkNew/src/MakeRelease
#		bash -x DirPatch.sh
#		scp -i ~/.ssh/uploads_plutohome_build_150_key replacements.tar.gz uploads@plutohome.com:~/
#		scp -i ~/.ssh/uploads_plutohome_build_150_key replacements.patch.sh uploads@plutohome.com:~/
		:
	fi

	popd	
fi

cp $build_dir/MakeRelease*.log "$BASE_OUT_FOLDER"/"$version_name"

trap - EXIT
Unlock "DebugBuild" "DailyBuilder"

echo "Marker: done `date`"
echo "Everything okay.  Press any key"

if [ "x$nobuild" = "x" ]; then
	(echo -e "MakeRelease $version ok\n\n") | mail -s "MakeRelease $version ok" razvan.g@plutohome.com -c aaron@plutohome.com
else
	(echo -e "MakeRelease $version ok\n\nNeed to reset nobuild flag") | mail -s "**reset nobuild flag** MakeRelease $version ok" aaron@plutohome.com
fi

if [ $version -ne 1 ]; then
	(echo -e "Change version back to 1\n\n") | mail -s "**change version back to 1**" aaron@plutohome.com
fi

if [ "x$fastrun" = "x" ]; then
	echo "Normal debug build"
else
	(echo -e "Remove fastrun flag\n\n") | mail -s "**remove fastrun flag**" aaron@plutohome.com
fi
#sh -x /home/SendToSwiss.sh
read
