#!/bin/bash

IP="$1"
PackageName="$2"
RepositoryURL="$3"
RepositoryName="$4"

[ "${RepositoryURL#deb }" != "$RepositoryURL" ] && IsDeb="y"

Result=""
if [ -n "$IsDeb" ]; then
	[ "${RepositoryName/ }" == "$RepositoryName" ] && RepositoryName="$RepositoryName main contrib non-free"
	echo "$RepositoryURL $RepositoryName #Pluto temporary" >>/etc/apt/sources.list
	apt-get update
	apt-get -y install "$PackageName"
	RetCode="$?"
	awk '!/#Pluto temporary/' /etc/apt/sources.list >/etc/apt/sources.list.$$
	mv /etc/apt/sources.list.$$ /etc/apt/sources.list

	[ "$RetCode" -ne 0 ] && Result="Installation failed"
else
	wget -P /tmp "$RepositoryURL/$PackageName"
	if [ "$?" -eq 0 ]; then
		dpkg -i /tmp/$PackageName || Result="Installation failed"
	else
		Result="Download failed"
	fi
	rm -f /tmp/$PackageName
fi

[ -z "$Result" ] && Result="Installation complete"

echo "$Result"
