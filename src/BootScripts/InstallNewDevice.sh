#!/bin/bash

. /usr/pluto/bin/LockUtils.sh

exec >>/var/log/pluto/InstallNewDevice.newlog 2>&1 0</dev/null
echo -n "$(date -R) Called with parameters:"
for Parm in "$@"; do
	echo -n " $Parm"
done
echo

Device="$1"
Package="$2"

TryInstall()
{
	WaitLock "InstallNewDevice" "$Device"
	error=1
	retries=0
	while [[ $retries < 10 && $error != 0 ]]; do
		apt-get -y install "$Package"
		error=$?
		if [[ $error != 0 ]]; then
			echo "Try $retries failed with exit code $error"
			echo "--- Listing possibly useful information ---"
			echo "=== Process tree ==="
			ps axf
			echo "=== lsof|grep /var/lib/dpkg"
			lsof|grep -F /var/lib/dpkg
			echo "--- End of listing ---"
			sleep 5
		fi
		((retries++))
	done

	if [[ $retries == 10 ]]; then
		echo "All retries failed"
	else
		echo "Package installed successfully"
	fi

	Unlock "InstallNewDevice" "$Device"
}

if [[ -z "$Device" || -z "$Package" ]]; then
	echo "Bad parameters: '$Device', '$Package'"
	exit 1
fi

TryInstall
disown -a
