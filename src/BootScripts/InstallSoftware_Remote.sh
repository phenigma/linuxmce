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
		echo "$RepositoryURL $RepositoryName" >>/etc/apt/sources.list
	fi
	apt-get update &> >(tee -a /var/log/pluto/remote_install.newlog)
	apt-get -y install "$PackageName" &> >(tee -a /var/log/pluto/remote_install.newlog)
	RetCode="$?"

	[ "$RetCode" -ne 0 ] && Result="Installation failed (from Debian repository)"
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
