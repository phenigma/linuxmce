#!/bin/bash

IP="$1"
PackageName="$2"
RepositoryURL="$3"
RepositoryName="$4"

[ "${RepositoryURL#deb }" != "$RepositoryURL" ] && IsDeb="y"

date -R >>/var/log/pluto/remote_install.newlog

Result=""
if [ -n "$IsDeb" ]; then
	[ "${RepositoryName/ }" == "$RepositoryName" ] && RepositoryName="$RepositoryName main contrib non-free"
	echo "$RepositoryURL $RepositoryName #Pluto temporary" >>/etc/apt/sources.list
	apt-get update &> >(tee -a /var/log/pluto/remote_install.newlog)
	apt-get -y install "$PackageName" &> >(tee -a /var/log/pluto/remote_install.newlog)
	RetCode="$?"
	awk '!/#Pluto temporary/' /etc/apt/sources.list >/etc/apt/sources.list.$$
	mv /etc/apt/sources.list.$$ /etc/apt/sources.list

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
