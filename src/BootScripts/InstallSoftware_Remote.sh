#!/bin/bash

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

date -R >>/var/log/pluto/remote_install.newlog

Result=""
if [ -n "$IsDeb" ]; then
	[[ "$RepositoryName" != *" "* ]] && RepositoryName="$RepositoryName main contrib non-free"
	if [[ -z "$NoAptSource" ]] && ! grep -qF "$RepositoryURL $RepositoryName" /etc/apt/sources.list; then
		echo "$RepositoryURL $RepositoryName" |tee "/etc/apt/sources.list.test"
		if ! apt-get -o Dir::Etc::SourceList="/etc/apt/sources.list.test" --no-list-cleanup update &> >(tee -a /var/log/pluto/remote_install.newlog); then
			Result="Installation failed (from Debian repository): Source failed the test."
			rm "/etc/apt/sources.list.test"
		fi
	fi

	if [[ -z "$Result" ]]; then
		if [[ -f "/etc/apt/sources.list.test" ]]; then
			cat "/etc/apt/sources.list.test" >>/etc/apt/sources.list
			rm "/etc/apt/sources.list.test"
		fi
		apt-get update &> >(tee -a /var/log/pluto/remote_install.newlog)
		apt-get -y install "$PackageName" &> >(tee -a /var/log/pluto/remote_install.newlog)
		RetCode="$?"

		if [[ "$RetCode" != 0 ]]; then
			Result="Installation failed (from Debian repository)"
		fi
	fi
else
	wget -P /tmp "$RepositoryURL/$PackageName" &> >(tee -a /var/log/pluto/remote_install.newlog)
	if [ "$?" -eq 0 ]; then
		dpkg -i /tmp/$PackageName || Result="Installation failed (direct link)" &> >(tee -a /var/log/pluto/remote_install.newlog)
	else
		Result="Download failed (direct link)"
	fi
	rm -f /tmp/$PackageName
fi

[ -z "$Result" ] && Result="Installation complete" || Err="yes"

echo "$Result"
[ -n "$Err" ] && exit 10 || exit 0
