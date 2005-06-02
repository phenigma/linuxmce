#!/bin/bash

if [[ ! -f /etc/pluto.conf ]]; then
	echo "/etc/pluto.conf doesn't exist. Can't do anything without it"
	exit 1
fi

PK_Installation=$(grep '^PK_Installation.*=' /etc/pluto.conf)
PK_Installation=${PK_Installation#*=}

if grep -q '^remote.*=' /etc/pluto.conf; then
	password=$(grep '^remote.*=' /etc/pluto.conf)
	password=${password#*=}
else
	password=
	while [[ -z "$password" ]]; do
		echo -n "Password for remote access (visible typing): "
		read password
	done
	echo "remote=$password" >>/etc/pluto.conf
fi

/usr/pluto/bin/SetupRemoteAccess.sh

echo
echo '-------------------------------------------'
echo "Pluto staff will ask you for these details:"
echo "Installation number: $PK_Installation"
echo "Remote assistance password: $password"
echo '-------------------------------------------'
