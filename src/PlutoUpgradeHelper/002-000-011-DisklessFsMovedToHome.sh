#!/bin/bash

# Changed from 2.0.0.10 to 2.0.0.11: diskless filesystems were moved to /home
if [[ -f /etc/diskless.conf ]] ;then
	exit 0
fi

if [[ ! -L /usr/pluto/diskless && -d /usr/pluto/diskless ]]; then
	rm -rf /home/diskless
        mv /usr/pluto/diskless /home
else
        mkdir -p /home/diskless
fi
rm -rf /usr/pluto/diskless
ln -s /home/diskless /usr/pluto/diskless

