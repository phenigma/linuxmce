#!/bin/bash

IP="$1"
PackageName="$2"
RepositoryURL="$3"
RepositoryName="$4"

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

echo ssh "$IP" "/usr/pluto/bin/InstallSoftware_Remote.sh '$IP' '$PackageName' '$RepositoryURL' '$RepositoryName'"
