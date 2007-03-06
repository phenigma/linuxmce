#!/bin/bash

. /etc/pluto/internal-mail.sh
. /home/WorkNew/src/MakeRelease/MR_Conf.sh
. /home/WorkNew/src/BootScripts/LockUtils.sh

for ((i = 1; i <= $#; i++)); do
	case "${!i}" in
		--force) Force=y ;; # bypass disabled flag; useful from command line after touching the ~/disabled file to prevent cron from interfering
	esac
done

if [[ -f ~/disabled && "$Force" != y ]]; then
	exit
fi

SVN_Repository_Dir=/home/sources/svn-repositories/pluto
SVN_Repository_BaseURL=http://10.0.0.170/pluto

State_Dir=/var/lib/pluto/build_state
Build_Dir=/home/MakeRelease_debug
Saved_Logs=/var/log/pluto/build

function reportError
{
	local Message="$1" Revision="$2" Build_Dir="$3" ExitCode="$4"
	
	if [[ -d "$Saved_Logs" ]]; then
		mkdir -p "$Saved_Logs"
	fi
	echo "Build failed.";
	mkdir -p "$Saved_Logs"/$Revision;
	cp $Build_Dir/*.log "$Saved_Logs"/$Revision/;

	if [ -e "$Build_Dir/makepkg.log" ]; then
		(echo -e "Build failed $Message. Attached are the last 50 lines of the relevant log file\n\n"; tail -n 50 $Build_Dir/makepkg.log; echo "Ret code: $ExitCode") | mail -s "CDHB: Build failure for revision: $Revision" "${Mail_Builder[@]}"
		return;
	fi
	
	if [ -e "$Build_Dir/make.log" ]; then 
		(echo -e "Build failed $Message. Attached are the last 50 lines of the relevant log file\n\n"; tail -n 50 $Build_Dir/make.log; echo "Ret code: $ExitCode") | mail -s "CDHB: Build failure for revision: $Revision" "${Mail_Builder[@]}"
		return;
	fi

	if [ -e "$Build_Dir/svn-checkout.log" ]; then
		(echo -e "Build failed $Message. Attached are the last 50 lines of the relevant log file\n\n"; tail -n 50 $Build_Dir/svn-checkout.log; echo "Ret code: $ExitCode") | mail -s "CDHB: Build failure for revision $Revision" "${Mail_Builder[@]}"
		return;
	fi
}

GetSVN()
{
	local Dir="$1" SVN_Dir="$2" Type="$3"
	local Suffix Revision

	case "$Type" in
		private)
			Suffix="-private"
			Revision="$Head_Revision_PRIV"
		;;
		*)
			Revision="$Head_Revision"
		;;
	esac
	
	if [[ -d "$Dir" ]]; then
		echo Updating "'$Dir'" to revision "$Revision"
		pushd "$Dir"
		svn revert -R .
		if ! svn update --username automagic --password "$(</etc/pluto/automagic.pwd)" -r "$Revision" &>../../svn-checkout.log; then
			reportError "updating checked out copy '$Dir' to revision $Revision" $Revision "$Build_Dir" $?
			exit 1
		fi
		popd
	else
		echo Checking out clean copy of "'$Dir'"
		mkdir -p "$Dir"
		if ! svn co --username automagic --password "$(</etc/pluto/automagic.pwd)" -r $Revision ${SVN_Repository_BaseURL}${Suffix}/"$SVN_Dir" "$Dir" &> svn-checkout.log; then 
			reportError "checking out revision $Revision of '$Dir'" $Revision "$Build_Dir" $?
			exit 1
		fi
	fi
}

GetMercurial()
{
	local Dir="$1" Repository="$2"
	
	if [[ -d "$Dir" ]]; then
		echo Updating Mercurial repository for "'$Dir'"
		pushd "$Dir"
		hg pull -u &>../../svn-checkout.log
		if [ $? -gt 1 ]; then
			reportError "updating Mercurial repository for '$Dir'" $Head_Revision "$Build_Dir" $?
			exit 1
		fi
		popd
	else
		echo Cloning Mercurial repository for "'$Dir'"
		mkdir -p "$(dirname "$Dir")"
		pushd "$(dirname "$Dir")"
		if ! hg clone /home/sources/mercurial-repositories/"$Repository" &> svn-checkout.log; then 
			reportError "Cloning Mercurial repository for '$Dir'" $Head_Revision "$Build_Dir" $?
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

CheckoutSourceCode()
{
	GetSVN trunk trunk
	GetSVN private/trunk trunk private
	PrivateSymlinks private/trunk/src trunk/src
	GetMercurial trunk/src/drivers/v4l-dvb v4l-dvb

	MakeRelease_PrepFiles -p "$Build_Dir"/trunk/src -e "*.cpp,*.h,Makefile*,*.php,*.sh,*.pl" -c /etc/MakeRelease/pluto_debug.conf
}

CompileSourceCode()
{
	echo Building
	if ! MakeRelease -a -o 1 -m 1 -s "$Build_Dir"/trunk -n / -R "$Head_Revision" -v 1 -c -V &>make.log; then
		reportError "building revision $Head_Revision" "$Head_Revision" "$Build_Dir" "$?"
		exit 1
	fi

	cp /home/builds/Windows_Output/src/bin/* "$Build_Dir"/trunk/src/bin
}

CreateDebs()
{
	# These should exist after a full build and it would be wrong to create empty files instead
	#touch "$Build_Dir"/trunk/src/database/{security,media,telecom,local,website,constants,dce,designer,document,ir,myth}.sqlcvs
	#touch "$Build_Dir"/trunk/src/database/city.dump
	#touch "$Build_Dir"/trunk/src/database_marbella/{dce,designer}.sqlcvs
	#touch "$Build_Dir"/trunk/src/database_audi/{dce,designer}.sqlcvs
	#touch "$Build_Dir"/trunk/src/database_monster/{dce,designer}.sqlcvs

	if ! MakeRelease -D main_sqlcvs_pluto_debug -g -b -a -o 1 -r 2,9,11 -m 1 -s "$Build_Dir"/trunk -n / -v 1 -V -O /home/builds/CDHB &>makepkg.log; then
		reportError "building packages revision $Head_Revision" "$Head_Revision" "$Build_Dir" "$?"
		exit 1
	fi
}

# make sure the file exists (don't fail if not)
if [[ ! -d "$State_Dir" ]]; then
	mkdir -p "$State_Dir"
fi
echo "Is this running ? "

if ! Lock "DebugBuild" "HourlyBuilder"; then
	echo "It's running"
	exit 1
fi
trap 'Unlock "DebugBuild" "HourlyBuilder"' EXIT

echo "It is not running"

flavor=pluto_debug
export MakeRelease_Flavor="$flavor"
ConfEval "$flavor"

if [[ -z "$SVN_Repository_Dir" ]]; then
	echo "Repository location not defined"
	exit 1
fi
if [[ ! -d "$SVN_Repository_Dir" ]]; then
	echo "\"$SVN_Repository_Dir\" is not a folder. Aborting!"
	exit 1
fi

touch "$State_Dir"/last_revision
touch "$State_Dir"/last_revision_private
Last_Revision=$(< $State_Dir/last_revision)
Last_Revision_PRIV=$(< $State_Dir/last_revision_private)
Head_Revision=$(sudo -u www-data svnlook youngest $SVN_Repository_Dir)
Head_Revision_PRIV=$(sudo -u www-data svnlook youngest $SVN_Repository_Dir-private)

if [[ "$Last_Revision" == "$Head_Revision" && "$Last_Revision_PRIV" == "$Head_Revision_PRIV" ]]; then
	echo "No need to run the build (no changes detected: $Last_Revision == $Head_Revision, "$Last_Revision_PRIV" == "$Head_Revision_PRIV")"
	exit 0
fi

echo "Need to build (version changed from $Last_Revision to $Head_Revision private from $Last_Revision_PRIV to $Head_Revision_PRIV)"

if [[ ! -d "$Build_Dir" ]]; then
	mkdir "$Build_Dir"
fi

export PATH=/usr/lib/ccache:"$PATH"
pushd "$Build_Dir"

rm -f *.log

/home/WorkNew/src/MakeRelease/UpdateVersion.sh
CheckoutSourceCode
CompileSourceCode
CreateDebs

popd

#echo "Build succesfull for revision $Head_Revision." | mail -s "Build $Head_Revision was ok" radu.c@plutohome.com

echo Registring last built version
echo "$Head_Revision" > "$State_Dir"/last_revision
echo "$Head_Revision_PRIV" > "$State_Dir"/last_revision_private
