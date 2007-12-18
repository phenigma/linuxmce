#!/bin/bash

if [[ "$1" != "background" ]] ;then
	screen -d -m -S Upgrade_Download "$0" background
	exit 0
fi

mkdir -p /var/gutsy-upgrade-scripts/ 

DOWNLOAD_PID_FILE="/var/gutsy-upgrade-scripts/download.pid"
DOWNLOAD_PID=$(cat "$DOWNLOAD_PID_FILE")

if [[ "$DOWNLOAD_PID" != "" ]] ;then
	if [[ ! -d "/proc/$DOWNLOAD_PID" ]] ;then
		echo "$$" > "$DOWNLOAD_PID_FILE"
	else
		exit 1
	fi
else
	echo "$$" > "$DOWNLOAD_PID_FILE"
fi

#TODO: Implement download / unpack
wget --directory-prefix=/var/gutsy-upgrade-scripts/ --continue --tries=5 --timeout=5  --output-file=/var/gutsy-upgrade-scripts/download.log \
	ftp://ftp.iasi.roedu.net/mirrors/ubuntulinux.org/releases/7.10/ubuntu-7.10-alternate-amd64.iso || exit 1

## Generate a list containig all orbiter ids ($OrbiterIDList)
. /usr/pluto/bin/SQL_Ops.sh
Q="
        SELECT
                PK_Device
        FROM
                Device
                JOIN DeviceTemplate ON PK_DeviceTemplate = FK_DeviceTemplate
                JOIN DeviceCategory ON PK_DeviceCategory = FK_DeviceCategory
        WHERE
                PK_DeviceCategory IN (5,2,3)
"
OrbiterList=$(RunSQL "$Q")

OrbiterIDList=""
for Orbiter in $OrbiterList; do
        if [[ $OrbiterIDList != "" ]]; then
                OrbiterIDList="$OrbiterIDList,"
        fi

        Orbiter_ID=$(Field 1 "$Orbiter")
        OrbiterIDList="$OrbiterIDList""$Orbiter_ID"
done
/usr/pluto/bin/MessageSend localhost 0 $OrbiterIDList 1 741 10 "UpgradeFinished" 159 53 183 0 9 "Finished downloading 0710 Update. You need to reboot you Hybrid/Core to install the updates."



touch /var/gutsy-upgrade-scripts/upgrade-ready
