#!/bin/bash

REPO_DIR=/home/sources/svn-repositories/pluto
BASE_REPO_URL=http://10.0.0.170/pluto/trunk/

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
	if [ -n "$running" ]; then
		echo "yes \(pid: $oldpid\)";
		exit 1
	fi
	echo "Lock file found, but process '$oldpid' not running. Deleting lock."
	rm "$STATE_DIR/lock"
fi

echo $$ >"$STATE_DIR/lock"

echo It is not running

DEST="mihai.t@newflavorstudio.com -c igor.s@newflavorstudio.com -c aaron@plutohome.com -c radu.c@newflavorstudio.com -c mtoader@gmail.com"

function reportError
{
	echo "Build failed.";
	mkdir -p $SAVED_LOGS/$2;
	cp $3/*.log $SAVED_LOGS/$2/;

	if [ -e "$3/make.log" ]; then 
		(echo -e "Build failed $1. Attached are the last 50 lines of the relevant log file\n\n"; tail -n 50 $3/make.log; echo "Ret code: $4") | mail -s "Build failure for revision: $2" $DEST
		return;
	fi

	if [ -e "$3/svn-checkout.log" ]; then
		(echo -e "Build failed $1. Attached are the last 50 lines of the relevant log file\n\n"; tail -n 50 $3/svn-checkout.log; echo "Ret code: $4") | mail -s "Build failure for revision $2" $DEST
		return;
	fi
}

[ -z "$REPO_DIR" ] && echo Repository location not defined && exit 1;
[ ! -d $REPO_DIR ] && echo "\"$REPO_DIR\" is not a folder. Aborting!" && exit 1;

touch $STATE_DIR/last_revision
LAST_REV=`cat $STATE_DIR/last_revision`
THIS_REV=`sudo -u www-data svnlook youngest $REPO_DIR`;

[ "$LAST_REV" = "$THIS_REV" ] && echo "No need to run the build (no changes detected: $LAST_REV == $THIS_REV)" && exit 0;

echo Need to build \(version changed from $LAST_REV to $THIS_REV\)


[ ! -d $BUILD_DIR ] && mkdir $BUILD_DIR 
pushd $BUILD_DIR

rm *.log

if [ -d sources/src ]; then
	echo Updating repository to revision $THIS_REV
	pushd sources/src
	if ! svn update -r $THIS_REV &>../../svn-checkout.log; then
		reportError "updating checked out copy to revision $THIS_REV" $THIS_REV "$BUILD_DIR" $?;
		exit 1;
	fi;
	popd
else
	echo Checking out clean copy
	mkdir -p sources/src
	if ! svn co -r $THIS_REV $BASE_REPO_URL/src sources/src &> svn-checkout.log; then 
		reportError "checking out revision $THIS_REV" $THIS_REV "$BUILD_DIR" $?;
		exit 1;
	fi;
fi;

if [ -d sources/libs ]; then
	echo Updating repository to revision $THIS_REV
	pushd sources/libs
	if ! svn update -r $THIS_REV &>../../svn-checkout.log; then
		reportError "updating checked out copy to revision $THIS_REV" $THIS_REV "$BUILD_DIR" $?;
		exit 1;
	fi;
	popd
else
	echo Checking out clean copy
	mkdir -p sources/libs
	if ! svn co -r $THIS_REV $BASE_REPO_URL/libs sources/libs &>svn-checkout.log; then 
		reportError "checking out revision $THIS_REV" $THIS_REV "$BUILD_DIR" $?;
		exit 1;
	fi;
fi;



echo Building
export PATH=/usr/lib/ccache:$PATH
if !  MakeRelease -a -o 1 -m 1 -s $BUILD_DIR/sources -n / -R $THIS_REV -v 1 -c &>make.log; then 
	reportError "building revision $THIS_REV" $THIS_REV "$BUILD_DIR" $?;
	exit 1;
fi;

mkdir -p /home/users/builder/build/sources/src/bin
cp /home/builds/Windows_Output/src/bin/* /home/users/builder/build/sources/src/bin
cp /home/MakeRelease/trunk/src/database/*.sqlcvs /home/users/builder/build/sources/src/database

if ! MakeRelease -b -a -o 1 -r 2,9,11 -m 1 -s /home/users/builder/build/sources -n / -v 1 -S &>makepkg.log; then
	reportError "building packages revision $THIS_REV" $THIS_REV "$BUILD_DIR" $?;
	exit 1;
fi;


popd

echo "Build succesfull for revision $THIS_REV." | mail -s "Build $THIS_REV was ok" mihai.t@newflavorstudio.com -c aaron@plutohome.com

echo Registring last built version
echo -n $THIS_REV > $STATE_DIR/last_revision
echo Removing lock
rm "$STATE_DIR/lock"
