#!/bin/bash

LogFile="/var/log/pluto/ApplyUpdates.log"
. /usr/pluto/bin/TeeMyOutput.sh --outfile "$LogFile" --append --stdboth -- "$@"

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

###############################################################################
# From AptUtils/AptSources.sh - Start
###############################################################################
AptSrc_AddSource()
{
	local Source="$*"
	local type uri distribution components
	local id
	local ret=1

	read type uri distribution components < <(echo "$Source")
	if [[ -z "$type" ]]; then
		return 1
	fi

	uri="${uri%/}/" # make sure URI ends with a slash
	id=$(echo "$type $uri $distribution" | md5sum | cut -d' ' -f1)

	if [[ "$AptSrc_id_list" != *" $id "* ]]; then
		AptSrc_id_list="$AptSrc_id_list$id "
		eval "AptSrc_base_${id}='$type $uri $distribution'"
		eval "AptSrc_comp_${id}=' '" # AptSrc_comp_* has a leading space
		ret=0
	fi
	for comp in $components; do
		Var="AptSrc_comp_${id}"
		if [[ "${!Var}" == *" $comp "* ]]; then # AptSrc_comp_* is bound and separated by spaces
			continue
		fi
		eval "AptSrc_comp_${id}=\"\${AptSrc_comp_${id}}$comp \"" # AptSrc_comp_* has a trailing space
		ret=0
	done

	return $ret
}

# Description: Clear internal state
AptSrc_Clear()
{
	local Var
	for Var in ${!AptSrc_*}; do
		unset $Var
	done
}

# Description: Load a sources.list file
AptSrc_ParseSourcesList()
{
	local SourcesList="${1:-/etc/apt/sources.list}"
	local Source type uri distribution components
	local id comp Var

	AptSrc_Clear

	AptSrc_id_list=' '
	while read Source; do
		Source="${Source%%#*}" # remove comments
		AptSrc_AddSource "$Source" || :
	done < "$SourcesList"
	AptSrc_WasParsed=AptSrc_WasParsed
}

# Description: Output currently memorized sources.list
# Note: file: sources go first, before net sources
AptSrc_WriteSourcesList()
{
	local VarBase VarComp
	local id
	local -a file_source net_source
	local Source

	file_source=()
	net_source=()
	for id in $AptSrc_id_list; do
		VarBase="AptSrc_base_${id}"
		VarComp="AptSrc_comp_${id}"
		Source="${!VarBase} ${!VarComp}"
		read type uri distribution components < <(echo "$Source")
		if [[ "$uri" == file:* ]]; then
			file_source=("${file_source[@]}" "$Source")
		else
			net_source=("${net_source[@]}" "$Source")
		fi
	done
	for Source in "${file_source[@]}" "${net_source[@]}"; do
		echo "$Source"
	done
}
###############################################################################
# From AptUtils/AptSources.sh - End
###############################################################################

## Check to see if this is a Core or a MD
RunningOnMD="No"
if [[ -f /etc/diskless.conf ]]; then
	RunningOnMD="Yes"
fi

## cleanup sources.list on the core and all its MDs
if [[ "$RunningOnMD" == "No" ]]; then
	# Cleanup core
	AptSrc_ParseSourcesList
	AptSrc_WriteSourcesList >/etc/apt/sources.list
	apt-get update
	# MDs
	for MediaDirector in /home/diskless/*; do # nullglob is in effect from Config_Ops.sh
		AptSrc_ParseSourcesList "$MediaDirector"/etc/apt/sources.list
		AptSrc_WriteSourcesList >"$MediaDirector"/etc/apt/sources.list
		chroot "$MediaDirector" apt-get update
	done
fi

## If a cron job for DownloadUpdates.sh is not allready made, do it now
if [[ $RunningOnMD == "No" ]] ;then
	cronEntry="0 3 * * * root /usr/pluto/bin/DownloadUpdates.sh"
	if ! grep -qF "$cronEntry" /etc/crontab; then
		echo "$cronEntry" >>/etc/crontab
		invoke-rc.d cron reload
	fi
fi

## Try to force NFS to sync so we can read the flags/locks in realtime
sync

## Check the existence of DownloadUpdates.lock
if [[ -f /usr/pluto/var/Updates/DownloadUpdates.lock ]]; then
	if [[ $RunningOnMD == "No" ]]; then
		# This machine is the Core => Powered off while DownloadUpdates.sh 
		# was runnig. We should clean up.
		rm -f /usr/pluto/var/Updates/DownloadUpdates.lock
	elif [[ $RunningOnMD == "Yes" ]];then
		# This machine is a MD => Core is doing a DownloadUpdates.sh right
		# now, we need to wait for it to finish.
		exit 0
	fi
fi

## Test if is ok to apply updates now else skip
if [[ ! -f /usr/pluto/var/Updates/UpdatesOk.flag ]]; then
	exit 0
else
	UpdatesOkStamp=$( cat /usr/pluto/var/Updates/UpdatesOk.flag )
fi

## What is our version now, should we update
LastUpdateStamp=0
if [[ -f /etc/lastUpdate.stamp ]]; then
	LastUpdateStamp=$( cat /etc/lastUpdate.stamp )
fi

if [[ "$LastUpdateStamp" == "$UpdatesOkStamp" ]]; then
	exit 0
fi

## Offline mode check
if [[ $OfflineMode == "true" ]]; then
	## Should check if apt wants to download any files
	bytesLeft=`apt-get dist-upgrade < /dev/null | grep '^Need to get' | cut -f4 -d' ' | cut -f1 -d'/'`
	if [[ "$bytesLeft" != "0B" && "$bytesLeft" != "" ]]; then
		## We have a problem. some packages neet to update from the internet
		## PS: We should never get here !!
		echo " - Some packages where not downloaded but we are in offline mode, exiting"
		exit 0
	fi
fi

## Check if a force upgrade was causing us to get here
ForceUpdatesStamp=$(cat /usr/pluto/var/Updates/ForceUpdates.stamp )
if [[ "$ForceUpdatesStamp" == "$UpdatesOkStamp" ]]; then
	echo "- Performing a force upgrade as requested"
	apt-get update
fi

## If we get till here than we should start upgrading
echo "- Previewing dist-upgrade"
InstPkgs="$(apt-get -s -f dist-upgrade | grep "^Conf " | cut -d' ' -f2 | tr '\n' ' ')"
RebootPkg="pluto-kernel-upgrade"
for Pkg in $RebootPkg; do
	if [[ "$InstPkgs" == *"$Pkg"* ]]; then
		echo "- A reboot will be performed after the updates are applied"
        	DoReboot=y
        	break
	fi
done

Count=$(apt-get -f -y -s dist-upgrade | egrep -c '^Inst |^Conf ')

echo "- Doing dist-upgrade (this can take some time)"
apt-get -V -f -y dist-upgrade || exit 1

if [[ "$Count" != "0" ]]; then
	Q="UPDATE Device SET NeedConfigure=1"
	RunSQL "$Q"
	if [[ "$DoReboot" == y ]]; then
		echo "New kernel installed. Rebooting"
		reboot
	fi
fi

echo "- Copying kernel package(s) for later use"
cp /var/cache/apt/archives/linux-image* /usr/pluto/deb-cache/dists/sarge/main/binary-i386/ 2>/dev/null

## Save our state to file
echo $UpdatesOkStamp > /etc/lastUpdate.stamp

### END ###
