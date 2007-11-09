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

# remove Network Manager
apt-get -y -f remove network-manager brltty
# remove adept notifier
apt-get -y -f remove adept adept-notifier adept-batch adept-common adept-installer adept-manager adept-updater
/etc/init.d/networking restart

Setup_NIS
