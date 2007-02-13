#!/bin/bash

set -e 

Moon_DeviceID=31
DestDir=$(mktemp -d)

function update_config_files
{
	local ScriptDir="/usr/pluto/bin/files.d"
	local ScriptsList="cron.d-synctime interfaces mythtv-mysql.txt nis-client pluto.conf resolv.conf syslog.conf timezone mountnfs.sh"
	for Script in $ScriptsList ;do
		if [[ ! -x $ScriptDir/$Script ]] ;then
			echo "WARNING: Script $Script cannot be executed"
			continue
		fi

		pushd $ScriptDir
			$ScriptDir/$Script --root $DestDir --device $Moon_DeviceID
		popd
	done
}

function build_installer_script 
{
	mkdir -p "${DestDir}/usr/pluto/install"
	local InstallerFiles="ConfirmDependencies_Debian.sh Common.sh"
	for file in $InstallerFiles ;do
		cp "/usr/pluto/install/${file}" "${DestDir}/usr/pluto/install"
	done

	/usr/pluto/bin/ConfirmDependencies -o 21 -r -D pluto_main -h dcerouter -u root -p '' -d $Moon_DeviceID install > "${DestDir}/usr/pluto/install/activation.sh"
}

function create_archive
{
	pushd ${DestDir}
		tar zcf /usr/pluto/install/Disked${Moon_DeviceID}.tar.gz *
	popd
}

update_config_files
build_installer_script
create_archive
