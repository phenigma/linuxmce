#!/bin/bash
/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :

## Create the main mount directory
mkdir -p /mnt/device

## Add a cron entry that scans for Internal Storage Devices
cronEntry="*/10 * * * * root /usr/pluto/bin/StorageDevices_Radar.sh"
#if ! grep -qF '/usr/pluto/bin/StorageDevices_Radar.sh' /etc/crontab; then
#        echo "$cronEntry" >>/etc/crontab
#        invoke-rc.d cron reload
#fi
if [[ ! -e /etc/cron.d/StorageDevicesRadar ]] ;then
	echo "$cronEntry" >>/etc/cron.d/StorageDevicesRadar
	invoke-rc.d cron reload
fi

## Add a init script on the core which will look for samba shares over the network
if [[ ! -f /etc/diskless.conf ]] ;then
	ln -s /usr/pluto/bin/StorageDevices_SambaRadar.sh /etc/rc2.d/StorageDevices_SambaRadar.sh || :
	ln -s /usr/pluto/bin/StorageDevices_SambaRadar.sh /etc/init.d/StorageDevices_SambaRadar.sh || :
fi

## Add a init script that will scan the devices for state info (online/ofline)
ln -s /usr/pluto/bin/StorageDevices_StatusRadar.sh /etc/rc2.d/StorageDevices_StatusRadar.sh || :
ln -s /usr/pluto/bin/StorageDevices_StatusRadar.sh /etc/init.d/StorageDevices_StatusRadar.sh || :

## Samba Share Helper 
if [[ "$(id -u sambahelper)" == "" ]] ;then
	useradd -c "Pluto Samba Share Helper" -d /tmp -s /bin/false sambahelper
fi

if [[ ! -f /usr/pluto/var/sambaCredentials.secret ]] ;then
	. /usr/pluto/bin/Utils.sh
	smbUser="sambahelper"
	smbPass=$(GeneratePassword)
	mkdir -p /usr/pluto/var
	echo "
username=$smbUser
password=$smbPass
" > /usr/pluto/var/sambaCredentials.secret
fi

chmod 600 /usr/pluto/var/sambaCredentials.secret

## Add the sambahelper user to smbpasswd
if [[ -r /usr/pluto/var/sambaCredentials.secret ]] ;then
	smbpass=$(cat /usr/pluto/var/sambaCredentials.secret | grep '^password' | cut -d '=' -f2)
	smbpass=$(/usr/pluto/bin/smbpass.pl $smbpass)

	smbuser=$(cat /usr/pluto/var/sambaCredentials.secret | grep '^user' | cut -d '=' -f2)
	smbuserid=$(id -u $smbuser)

	echo "$smbuser:$LinuxUserID:$smbpass:[U          ]:LCT-00000001:,,," >> /etc/samba/smbpasswd 
fi


