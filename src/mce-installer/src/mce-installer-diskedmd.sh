#!/bin/bash

. /tmp/mce_wizard_data.sh
. /usr/pluto/bin/Utils.sh
. /usr/pluto/bin/SQL_Ops.sh
. ./mce-installer-common.sh


set -x

InstallerLogFile="/var/log/mce-installer-$(date +%s).log"
exec &> >(tee $InstallerLogFile)

echo "--- start mce wizard data ---"
cat /tmp/mce_wizard_data.sh
echo "--- end mce wizard data ---"


function Setup_NIS 
{
	if ! BlacklistConfFiles '/etc/passwd' ;then
		if ! grep -q "+::::::"   /etc/passwd ;then
			cat /etc/passwd  > /etc/passwd.$$
		       	echo "+::::::"  >> /etc/passwd.$$
			mv /etc/passwd.$$ /etc/passwd
		fi
	fi
	if ! BlacklistConfFiles '/etc/shadow' ;then
		if ! grep -q "+::::::::" /etc/shadow ;then
		       cat /etc/shadow   > /etc/shadow.$$
		       echo "+::::::::" >> /etc/shadow.$$
		       mv /etc/shadow.$$ /etc/shadow
	       fi
	fi
	if ! BlacklistConfFiles '/etc/group' ;then
		if ! grep -q "+:::" /etc/group ;then
			cat /etc/group  | grep -v "^public:" > /etc/group.$$
		       	echo "+:::"   >> /etc/group.$$
			mv /etc/group.$$ /etc/group
		fi
	fi
}

function Configure_Mounts
{
	
	local nfsOptions="intr,rsize=32768,wsize=32768,retrans=10,timeo=50"
	local homeDirs="cameras coredump coverartscan flickr logs mediapics mydvd mythtv public quick_start_icons securitypic temp_pvr tv_listing videowiz updates"
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
	apt-get -y install pluto-boot-scripts
	. /usr/pluto/install/activation.sh
}

function Setup_UI 
{
	## Set UI interface
	Q="SELECT PK_Device FROM Device WHERE FK_Device_ControlledVia='$c_deviceID' AND FK_DeviceTemplate=62"
	OrbiterDevice=$(RunSQL "$Q")

	case "$c_installUI" in
		"0")
			# select UI1
			UI_SetOptions "$OrbiterDevice" 0 0 1
		;;
		"1")
			# select UI2 without alpha blending
			UI_SetOptions "$OrbiterDevice" 1 0 4
		;;
		"2")
			# select UI2 with alpha blending
			UI_SetOptions "$OrbiterDevice" 1 1 4
		;;
		*)
			echo "Unknown UI version: $c_installUI"
		;;
	esac	
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


if [[ "$c_startupType" == "1" ]] ;then
	mv /etc/init.d/gdm /etc/init.d/gdm.saved
	mv /etc/init.d/kdm /etc/init.d/kdm.saved
	echo "
start on runlevel 2

stop on shutdown
stop on runlevel 3
stop on runlevel 4
stop on runlevel 5

script
/usr/pluto/bin/Startup_DiskedMD.sh
end script
" > /etc/event.d/pluto
fi

echo "
start on runlevel 2

stop on shutdown
stop on runlevel 3
stop on runlevel 4
stop on runlevel 5

script
/usr/pluto/bin/startup-script-diskedmd.sh
end script
" > /etc/event.d/media-cernter-startup

apt-get -f -y install pluto-mysql-wrapper
Setup_NIS
Configure_Mounts
Unpack_Config_Files
Setup_Apt_Conffiles
Run_Installer_Script
Setup_XOrg
Setup_UI
