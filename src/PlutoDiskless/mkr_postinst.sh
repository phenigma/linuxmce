#!/bin/bash

## Create the /home/diskless and the /usr/pluto/diskless symlink
if [[ ! -d /home/diskless ]] ;then
	mkdir -p /home/diskless
fi

if [[ ! -L /usr/pluto/diskless && -d /usr/pluto/diskless ]]; then
        mv /usr/pluto/diskless /home
fi

if [[ ! -L /usr/pluto/diskless ]] ;then
	ln -s /home/diskless /usr/pluto/diskless
fi

## This is safe to be runned at every install/upgrade
/usr/pluto/bin/Diskless_Setup.sh --skiplock || /bin/true
/usr/pluto/bin/DHCP_config.sh || /bin/true
/usr/pluto/bin/Diskless_ExportsNFS.sh || /bin/true
