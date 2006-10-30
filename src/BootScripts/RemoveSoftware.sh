#!/bin/bash

TrimSpace()
{
	echo "$1" | sed 's/^ *//; s/ *$//'
}

IP="$(TrimSpace "$1")"
PackageName="$(TrimSpace "$2")"

if [[ -z "$IP" ]]; then
	echo "Invalid device selected"
	exit 1
fi

if [[ -z "$PackageName" ]]; then
	echo "Please input a package name"
	exit 1
fi

/usr/pluto/bin/LaunchRemoteCmd.sh "$IP" "/usr/pluto/bin/RemoveSoftware_Remote.sh '$IP' '$PackageName'"
#ssh -l root -i /usr/pluto/keys/id_dsa_pluto_apache "$IP" "/usr/pluto/bin/InstallSoftware_Remote.sh '$IP' '$PackageName' '$RepositoryURL' '$RepositoryName'" || echo "Failed to contact destination computer"
