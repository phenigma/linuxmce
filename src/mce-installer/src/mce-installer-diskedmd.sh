#!/bin/bash


#if [[ ! -r /tmp/mce_wizard_data.sh ]] ;then
#	echo "ERROR: Cannot find wizard data";
#	exit 1

	c_coreIpAddress="192.168.80.1"
	c_deviceID="31"
#fi

function Create_Device
{
	## Install libs needed by CreateDevice
	apt-get -y --force-yes install libmysqlclinet12 libhttpfetcher1
}

function Setup_NIS 
{
	grep -q "+::::::"   /etc/passwd || echo "+::::::"   >> /etc/passwd
	grep -q "+::::::::" /etc/shadow || echo "+::::::::" >> /etc/shadow
	grep -q "+:::"      /etc/group  || echo "+:::"      >> /etc/group
}

function Configure_Mounts
{
	
	local nfsOptions="intr,rsize=32768,wsize=32768,retrans=10,timeo=50"
	local homeDirs="cameras coredump coverartscan flickr logs mediapics mydvd mythtv public quick_start_icons securitypic temp_pvr tv_listing"
	local Content=""

	mkdir -p /usr/pluto/var
	mkdir -p /usr/pluto/orbiter
	mkdir -p /usr/pluto/keys
	mkdir -p /usr/pluto/deb-cache
	mkdir -p /usr/pluto/home

	Content="${Content}${c_coreIpAddress}:/usr/pluto/var /usr/pluto/var nfs $nfsOptions 0 0\n"
	Content="${Content}${c_coreIpAddress}:/usr/pluto/orbiter /usr/pluto/orbiter nfs $nfsOptions 0 0\n"
	Content="${Content}${c_coreIpAddress}:/usr/pluto/keys /usr/pluto/keys nfs $nfsOptions 0 0\n"
	Content="${Content}${c_coreIpAddress}:/usr/pluto/deb-cache /usr/pluto/deb-cache nfs $nfsOptions 0 0\n"
	Content="${Content}//${c_coreIpAddress}/home /usr/pluto/home cifs credentials=/usr/pluto/var/sambaCredentials.secret 0 0\n"

	grep -q "/usr/pluto/orbiter" /etc/fstab || echo -e "$Content" >> /etc/fstab

	apt-get install --force-yes -y portmap smbfs
	invoke-rc.d portmap start

	mount -a

	pushd /home
		for homeDir in $homeDirs ;do
			ln -s /usr/pluto/home/$homeDir
		done

		for homeDir in /usr/pluto/home/user_* ;do
			ln -s $homeDir
		done
	popd
}

function Unpack_Config_Files
{
	pushd /
		tar xvf /usr/pluto/var/Disked_${c_deviceID}.tar.gz
	popd
}

function Run_Installer_Script
{
	. /usr/pluto/install/activation.sh
}

if [ ! -e /etc/group.pbackup ] ;then
    cp /etc/group /etc/group.pbackup
fi

if [ ! -e /etc/passwd.pbackup ] ;then
   cp /etc/passwd /etc/passwd.pbackup
fi

if [ ! -e /etc/shadow.pbackup ] ;then
   cp /etc/shadow /etc/shadow.pbackup
fi
			
Setup_NIS
Configure_Mounts
Unpack_Config_Files
Run_Installer_Script
Create_Device
