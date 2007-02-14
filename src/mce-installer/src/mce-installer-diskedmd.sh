#!/bin/bash


#if [[ ! -r /tmp/mce_wizard_data.sh ]] ;then
#	echo "ERROR: Cannot find wizard data";
#	exit 1

	c_coreIpAddress="192.168.80.1"
	c_plutoUser="dan"
	c_plutoPassword="dan"
	c_deviceID="31"
#fi

function Setup_NIS 
{
	grep -q "+::::::"   /etc/passwd || echo "+::::::"   >> /etc/passwd
	grep -q "+::::::::" /etc/shadow || echo "+::::::::" >> /etc/shadow
	grep -q "+:::"      /etc/group  || echo "+:::"      >> /etc/group
}

function Configure_Mounts
{
	
	local nfsOptions="intr,rsize=32768,wsize=32768,retrans=10,timeo=50"
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

	echo -e "$Content" >> /etc/fstab

	apt-get install -y portmap
	invoke-rc.d portmap start
	mount -a
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

#Setup_NIS
#Configure_Mounts
Unpack_Config_Files
Run_Installer_Script
