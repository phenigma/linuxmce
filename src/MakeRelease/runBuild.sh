#!/bin/bash

. /etc/pluto/internal-mail.sh
. /home/WorkNew/src/MakeRelease/MR_Conf.sh

for ((i = 1; i <= $#; i++)); do
	case "${!i}" in
		--force) Force=y ;; # bypass disabled flag; useful from command line after touching the ~/disabled file to prevent cron from interfering
	esac
done

if [[ -f ~/disabled && "$Force" != y ]]; then
	exit
fi

#DEST="chris.m@plutohome.com -c radu.c@plutohome.com -c rob.r@plutohome.com -c kirill.b@plutohome.com -c eugen.c@plutohome.com -c remus.c@plutohome.com -c razvan.g@plutohome.com -c ciprian.m@plutohome.com -c aaron.b@plutohome.com"

REPO_DIR=/home/sources/svn-repositories/pluto
BASE_REPO_URL=http://10.0.0.170/pluto

STATE_DIR=~/build_state
BUILD_DIR=~/build
SAVED_LOGS=~/build_logs

# make sure the file exists (don't fail if not)

[ ! -e $STATE_DIR ] && mkdir $STATE_DIR
#exit 1
echo "Is this running ? ";

if [ -e "$STATE_DIR/lock" ]; then
	oldpid=$(cat "$STATE_DIR/lock")
	running=$(ps ax | egrep "\b$oldpid\b" | grep -v grep)
	if [[ -n "$running" && "$oldpid" != 0 && -n "$oldpid" ]]; then
		echo "yes (pid: $oldpid)";
		exit 1
	fi
	echo "Lock file found, but process '$oldpid' not running. Deleting lock."
	rm "$STATE_DIR/lock"
fi

echo $$ >"$STATE_DIR/lock"

echo It is not running

flavor=pluto_debug
export MakeRelease_Flavor="$flavor"
MR_ConfEval "$flavor"


function reportError
{
	echo "Build failed.";
	mkdir -p $SAVED_LOGS/$2;
	cp $3/*.log $SAVED_LOGS/$2/;

	if [ -e "$3/makepkg.log" ]; then
		(echo -e "Build failed $1. Attached are the last 50 lines of the relevant log file\n\n"; tail -n 50 $3/makepkg.log; echo "Ret code: $4") | mail -s "Build failure for revision: $2" "${Mail_Builder[@]}"
		return;
	fi
	
	if [ -e "$3/make.log" ]; then 
		(echo -e "Build failed $1. Attached are the last 50 lines of the relevant log file\n\n"; tail -n 50 $3/make.log; echo "Ret code: $4") | mail -s "Build failure for revision: $2" "${Mail_Builder[@]}"
		return;
	fi

	if [ -e "$3/svn-checkout.log" ]; then
		(echo -e "Build failed $1. Attached are the last 50 lines of the relevant log file\n\n"; tail -n 50 $3/svn-checkout.log; echo "Ret code: $4") | mail -s "Build failure for revision $2" "${Mail_Builder[@]}"
		return;
	fi
}

[ -z "$REPO_DIR" ] && echo Repository location not defined && exit 1;
[ ! -d $REPO_DIR ] && echo "\"$REPO_DIR\" is not a folder. Aborting!" && exit 1;

touch $STATE_DIR/last_revision
touch $STATE_DIR/last_revision_private
LAST_REV=`cat $STATE_DIR/last_revision`
LAST_REV_PRIV=`cat $STATE_DIR/last_revision_private`
THIS_REV=`sudo -u www-data svnlook youngest $REPO_DIR`;
THIS_REV_PRIV=`sudo -u www-data svnlook youngest $REPO_DIR-private`;

[[ "$LAST_REV" == "$THIS_REV" && "$LAST_REV_PRIV" == "$THIS_REV_PRIV" ]] && echo "No need to run the build (no changes detected: $LAST_REV == $THIS_REV, "$LAST_REV_PRIV" == "$THIS_REV_PRIV")" && exit 0;

echo Need to build \(version changed from $LAST_REV to $THIS_REV private from $LAST_REV_PRIV to $THIS_REV_PRIV\)


[ ! -d $BUILD_DIR ] && mkdir $BUILD_DIR 
pushd $BUILD_DIR

rm *.log

GetSVN()
{
	local Dir="$1" SVN_Dir="$2" Type="$3"
	local Suffix Revision

	case "$Type" in
		private)
			Suffix="-private"
			Revision="$THIS_REV_PRIV"
		;;
		*)
			Revision="$THIS_REV"
		;;
	esac
	
	if [[ -d "$Dir" ]]; then
		echo Updating "'$Dir'" to revision "$Revision"
		pushd "$Dir"
		svn revert -R .
		if ! svn update --username automagic --password "$(</etc/pluto/automagic.pwd)" -r "$Revision" &>../../svn-checkout.log; then
			reportError "updating checked out copy '$Dir' to revision $Revision" $Revision "$BUILD_DIR" $?
			exit 1
		fi
		popd
	else
		echo Checking out clean copy of "'$Dir'"
		mkdir -p "$Dir"
		if ! svn co --username automagic --password "$(</etc/pluto/automagic.pwd)" -r $Revision ${BASE_REPO_URL}${Suffix}/"$SVN_Dir" "$Dir" &> svn-checkout.log; then 
			reportError "checking out revision $Revision of '$Dir'" $Revision "$BUILD_DIR" $?
			exit 1
		fi
	fi
	MakeRelease_PrepFiles -p "$Dir" -e "*.cpp,*.h,Makefile*,*.php,*.sh,*.pl" -c /etc/MakeRelease/pluto_debug.conf
}

GetMercurial()
{
	local Dir="$1" Repository="$2"
	
	if [[ -d "$Dir" ]]; then
		echo Updating Mercurial repository for "'$Dir'"
		pushd "$Dir"
		hg pull -u &>../../svn-checkout.log
		if [ $? -gt 1 ]; then
			reportError "updating Mercurial repository for '$Dir'" $THIS_REV "$BUILD_DIR" $?
			exit 1
		fi
		popd
	else
		echo Cloning Mercurial repository for "'$Dir'"
		mkdir -p "$(dirname "$Dir")"
		pushd "$(dirname "$Dir")"
		if ! hg clone /home/sources/mercurial-repositories/"$Repository" &> svn-checkout.log; then 
			reportError "Cloning Mercurial repository for '$Dir'" $THIS_REV "$BUILD_DIR" $?
			exit 1
		fi
		popd
	fi
}

PrivateSymlinks()
{
	local PrivDir="$1" PublicDir="$2"
	local Dir BaseName
	
	# Links from private in public
	for Dir in $PrivDir/*; do
		[[ -L "$Dir" ]] && continue
		BaseName=$(basename "$Dir")
		rm -f "$PublicDir/$BaseName"
		ln -s "$(pwd)/$Dir" "$PublicDir"
	done

	# Links from public in private
	for Dir in $PublicDir/*; do
		[[ -L "$Dir" ]] && continue
		BaseName=$(basename "$Dir")
		rm -f "$PrivDir/$BaseName"
		ln -s "$(pwd)/$Dir" "$PrivDir"
	done
}

GetSVN sources/src trunk/src
GetSVN private-sources/src trunk/src private
PrivateSymlinks private-sources/src sources/src
GetMercurial sources/src/drivers/v4l-dvb v4l-dvb
GetSVN sources/libs trunk/libs
GetSVN sources/config-pkgs trunk/config-pkgs
GetSVN sources/web trunk/web
GetSVN sources/installers trunk/installers
GetSVN sources/misc_utils trunk/misc_utils

echo Building
export PATH=/usr/lib/ccache:$PATH
if !  MakeRelease -a -o 1 -m 1 -s $BUILD_DIR/sources -n / -R $THIS_REV -v 1 -c -V &>make.log; then 
	reportError "building revision $THIS_REV" $THIS_REV "$BUILD_DIR" $?;
	exit 1;
fi;

#mkdir -p /home/users/builder/build/sources/src/bin
cp /home/builds/Windows_Output/src/bin/* /home/users/builder/build/sources/src/bin
#cp /home/MakeRelease/trunk/src/database/*.sqlcvs /home/users/builder/build/sources/src/database
touch /home/users/builder/build/sources/src/database/{security,media,telecom,local,website,constants,dce,designer,document,ir,myth}.sqlcvs
touch /home/users/builder/build/sources/src/database/city.dump
touch /home/users/builder/build/sources/src/database_marbella/{dce,designer}.sqlcvs
touch /home/users/builder/build/sources/src/database_audi/{dce,designer}.sqlcvs
touch /home/users/builder/build/sources/src/database_monster/{dce,designer}.sqlcvs

if ! MakeRelease -b -a -o 1 -r 2,9,11 -m 1 -s /home/users/builder/build/sources -n / -v 1 -V -S &>makepkg.log; then
	reportError "building packages revision $THIS_REV" $THIS_REV "$BUILD_DIR" $?;
	exit 1;
fi;

rm /home/users/builder/build/sources/src/bin/*.exe
rm /home/users/builder/build/sources/src/bin/*.dll
rm /home/users/builder/build/sources/src/database/*.sqlcvs

popd

#echo "Build succesfull for revision $THIS_REV." | mail -s "Build $THIS_REV was ok" radu.c@plutohome.com

echo "Copying the updateBinaries.sh script"
cp $HOME/bin/updateBinaries.sh $HOME/build/sources/src/bin
echo Registring last built version
echo -n $THIS_REV > $STATE_DIR/last_revision
echo -n $THIS_REV_PRIV > $STATE_DIR/last_revision_private
echo Removing lock
rm "$STATE_DIR/lock"
rm /home/newhb_bin_*.tar.bz2
cd /home/users/builder/build/sources/src/bin
tar --exclude='*.exe' --exclude='*.dll' -jcvf /home/newhb_bin_$THIS_REV.tar.bz2 *

rm /home/newhb_lib_*.tar.bz2
cd /home/users/builder/build/sources/src/lib
tar --exclude='*.exe' --exclude='*.dll' -jcvf /home/newhb_lib_$THIS_REV.tar.bz2 *

cd /home/users/builder/bin
bash -x runSQLCVSChecks.sh

