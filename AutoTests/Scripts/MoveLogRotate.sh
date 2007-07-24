#!/bin/bash

mv /etc/logrotate.d/pluto /root

#move the old core dumps
if [[ ! -e /usr/pluto/coredump/old ]] ;then
        mkdir /usr/pluto/coredump/old
fi
mv /usr/pluto/coredump/*_* /usr/pluto/coredump/old/
	

