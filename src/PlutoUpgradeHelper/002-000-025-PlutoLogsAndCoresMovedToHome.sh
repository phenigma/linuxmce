#!/bin/bash

# Changed from 2.0.0.24 to 2.0.0.25: pluto logs and core dumps were moved to /home

if [[ ! -L /usr/pluto/coredump && -d /usr/pluto/coredump ]]; then
        mv /usr/pluto/coredump /home
else
        mkdir -p /home/coredump
fi
rm -f /usr/pluto/coredump
ln -s /home/coredump /usr/pluto/coredump

if [[ ! -L /var/log/pluto && -d /var/log/pluto ]]; then
        mkdir -p /home/logs
        mv /var/log/pluto /home/logs	
	rm -f /var/log/pluto
	ln -s /home/logs/pluto /var/log/pluto
else
        mkdir -p /home/logs/pluto
fi


