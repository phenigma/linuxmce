#!/bin/bash

TrimSpace()
{
	echo "$1" | sed 's/^ *//; s/ *$//'
}

IP="$(TrimSpace "$1")"
PackageName="$(TrimSpace "$2")"
RepositoryURL="$(TrimSpace "$3")"
RepositoryName="$(TrimSpace "$4")"

if [ -z "$IP" ]; then
	echo "Invalid device selected"
	exit 1
fi

if [ -z "$PackageName" ]; then
	echo "Please input a package name"
	exit 1
fi

if [ -z "$RepositoryURL" ]; then
	echo "Please input a repository URL"
	exit 1
fi

if [ "${RepositoryURL#deb }" != "$RepositoryURL" -a -z "$RepositoryName" ]; then
	echo "You specified a Debian repository but didn't specify the name/section"
	exit 1
fi

ssh -l root -i /usr/pluto/keys/id_dsa_pluto_apache "$IP" "/usr/pluto/bin/InstallSoftware_Remote.sh '$IP' '$PackageName' '$RepositoryURL' '$RepositoryName'" || echo "Failed to contact destination computer"
