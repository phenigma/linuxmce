#!/bin/bash

. /tmp/mce_wizard_data.sh

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

echo "Waiting 2 seconds .. "
sleep 2
#ExitInstaller "You Suck !"
