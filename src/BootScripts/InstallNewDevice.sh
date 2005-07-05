#!/bin/bash

exec >>/var/log/pluto/InstallNewDevice.newlog 2>&1

Device="$1"
Package="$2"

TryInstall()
{
	error=1
	retries=0
	while [[ $retries < 10 && $error != 0 ]]; do
		apt-get -y install "$Package"
		error=$?
		sleep 5
	done

	if [[ $retries == 10 ]]; then
		echo "All retries failed"
	else
		echo "Package installed successfully"
	fi

	rm /usr/pluto/locks/installing."$Device"
}

if [[ -z "$Device" || -z "$Package" ]]; then
	echo "Bad parameters: '$Device', '$Package'"
	exit 1
fi

touch /usr/pluto/locks/installing."$Device"
TryInstall &
disown -a
