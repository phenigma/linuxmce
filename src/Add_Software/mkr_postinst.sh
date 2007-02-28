#!/bin/bash

ln -s /etc/init.d/instcheck.sh /etc/rc2.d/S23instcheck.sh || :

if [[ ! -f /etc/diskless.conf && -z "$2" ]] ;then
	/usr/pluto/bin/getxmls &>/dev/null </dev/null &
fi											
