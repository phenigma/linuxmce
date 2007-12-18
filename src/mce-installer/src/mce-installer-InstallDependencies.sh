#!/bin/bash

. /tmp/mce_wizard_data.sh
. ./mce-installer-common.sh

Setup_Apt_Conffiles

rm -rf /tmp/mce_installer_error
InstallerLogFile="/var/log/mce-installer-$(date +%s).log"
echo "--- start mce wizard data ---" >> $InstallerLogFile
cat /tmp/mce_wizard_data.sh >> $InstallerLogFile
echo "--- end mce wizard data ---" >> $InstallerLogFile
exec &> >(tee $InstallerLogFile)

function ExitInstaller {
	echo 
	echo "ERROR : $*" >&2
	echo
	echo "$*" > /tmp/mce_installer_error
	exit 1
}

rm -f /home/public/data/samples/sample.mpg

FROM_CD=1
FROM_ISO=2

if [[ "$c_linuxmceCdFrom" == "$FROM_ISO" ]] ;then
	./mce-installer-LinuxMceCD.sh 'cache'
	if [[ "$?" != "0" ]] ;then
		ExitInstaller 'Could not cache LMCE CD 1'
	fi
fi

if [[ "$c_ubuntuExtraCdFrom" == "$FROM_ISO" ]] ;then
	./mce-installer-UbuntuExtraCD.sh 'cache'
	if [[ "$?" != "0" ]] ;then
		ExitInstaller 'Could not cache LMCE CD 2'
	fi
fi

if [[ "$c_ubuntuLiveCdFrom" == "$FROM_ISO" ]] ;then
	./mce-installer-UbuntuLiveCD.sh 'cache'
	if [[ "$?" != "0" ]] ;then
		ExitInstaller 'Could not cache Kubuntu Live CD'
	fi
fi


# remove Network Manager
apt-get -y -f remove network-manager brltty
# remove adept notifier
apt-get -y -f remove adept adept-notifier adept-batch adept-common adept-installer adept-manager adept-updater
/etc/init.d/networking restart

Setup_NIS
