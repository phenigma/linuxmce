#!/bin/bash

#if [[ -z "$2" ]]; then # 1st install
#	/usr/pluto/bin/getxmls || :
#fi

ln -s /etc/init.d/instcheck.sh /etc/rc2.d/S23instcheck.sh || :
ln -s /etc/init.d/getxmls.sh /etc/rc2.d/S22getxmls.sh || :

if [[ ! -r /etc/diskless.conf ]] ;then
	invoke-rc.d getxmls.sh start
fi
