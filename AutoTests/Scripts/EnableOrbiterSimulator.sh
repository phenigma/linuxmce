#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Config_Ops.sh

paramEnable=1

	
## Do the job for 89 MDs
for md in /usr/pluto/diskless/* ;do
	
	## Enable generator for Orbiter
	touch $md/etc/Orbiter.conf
	if grep -q "^ *EnableGenerator *=" $md/etc/Orbiter.conf ;then
		sed "s/^ *EnableGenerator *=.*/EnableGenerator=$paramEnable/g" $md/etc/Orbiter.conf > $md/etc/Orbiter.conf.sed
		mv $md/etc/Orbiter.conf.sed $md/etc/Orbiter.conf
	else
		echo                                >> $md/etc/Orbiter.conf
		echo "EnableGenerator=$paramEnable" >> $md/etc/Orbiter.conf
	fi

	## Enable generator for BluetoothDongle
	touch $md/etc/BluetoothDongle.conf
	if grep -q "^ *EnableGenerator *=" $md/etc/BluetoothDongle.conf ;then
		sed "s/^ *EnableGenerator *=.*/EnableGenerator=$paramEnable/g" $md/etc/BluetoothDongle.conf > $md/etc/BluetoothDongle.conf.sed
		mv $md/etc/BluetoothDongle.conf.sed $md/etc/BluetoothDongle.conf
	else
		echo                                >> $md/etc/BluetoothDongle.conf
		echo "EnableGenerator=$paramEnable" >> $md/etc/BluetoothDongle.conf
	fi

done

/usr/pluto/bin/MessageSend $DCERouter 0 -1000 7 1
