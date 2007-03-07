#!/bin/bash

set -x

nobuild=""

branch="trunk"
#branch="2.0.0.40"

flavor=pluto
upload="y"

rev_pub=HEAD
rev_prv=HEAD

automagic_pwd="$(</etc/pluto/automagic.pwd)"
mysql_ph_pluto_main_pwd="$(</etc/pluto/mysql_ph_pluto_main.pwd)"

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
echo "Building: branch=$branch, rev=$rev_pub,$rev_prv, flavor=$flavor"

Pkgs="116,135 117,136 118,137 119,138 122,141 124,143 126,145 133,152 134,153 154,155 162,161 191,190 193,192 195,194 197,196 200,199 203,202 204 213,212 211 214 219 220 226,227 229,339 206 233 234 237,201 238,198 240,239 242,241 247,248 255,254 256 266 270,271 272,281 277,278 280,279 283,284 290,289 294 302 303,304 316,315 318,319 456,455 307,335 337,336 341,342 346,347 363 364 391 393,392 394 397,396 401,398 404,403 407 420,419 340,421 425,424 427,426 430,429 432,431 436 447,446 448 453,452 454 462,607 465,464 467,466 469,468 471,470 472 473 488,487 490 493,492 406,405 491,330 497,496 499,498 502 503,508 504 506,505 509 516,515 520 522 523 526,525 528 530,529 533 535,536 539,538 540 543,542 545,544 549 553 554 556,555 562 564,563 274,28 567 568 569 573,572 576 578 579 580 582,581 584,583 590,589 599,598"

## Read and export the configuration options
. /home/WorkNew/src/MakeRelease/MR_Conf.sh
export MakeRelease_Flavor="$flavor"
MR_ConfEval "$flavor"

fastrun=""
#fastrun="-f -DERROR_LOGGING_ONLY"

BASE_OUT_FOLDER=/home/builds/

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
		ssh uploads@plutohome.com "
			rm -f /tmp/main_sqlcvs_\"$flavor\".dump /tmp/myth_sqlcvs_\"$flavor\" /home/uploads/sqlcvs_dumps_\"$flavor\".tar.gz;
			mysqldump -e --quote-names --allow-keywords --add-drop-table -u root -p\"$mysql_ph_pluto_main_pwd\" main_sqlcvs > /tmp/main_sqlcvs_\"$flavor\".dump;
			mysqldump -e --quote-names --allow-keywords --add-drop-table -u root -p\"$mysql_ph_pluto_main_pwd\" myth_sqlcvs > /tmp/myth_sqlcvs_\"$flavor\".dump;
			cd /tmp;
			tar zcvf /home/uploads/sqlcvs_dumps_\"$flavor\".tar.gz main_sqlcvs_\"$flavor\".dump myth_sqlcvs_\"$flavor\".dump"
		scp uploads@plutohome.com:/home/uploads/sqlcvs_dumps_"$flavor".tar.gz /tmp/
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
			svn co -r$rev_prv --username automagic --password "$automagic_pwd" http://10.0.0.170/pluto-private/trunk/. | tee $build_dir/svn.log
		else
			svn co -r$rev_prv --username automagic --password "$automagic_pwd" http://10.0.0.170/pluto-private/branches/"$branch" | tee $build_dir/svn.log
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
fi;

# Creating target folder.
mkdir -p "$BASE_OUT_FOLDER/$version_name";
: >$build_dir/MakeRelease1.log
: >$build_dir/Messages.log

echo "Marker: starting compilation `date`"
for Pkg in $Pkgs; do
	if ! MakeRelease -k $Pkg -h localhost $fastrun $nobuild $dont_compile_existing -O "$BASE_OUT_FOLDER/$version_name" -D main_sqlcvs_"$flavor" -c -a -o 1 -r 20 -m 1 -s $build_dir/trunk -n / -R $svninfo -v $version > >(tee -a $build_dir/MakeRelease1.log); then
		echo "Failed to compile: $Pkg" |tee -a $build_dir/Messages.log
	fi
done

# We did a 'don't make package' above with -c so the windows builder may continue building/outputting the latest bins
cp /home/builds/Windows_Output/src/bin/* $build_dir/trunk/src/bin

echo "Marker: starting package building `date`"
for Pkg in $Pkgs; do
	if ! MakeRelease -k $Pkg -h localhost $fastrun -D main_sqlcvs_"$flavor" -O "$BASE_OUT_FOLDER/$version_name" -b -a -o 1 -r 20 -m 1 -s $build_dir/trunk -n / -R $svninfo -v $version > >(tee -a $build_dir/MakeRelease1.log); then
		echo "Failed to build package: $Pkg" |tee -a $build_dir/Messages.log
	fi
done

cp $build_dir/MakeRelease*.log "$BASE_OUT_FOLDER"/"$version_name"

echo "Marker: done `date`"
echo "Everything okay.  Press any key"
read
