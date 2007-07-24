#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Config_Ops.sh

if [[ "$1" != "demonized" ]] ;then
	screen -S TestActions -d -m /bin/bash $0 demonized
	exit 0
fi

echo "incepem" > /var/starting.log
installed=$(dpkg -l mailutils | cut -d' ' -f1 | tail -1)
if [[ "$installed" != "ii" ]] ;then 
	chmod +x /tmp/AutoTests/Scripts/debconf-exim4.sh
	/tmp/AutoTests/Scripts/debconf-exim4.sh
	/bin/bash /tmp/AutoTests/Scripts/install-mail-pkgs.sh
fi

if ! grep -qF "dc_eximconfig_configtype='internet'" /etc/exim4/update-exim4.conf.conf ;then
	while read line ;do
                if [[ "$line" == "dc_eximconfig_configtype"* ]] ;then
			echo "dc_eximconfig_configtype='internet'" >> /etc/exim4/update-exim4.conf.conf.$$
		else
			echo "$line" >> /etc/exim4/update-exim4.conf.conf.$$
		fi
	done < /etc/exim4/update-exim4.conf.conf
	mv /etc/exim4/update-exim4.conf.conf.$$ /etc/exim4/update-exim4.conf.conf
	/etc/init.d/exim4 restart
fi
/bin/bash /tmp/AutoTests/Scripts/RotateLogs.sh
/bin/bash /tmp/AutoTests/Scripts/MoveLogRotate.sh
#/bin/bash /tmp/AutoTests/Scripts/Uncompress.sh


FK_Room=$(RunSQL "SELECT FK_Room FROM Device WHERE PK_Device='${PK_Device}'")
/bin/bash /tmp/AutoTests/Scripts/loop.sh $FK_Room
/bin/bash /tmp/AutoTests/Scripts/EnableOrbiterSimulator.sh

