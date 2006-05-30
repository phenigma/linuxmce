#!/bin/bash

. /usr/pluto/bin/LockUtils.sh

IP="$1"
PackageName="$2"
RepositoryURL="$3"
RepositoryName="$4"

IsDeb=
NoAptSource=
if [[ "$RepositoryURL" == "deb "* ]]; then
	IsDeb="y"
fi

if [[ -z "$RepositoryURL" && -z "$RepositoryURL" && -z "$RepositoryName" ]]; then
	IsDeb="y"
	NoAptSource="y"
fi

LogFile=/var/log/pluto/remote_install.log
date -R >>"$LogFile"

Result=""
if [ -n "$IsDeb" ]; then
	WaitLock "InstallNewDevice" "InstallSoftware_Remote"
	[[ "$RepositoryName" != *" "* ]] && RepositoryName="$RepositoryName main contrib non-free"
	if [[ -z "$NoAptSource" ]] && ! grep -qF "$RepositoryURL $RepositoryName" /etc/apt/sources.list; then
		echo "$RepositoryURL $RepositoryName" |tee "/etc/apt/sources.list.test"
		if ! apt-get -o Dir::Etc::SourceList="/etc/apt/sources.list.test" --no-list-cleanup update &> >(tee -a "$LogFile"); then
			Result="Installation failed (from Debian repository): Source failed the test."
			rm "/etc/apt/sources.list.test"
		fi
	fi

	if [[ -z "$Result" ]]; then
		if [[ -f "/etc/apt/sources.list.test" ]]; then
			cat "/etc/apt/sources.list.test" >>/etc/apt/sources.list
			rm "/etc/apt/sources.list.test"
		fi

		apt-get update
		echo "($?) apt-get update ($?)" >> $LogFile

		apt-get -y install "$PackageName" 
		RetCode="$?"
		echo "($RetCode) apt-get -f install $PackageName" >> $LogFile

		if [[ "$RetCode" != 0 ]]; then
			Result="Installation failed (from Debian repository)"
		fi
	fi
	Unlock "InstallNewDevice" "InstallSoftware_Remote"
else
	wget -P /tmp "$RepositoryURL/$PackageName"
	RetCode=$?
	echo "($RetCode) wget -P /tmp $RepositoryURL/$PackageName" >> $LogFile

	if [ "$RetCode" -eq 0 ]; then
		WaitLock "InstallNewDevice" "InstallSoftware_Remote"

		dpkg -i /tmp/$PackageName
		echo "($?) dpkg -i /tmp/$PackageName"

		Unlock "InstallNewDevice" "InstallSoftware_Remote"
	else
		Result="Download failed (direct link)"
	fi

	rm -f /tmp/$PackageName
fi

[ -z "$Result" ] && Result="Installation complete" || Err="yes"

echo "$Result"
[ -n "$Err" ] && exit 10 || exit 0
