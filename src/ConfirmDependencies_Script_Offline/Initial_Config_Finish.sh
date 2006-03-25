#!/bin/bash

. /usr/pluto/install/Common.sh

# At this point these scripts should be available (installation finished, didn't it?)
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Config_Ops.sh

chmod 777 /etc/pluto.conf &>/dev/null

Q="SELECT FK_DeviceCategory FROM DeviceTemplate JOIN Device ON FK_DeviceTemplate=PK_DeviceTemplate WHERE PK_Device='$PK_Device'"
R=$(RunSQL "$Q")

case "$R" in
	# Core
	7)
		SysType="Core"
	;;

	# Media Director
	8)
		SysType="Media Director"
	;;

	# Other (usually, this means it's a bug)
	*)
		SysType="Unknown Device Type '$R'"
	;;
esac

Message1="Congratulations.  Pluto installation has completed.  The system will now 
reboot.  The Pluto Core software will be started automatically.  As soon as 
the computer finishes rebooting you will be able to access the Pluto Admin
website to configure it, and you can start plugging in your media directors
and other plug-and-play devices.  
**Note**  The first boot will take an unusually long time while it sets up
a lot of new devices, and builds user interfaces for any Orbiters.
If you are an advanced Linux user and want to access a terminal before the
reboot, press ALT+F2.  Otherwise..."
Key="the Enter key"
#Key="OK"
Message2="Press $Key to reboot and startup your new Pluto $SysType."

Message24="
[1;5;31m*** WARNING ***[0m You installed the system using the 2.4 kernel.

The system will reboot using a 2.6 kernel, but the two versions see
Serial-ATA ports in different ways. If you only have Paralel-ATA units,
you can reboot without changes. Otherwise, read on.

For now, you have to manually update these files:
/etc/fstab:
	replace /dev/hdX* with /dev/sdY*
		X, Y = letters
		ex: /dev/hde -> /dev/sda; /dev/hde1 -> /dev/sda1
/boot/grub/menu.lst:
	For the \"Debian GNU/Linux, kernel 2.6.13-vanilla-pluto-1-686\" kernel entry you
	need to specify 'root=/dev/sdY1' instead of 'root=/dev/hdX1'

Switch to another console (Alt+F2), login as root and use an editor like
'nano' or 'vi' to edit those files before you reboot"

echo "$(CatMessages "$Message1" "$Message2")" | fmt
Kver=$(uname -r)
if [ "${Kver:0:4}" == "2.4." ]; then
	echo "$Message24"
fi
read
#MessageBox "$Message1" "$Message2" "$Message3"

# Set vimrc
[[ -e /root/.vimrc ]] || cp /usr/share/vim/vim63/vimrc_example.vim /root/.vimrc

# This is used so the offline apt mode (see OfflineMode.sh) can work
mkdir -p /etc/apt/apt.conf.d/
ln -s /usr/pluto/var/apt.conf.offline /etc/apt/apt.conf.d/99offline 2>/dev/null

echo "Setting debconf front-end to Noninteractive"
awk '/Name: debconf\/frontend/,/^$/ {if ($1 == "Value:") print "Value: Noninteractive"; else print; next}
	{print}' /var/cache/debconf/config.dat > /var/cache/debconf/config.dat.$$
mv /var/cache/debconf/config.dat.$$ /var/cache/debconf/config.dat
